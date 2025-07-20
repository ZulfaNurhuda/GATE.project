# Contributing to GATE

We love contributions from the community! Whether it's a bug report, a new feature, or a documentation improvement, your help is always welcome. Please take a moment to review this document to make the contribution process as smooth as possible.

## How Can I Contribute?

### Reporting Bugs

If you find a bug, please open an issue on our GitHub repository. When reporting a bug, please include:

*   A clear and concise description of the bug.
*   Steps to reproduce the behavior.
*   Expected behavior.
*   Actual behavior.
*   Screenshots or error messages (if applicable).
*   Your operating system and compiler version.

### Suggesting Enhancements

We welcome suggestions for new features or improvements. Please open an issue on GitHub and describe your idea in detail. Explain why you think it would be a valuable addition to GATE.

### Code Contributions

If you'd like to contribute code, please follow these steps:

1.  **Fork the repository** on GitHub.
2.  **Clone your forked repository** to your local machine.
    ```bash
    git clone https://github.com/your-username/GATE.project.git
    cd GATE.project
    ```
3.  **Create a new branch** for your feature or bug fix.
    ```bash
    git checkout -b feature/your-feature-name
    # or
    git checkout -b bugfix/fix-description
    ```
4.  **Make your changes.** Ensure your code adheres to the existing coding style and conventions.
5.  **Write tests** for your changes. All new features and bug fixes should have corresponding unit or integration tests.
6.  **Run tests** to ensure everything is working correctly and no existing functionality is broken.
    ```bash
    make tests
    ./bin/run_tests
    ```
7.  **Build the project** to ensure there are no compilation errors.
    ```bash
    make
    ```
8.  **Commit your changes** with a clear and concise commit message. Follow the [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/) specification (e.g., `feat: add new feature`, `fix: resolve bug`).
    ```bash
    git add .
    git commit -m "feat: add amazing new feature"
    ```
9.  **Push your branch** to your forked repository.
    ```bash
    git push origin feature/your-feature-name
    ```
10. **Open a Pull Request (PR)** on the main GATE repository. Provide a detailed description of your changes and reference any related issues.

## Code Style and Conventions

*   **Indentation:** Use 2 spaces for indentation.
*   **Naming:** Follow PascalCase for class names, camelCase for function and variable names, and SCREAMING_SNAKE_CASE for constants.
*   **Comments:** Add comments where necessary to explain complex logic or non-obvious parts of the code.
*   **Error Handling:** Utilize the existing `ErrorHandler` for reporting errors.
*   **AST Structure:** Adhere to the existing AST node definitions and visitor pattern.

## License

By contributing to GATE, you agree that your contributions will be licensed under the MIT License. See the `LICENSE` file for more details.

Thank you for contributing to GATE! ✨
