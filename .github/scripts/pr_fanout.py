#!/usr/bin/env python3

"""
PR Fanout Script
------------------
This script takes a list of changed subtrees in `category/name` format and for each:
    - Pushes the corresponding subtree directory from the monorepo to the appropriate branch in the sub-repo using `git subtree push`.
    - Creates or updates a pull request in the sub-repo with a standardized branch and label.

Arguments:
    --repo      : Full repository name (e.g., org/repo)
    --pr        : Pull request number
    --subtrees  : A newline-separated list of subtree paths in category/name format (e.g., projects/rocBLAS)
    --config    : OPTIONAL, path to the repos-config.json file
    --dry-run   : If set, will only log actions without making changes.
    --debug     : If set, enables detailed debug logging.

Example Usage:

    To run in debug mode and perform a dry-run (no changes made):
        python pr-fanout.py --repo ROCm/rocm-libraries --pr 123 --subtrees "$(printf 'projects/rocBLAS\nprojects/hipBLASLt\nshared/rocSPARSE')" --dry-run --debug
"""

import argparse
import subprocess
import logging
from typing import List, Optional
from github_cli_client import GitHubCLIClient
from repo_config_model import RepoEntry
from config_loader import load_repo_config
from utils_fanout_naming import FanoutNaming

logger = logging.getLogger(__name__)

def parse_arguments(argv: Optional[List[str]] = None) -> argparse.Namespace:
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(description="Fanout monorepo PR to sub-repos.")
    parser.add_argument("--repo", required=True, help="Full  repository name (e.g., org/repo)")
    parser.add_argument("--pr", required=True, type=int, help="Pull request number")
    parser.add_argument("--subtrees", required=True, help="Newline-separated list of changed subtrees (category/name)")
    parser.add_argument("--config", required=False, default=".github/repos-config.json", help="Path to the repos-config.json file")
    parser.add_argument("--dry-run", action="store_true", help="If set, only logs actions without making changes.")
    parser.add_argument("--debug", action="store_true", help="If set, enables detailed debug logging.")
    return parser.parse_args(argv)

def get_subtree_info(config: List[RepoEntry], subtrees: List[str]) -> List[RepoEntry]:
    """Return config entries matching the given subtrees in category/name format."""
    requested = set(subtrees)
    matched = [
        entry for entry in config
        if f"{entry.category}/{entry.name}" in requested
    ]
    missing = requested - {f"{e.category}/{e.name}" for e in matched}
    if missing:
        logger.warning(f"Some subtrees not found in config: {', '.join(sorted(missing))}")
    return matched

def subtree_push(entry: RepoEntry, branch: str, prefix: str, subrepo_full_url: str, dry_run: bool) -> None:
    """Push the specified subtree to the sub-repo using `git subtree push`."""
    # the output for git subtree push spits out thousands of lines for history preservation, suppress it
    push_cmd = ["git", "subtree", "push", "--prefix", prefix, subrepo_full_url, branch, "--quiet"]
    logger.debug(f"Running: {' '.join(push_cmd)}")
    if not dry_run:
        # explicitly set the shell to bash if possible to avoid issue linked, which was hit in testing
        # https://stackoverflow.com/questions/69493528/git-subtree-maximum-function-recursion-depth
        # we also need to increase python's recursion limit to avoid hitting the recursion limit in the subprocess
        try:
            result = subprocess.run(
                push_cmd,
                check=True,
                capture_output=True,
                text=True,
            )
            logging.debug(f"subtree push stdout:\n{result.stdout}")
            logging.debug(f"subtree push stderr:\n{result.stderr}")
        except subprocess.CalledProcessError as e:
            logging.error(f"subtree push failed with exit code {e.returncode}")
            logging.error(f"stdout:\n{e.stdout}")
            logging.error(f"stderr:\n{e.stderr}")
            raise RuntimeError("git subtree push failed — see logs for details.") from e

def main(argv: Optional[List[str]] = None) -> None:
    """Main function to execute the PR fanout logic."""
    args = parse_arguments(argv)
    logging.basicConfig(
        level=logging.DEBUG if args.debug else logging.INFO
    )
    client = GitHubCLIClient()
    config = load_repo_config(args.config)
    # Key in on intersection between the subtrees input argument (new-line delimited) and the config file contents
    subtrees = [line.strip() for line in args.subtrees.splitlines() if line.strip()]
    relevant_subtrees = get_subtree_info(config, subtrees)
    for entry in relevant_subtrees:
        entry_naming = FanoutNaming(
            pr_number = args.pr,
            monorepo = args.repo,
            category = entry.category,
            name = entry.name,
            subrepo = entry.url
        )
        logger.debug(f"\nProcessing subtree: {entry_naming.prefix}")
        logger.debug(f"\tBranch: {entry_naming.branch_name}")
        logger.debug(f"\tRemote: {entry_naming.subrepo_full_url}")
        logger.debug(f"\tPR title: {entry_naming.pr_title}")
        subtree_push(entry, entry_naming.branch_name, entry_naming.prefix, entry_naming.subrepo_full_url, args.dry_run)
        pr_exists = client.pr_view(entry.url, entry_naming.branch_name)
        if not pr_exists:
            logger.debug(f"No PR found for branch {entry_naming.branch_name} in {entry.url}")
            # check if the branch already exists in the subrepo and error out if it did not
            # means git subtree push failed
            check_branch_subprocess = subprocess.run(
                ["git", "ls-remote", "--heads", entry_naming.subrepo_full_url, entry_naming.branch_name],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                check=True, text=True
            )
            if not args.dry_run:
                if bool(check_branch_subprocess.stdout.strip()):
                    # entry.branch is the default branch for the subrepo
                    # entry_naming.branch_name is the pull request branch name
                    client.pr_create(entry.url, entry.branch, entry_naming.branch_name, entry_naming.pr_title, entry_naming.pr_body)
                    logger.info(f"Created PR in {entry.url} for branch {entry_naming.branch_name}")
                else:
                    logger.error(f"Branch {entry_naming.branch_name} does not exist in {entry.url}. Cannot create PR.")
            else:
                logger.info(f"[Dry-run] Would create PR in {entry.url} for branch {entry_naming.branch_name}")
        else:
            logger.info(f"PR already exists for {entry.url}#{entry_naming.branch_name}. subtree push is enough.")

if __name__ == "__main__":
    main()
