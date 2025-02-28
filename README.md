# Anchorpoint plugin for Unreal

This plugin allows revision control to be handled directly from the Unreal editor. If you’re working in a team, it’s highly recommended that you use the plugin, as it gives you more control over your modified files. Unreal Engine can communicate with the Anchorpoint desktop application via an IPC connection. 

## Features

- Pull, revert and reset project files in Anchorpoint without closing Unreal Engine.
- Check for unsaved files before committing changes
- Open the Anchorpoint Browser from any asset in the Unreal Editor
- Instantly check for locked files

In addition, the Anchorpoint revision control plugin for Unreal Engine allows you to

- Run checkout workflows and lock files exclusively
- See who locked what file in the Unreal Editor
- Run blueprint diffs to see what you have changed
- View individual file history, resolve merge conflicts and revert individual files
- Commit your changes from the Unreal Editor

During the commit process, you will need to wait for Anchorpoint to commit your files and check the Git repository for updates. Once Anchorpoint starts committing your files in the background, you can continue working on your project. It's also a good idea to keep the Anchorpoint project open while you're working with the plugin, to speed up the process of detecting changes.

## Basic usage

Put the compiled plugin in your project/plugins or in your engine folder. Check the [documentation](https://docs.anchorpoint.app/docs/version-control/first-steps/unreal/) on how to use it with Anchorpoint.

## Contribution

We appreciate any kind of contribution via a pull request. If you have other ideas for features or other improvements, please join our [Discord](https://discord.com/invite/ZPyPzvx) server.


## Building from Source

To build the plugin from source in your project:

1. Create a `Plugins` folder in the root folder of your project (where the `.uproject` file is located).
2. Run in your terminal:
```
cd `Plugins`
git clone https://github.com/configcat/unreal-engine-sdk.git
```

3. Right-click the `.uproject` file and select `Generate Visual Studio project files`.
4. Open the .sln to run via Visual Studio or double-click the `.uproject` and you will be prompted to build the plugin.