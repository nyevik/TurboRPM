# ADR 0001: Context menu for package table

## Context
The package table currently allows only button-driven actions, which makes per-row operations cumbersome. Users asked for right-click menus that adapt to the column (e.g., name vs. size) to expose contextual actions like opening package details or converting size units. We also need placeholder wiring for future actions while keeping the GUI responsive and lightweight.

## Decision
Add a custom context menu to the package table view that inspects the clicked column and builds column-specific actions. For the Name column we expose placeholder actions ("Get more information", "Check for updates"); for the Size column we expose conversion actions (KB/MB/GB) backed by helper slots that parse the byte value and display the converted amount. The menu wiring relies on existing Qt signals/slots without new dependencies and records the source model index to keep conversions tied to the selected row.

## Consequences
- Users get consistent right-click behavior with column-aware options.
- Future features can replace the placeholder slots without altering the menu wiring.
- Additional columns can be supported by extending the switch statement in the context menu handler.
- Parsing assumes the size field is numeric bytes; malformed values produce a warning dialog instead of crashing.
