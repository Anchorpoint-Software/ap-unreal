param (
    [string]$UnrealPath,
    [string[]]$UnrealVersions,
    [string]$destinationBase,
    [string]$source
)
Remove-Item -Path "$destinationBase" -Recurse -Force

foreach ($UnrealVersion in $UnrealVersions) {
    $destination = "$destinationBase\$UnrealVersion"
    $RunUAT = "$UnrealPath$UnrealVersion\Engine\Build\BatchFiles\RunUAT.bat"

    &"$RunUAT" BuildPlugin -Plugin="$source/Anchorpoint.uplugin" -Package="$destination"

    Remove-Item -Path "$destination/Source/Anchorpoint/Private" -Recurse -Force
    Remove-Item -Path "$destination/Source/Anchorpoint/Public" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointCli/Private" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointCli/Public" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointSync/Private" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointSync/Public" -Recurse -Force
}