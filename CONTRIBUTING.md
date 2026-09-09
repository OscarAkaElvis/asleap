# Contributing

Thank you for your interest in contributing to asleap.

This repository is a maintained fork of a mature legacy project. Contributions should focus on compatibility, correctness, portability, and preservation of the original behavior.

For substantial changes, please open an issue first so the scope and proposed approach can be discussed.

## Issue Creation Policy

Search existing issues before opening a new one and confirm that the problem occurs with the latest applicable version or the current `dev` branch.

All issues must be written in English and include:

- The asleap version from `./asleap -V`.
- The exact commit when testing `dev`.
- The operating system, distribution version, and architecture.
- The OpenSSL version from `openssl version -a`.
- Compiler, Make, and relevant dependency versions for build problems.
- How asleap was installed or built.
- The exact command that produced the problem.
- Complete error messages and relevant output.
- Clear steps to reproduce the issue.
- The expected and actual behavior.

Provide text output instead of screenshots whenever practical.

Use controlled test data. Do not publish real credentials, private packet captures, API keys, tokens, or other sensitive information.

Issues that omit essential version information, reproduction steps, or requested diagnostic details may be marked as invalid and closed. Issues may also be closed if the reporter does not provide the additional information required to investigate them.

## Git Workflow

The `master` branch represents the latest stable release. Active development and pre-release testing take place on `dev`.

All contributor pull requests must target `dev`. Pull requests targeting `master` are not accepted as part of the normal development workflow and may be closed without merging.

The usual workflow is:

1. Create a focused branch from the latest `dev`.
2. Make and test the proposed change.
3. Update the branch with the latest changes from `dev`.
4. Open a pull request targeting `dev`.
5. Address review comments and provide any requested test results.

Changes are reviewed and tested in `dev` before release. Every pull request from `dev` to `master` represents a new release and is managed by the project maintainer.

Contributors should not create release tags, GitHub releases, or version bumps unless explicitly requested by the maintainer.

## Pull Request Guidelines

Pull requests must describe the problem, explain the solution, remain limited to one logical change, and reference the related issue when one exists.

Include the environments and exact commands used for testing. Clearly distinguish between compilation, startup, and functional testing, and mention anything that could not be tested.

Commits should be small, focused, and written in English. Preserve original authorship and contributor attribution when adapting existing work.

Avoid unrelated cleanup, reformatting, refactoring, or new dependencies.

## Code and Documentation Style

Follow the style of the surrounding code and the repository's `.editorconfig` rules:

- Use UTF-8 encoding.
- Use LF line endings.
- Insert a final newline.
- Use four spaces for indentation in C source files, headers, and Perl scripts.
- Preserve mandatory tabs in Makefile recipes.
- Do not introduce trailing whitespace or unrelated formatting changes.

All repository content must be written in English, including source code, comments, documentation, commit messages, error messages, and similar project content.

## Testing

A basic validation sequence is:

```sh
make clean
make
./asleap -V
./asleap -h
ldd ./asleap
ldd ./genkeys
```

No dynamic library should be reported as `not found`.

A successful build alone is not sufficient for changes affecting authentication calculations, password recovery, cryptographic compatibility, packet processing, or file formats. Relevant changes must receive functional testing using deterministic test vectors or controlled data.

Changes involving OpenSSL, DES, MD4, or portability should be tested on more than one relevant environment when practical. Do not claim that a platform was tested unless the test was actually performed.

## Maintenance Scope

Preserve the original command-line interface, behavior, purpose, attribution, and historical code wherever practical.
