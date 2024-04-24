![Software build](https://github.com/JesseSwildens/Software-ontwikkeling-VESOFTON/actions/workflows/main.yml/badge.svg?branch=main)

# Software-ontwikkeling-VESOFTON

# Docs
https://jesseswildens.github.io/Software-ontwikkeling-VESOFTON/

# Contributing guidelines
Contributing to the project comes with the following steps:

## Issues
Create an issue, or assign one from the scrumboard. This issue has a number and a description. All the documentation, comments or other relevant documentation must be posted under the issue. It is important to keep information bundled together.

## Branches
After issue creation, a branch can be created linked to the issue. All branches follow the same naming convention: `[branch number]-[relevant title]`. Any changes relevant to the issue can be made on this branch. Commit often to keep a clear history.

## Commits
The commit description must include two things:
1. \# Issue number
2. A short description of the commit

An example: `#16 fixed stuck in inf loop in API_Draw_line`.

The issue number links the commit to the relevant issue and adds it to its history.

## Merging
After the issue is deemed complete, it can be merged via a pull request (PR). A PR has to pass several  checks (some automatic) before it can be merged into the main branch:

1. (Automatic) The project must build
2. (Automatic) The unit tests must pass (WIP)
3. (Automatic) The style checker must pass (WIP)
4. A manual review must approve of the following:
    - Check for code coherence
    - Check for code clarity
    - Check for naming convention
    - Check for comment contents and style
    - Crosscheck code with the issue description

After all these checks are passed, the issue owner may merge the branch with the main branch.

