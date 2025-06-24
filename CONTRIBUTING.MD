# Contributing to Conqueror Engine

Thank you for your interest in contributing to the Conqueror Engine project! We welcome improvements, bug fixes, and feature additions from the community. This document details our contribution guidelines.

## How to Contribute

1. **Reporting Issues:**  
   - Before reporting a bug or suggesting an enhancement, please check existing issues to avoid duplicates.
   - Create a detailed issue on the repository's issue tracker with a clear description, steps to reproduce (for bugs), and any relevant logs or screenshots.

2. **Submitting Pull Requests:**  
   - Fork the repository and create a new branch with your changes.
   - Ensure your code adheres to the project's coding style and guidelines.
   - Include clear commit messages explaining your changes.
   - Test your changes and ensure that the build passes locally. For Emscripten builds, verify that your changes compile to WebAssembly and function correctly.
   - Submit a pull request against the main branch. A project maintainer will review your PR and provide feedback.

3. **Coding Guidelines:**  
   - Use clear and descriptive variable/function/class names.
   - Write modular, well-documented code. Every module should include a brief header comment describing its purpose.
   - Prefer standard C++ practices and follow modern C++ (C++11 or later) conventions.
   - For JavaScript modules (e.g., ES6 modules used by the loader), use clear imports and comments to explain integration points.
   - Maintain consistency across projects files, including Markdown documentation and shell scripts.

4. **Documentation:**  
   - Update the README.md and CHANGELOG.md files with any significant changes or new features.
   - Add inline comments to your code to explain complex sections or provide context where necessary.

## Branching and Merging

- We follow a **Git Flow**–style workflow:
  - **Feature branches:** Create branches for each new feature or bug fix.
  - **Pull Requests:** Merge changes via pull requests into the `main` branch after review and approval.
  - **Rebasing:** Please keep your branches rebased against the latest state of the `main` branch to simplify integration.

## Code of Conduct

Please note that this project has a [Code of Conduct](CODE_OF_CONDUCT.md). Contributors are expected to adhere to it to ensure a welcoming, respectful, and collaborative environment for all.

## Getting Started

- Clone the repository:
  ```bash
  git clone https://github.com/yourusername/conqueror-engine.git
