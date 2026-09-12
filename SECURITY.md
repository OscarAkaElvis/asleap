# Security Policy

## Supported Versions

Only the latest released version of asleap receives security and compatibility fixes.

## Reporting a Vulnerability

Please do not report suspected security vulnerabilities through public GitHub issues.

Use GitHub's private vulnerability reporting feature for this repository. Include enough information to reproduce and assess the issue, such as:

- The affected asleap version.
- The operating system and architecture.
- The steps required to reproduce the issue.
- The expected and observed behavior.
- Relevant compiler or runtime output.
- A minimal test case, when practical.

Please use only controlled test data. Do not include real credentials, private packet captures, API keys, tokens, or other sensitive information.

Security reports will be reviewed as maintainer availability permits. No specific response or resolution timeframe is guaranteed.

## Scope

Reports concerning implementation vulnerabilities in asleap are welcome, including:

- Memory-safety issues.
- Unsafe packet, capture, dictionary, or password-database processing.
- Unexpected code execution.
- Security-relevant crashes or undefined behavior.
- Vulnerabilities introduced by build, installation, or compatibility changes.

asleap implements password recovery for legacy authentication protocols. The known weaknesses of LEAP, PPTP, MS-CHAPv2, DES, MD4, and other legacy mechanisms used by the tool are part of its intended purpose and are not, by themselves, vulnerabilities in asleap.

For non-security bugs, compatibility problems, and feature requests, please use the repository's public GitHub issue templates.
