# 0002: Privileged package actions via sudo session

## Context
- TurboRPM currently runs `dnf`/`rpm` directly from the UI, so starting as an unprivileged user leaves install/remove/update commands without a way to elevate.
- The UI must show whether the session is limited or administrative and let the user request or drop administrative access without restarting.
- The codebase already centralizes process execution in `MainWindow::runCommand` and should avoid new dependencies beyond Qt and system tools.

## Decision
- Detect root at startup with `geteuid`; treat root sessions as always administrative.
- Add an access banner with lock/unlock icons and a toggle button to request or drop administrative access.
- When administrative access is requested, validate a sudo session using `sudo -S -v` with a GUI password prompt; cache only the sudo timestamp, not the password.
- Execute privileged commands through `sudo -n <cmd>` when not running as root; clear the sudo timestamp with `sudo -K` to return to limited access.
- Track the elevation state inside `MainWindow` and update the banner (icons, text, button state) whenever the state changes or sudo authentication fails.

## Consequences
- No new runtime dependencies; relies on the existing `sudo` binary and Qt Widgets.
- Administrative tasks (install/remove/check-update) prompt once per sudo timeout window; failures that mention missing authentication drop back to limited mode.
- Limited mode still supports read-only queries; some actions are intentionally gated to avoid accidental privileged execution.
- Sessions started as root cannot truly drop privileges; the banner clarifies that administrative access is always active in that case.
