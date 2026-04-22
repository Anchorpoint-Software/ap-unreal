
# 0.61

- Allow ini file check-ins from the editor

# 0.60

- Always allow writing to ini files
- Add launch argument to use CLI args

# 0.59

- Revert now covers everything, not only files listed in the message
- Replace unattended script with a dirty flag reset
- Prevent save dialogs when starting sync
- Always reply for files changed, even if there is an error
- Add support for querying the CLI version
- Delegate unchanged-file checks to the desktop app before pulling

# 0.58

- Unregister the post-tick delegate after the window is closed

# 0.57

- Handle newly added files as AddInMemory to prevent the checkout dialog
- Thread-safe multicast
- Use engine-relative paths for ini commands to avoid unicode issues from username or project name

# 0.56

- Pointer protection for subsystem access

# 0.55

- Add mutex for all static caches to prevent race conditions
- Update docs link

# 0.54

- Prevent infinite status loads during checkout
- Add mutex for the status cache
- Add logout support and logout info in the status bar
- Check login status for availability and force cache skip during connect requests
- Delay the connect command until login is confirmed

# 0.53

- Fixes for auto-checkout
- Refresh asset status while the checkout modal is open
- More status messages and user settings surfaced in the status message
- Support for the Unreal 5.7 source control provider

# 0.52

- Fix MacOS compilation for callbacks
- Run a forced update when the Submit Content button is pressed
- Fix double modal crash by delaying the modal until the previous one completes

# 0.51

- Support for config files
- Support for the experimental navmesh
- Allow querying the state of files that have not been created yet
- Extend modal detection support for additional types

# 0.50

- Display username instead of email for file lockers
- Fix default brush being returned instead of querying the parent for GetOptionalBrush
- Fix icon in the submit window
- Fix window being forced in front of modals
- Outdated files now support Modified and Unmodified states
- Only display the connection warning when Anchorpoint is the selected source control provider
- Show a popup when conflicts are detected, with a cache to avoid duplicate popups
- Click the warning to open Anchorpoint
- Optimize revert operation to delete files directly, speeding up renames
- Outdated state now has higher priority than normal changes

# 0.49

- Fix error during cooking for the Slate app

# 0.48

- Maintenance release

# 0.47

- Maintenance release

# 0.46

- Use project connection state instead of CLI connection state
- Display full prompts when the CLI is not connected
- Disable the Keep Files Checked Out modal
- Status bar icon now reflects the CLI status
- Add scoped timer profiling

# 0.45

- Fix multiple OFPA deletion by deleting files directly instead of using git rm
- Display extra logs when handling information during shutdown
- Improve parsing to handle messages split across newlines

# 0.44

- Fix process end being called while the thread is still running during a light build cleanup

# 0.43

- Maintenance release

# 0.42

- Wait for commands to complete during shutdown to fix a shutdown crash

# 0.41

- Fix crash during shutdown when executing commands after unload
- Re-use the last cache when receiving a project dirty message while waiting for a new status
- Use the last cached response even if a request is in progress
- Fix special characters being ignored when sending data to the Anchorpoint CLI
- Add support for unicode decoding
- Reduce log spam when message parsing fails
- Disable verbose logging for end users

# 0.40

- Remove unwanted git-specific files from the FAB package

# 0.39

- Fix FAB packaging regressions

# 0.38

- Download implementation for both commit id and oid
- Fix false errors shown during LFS check-in

# 0.37

- Display version on the provider status and remove version from the descriptor

# 0.36

- Fix version checks for 5.3
- Fix missing plugin dependency
- Fix clone link

# 0.35

- Remove the packaging script
- Add missing URL field

# 0.34

- Add PlatformAllowList property for all modules
- Allow extra files to be submitted

# 0.32

- Add a release action to build the plugin for multiple engine versions
- Improve error messages for checkout failure
- Parse multiple messages received in a single read
- String polish (points and commas adjusted)

# 0.31

Initial public version of the plugin, providing integration between Unreal Engine and the Anchorpoint desktop application via a CLI, source control provider, and in-editor UI.

- Disable project saved prompts when submitting from Unreal
- Advanced merge options with conflict handling and file download from a merge command
- Toast notifications for errors and connection state
- Checkout progress bar and notifications
- Support for the ANCHORPOINT_ROOT environment variable to point at a custom installation
- Prepare plugin for FAB with updated README, descriptor links, and build-from-source instructions
- Fix crash on startup caused by an invalid class being loaded
- Add success message for the post-submit popup
- Merge-in-Anchorpoint button shown in the proper place
- Create a write-permission pipe to send messages back to Anchorpoint
- Fix wrong color displayed on base version 5.5.0
- Fix mismatched colors for 5.4
- Fix uncompleted statuses getting cached
- Add notification on connection success
- Switch autolock to be disabled project-wide
- Show popup for a select list of commands and conditions
- Fix missing include for packaging
- Custom red icon, adapted to the engine version
- Anchorpoint style class
- Improved setup flow for when Anchorpoint is not installed or the project is not initialised
- New CLI class with support for longer messages
- Integrate history into the source control update command
- Implement conflict state
- Support for fake messages via console commands
- Add mutex to avoid multiple updates at the same time
