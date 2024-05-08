![Software build](https://github.com/JesseSwildens/Software-ontwikkeling-VESOFTON/actions/workflows/main.yml/badge.svg?branch=main)

# Software-ontwikkeling-VESOFTON

# Docs
https://jesseswildens.github.io/Software-ontwikkeling-VESOFTON/

# Contributing guidelines
Contributing to the project comes with the following steps:

## Glossary
*Must*: Rule must be followed at all times. Excpetions are exceptionally rare and must be discussed with the team.

*Should*: Rule should be followed as much as possible. Exceptions are possible.

## Issues
Create an issue, or assign one from the scrumboard. This issue has a number and a description. All the documentation, comments or other relevant documentation must be posted under the issue. It is important to keep information bundled together.

## Branches
After issue creation, a branch must be created, linked to the issue. All branches must follow the same naming convention: `[branch number]-[relevant title]`. Any changes relevant to the issue can be made on this branch. Commit often to keep a clear history.

## Commits
The commit description must include two things:
1. \# Issue number
2. A short description of the commit

An example: `#16 fixed stuck in inf loop in API_Draw_line`.

The issue number links the commit to the relevant issue and adds it to its history.

"Temporary" or "Syncing" commits like "Latest WIP" are explicitly not allowed.

## Merging
After the issue is deemed complete, it must be merged via a pull request (PR). A PR must pass several  checks (some automatic) before it can be merged into the main branch:

1. (Automatic) The project must build
2. (Automatic) The unit tests must pass (WIP)
3. (Automatic) The style checker must pass 
4. A manual review must approve of the following:
    - Check for code functionality
    - Check for code coherence
    - Check for code clarity
    - Check for naming convention
    - Check for comment contents and style
    - Crosscheck code with the issue description!

After all these checks are passed, the PR owner should merge the branch with the main branch.

## Other standards

### Files
All files must follow the following naming convention:

Source files: `*.cpp\` or `*.c`

Header files: `*.h`

The name of the file must be in snake_case and cover its contents. It's encouraged to keep files as simple as possible. 

### Naming convention
All *functions* part of the API must contain the prefix `API_`. 

Other important prefixes:
- `BL_`, Business Layer
- `AL`, Application Layer