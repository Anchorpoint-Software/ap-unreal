param (
    [string]$UnrealPath,
    [string[]]$UnrealVersions,
    [string]$destinationBase,
    [string]$source
)
Remove-Item -Path "$destinationBase" -Recurse -Force

# Pwsh has access to $IsWindows, $IsMacOS, $IsLinux however version 5 of powershell does not have these variables
if($IsWindows -Or $PSVersionTable.PSVersion.Major -eq 5) {
    $UatFile = "RunUAT.bat"
}
if($IsMacOS) {
    $UatFile = "RunUAT.sh"
}

foreach ($UnrealVersion in $UnrealVersions) {
    $destination = "$destinationBase/$UnrealVersion"
    $RunUAT = "$UnrealPath$UnrealVersion\Engine\Build\BatchFiles\$UatFile"

    &"$RunUAT" BuildPlugin -Plugin="$source/Anchorpoint.uplugin" -Package="$destination"

    Remove-Item -Path "$destination/Source/Anchorpoint/Private" -Recurse -Force
    Remove-Item -Path "$destination/Source/Anchorpoint/Public" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointCli/Private" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointCli/Public" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointSync/Private" -Recurse -Force
    Remove-Item -Path "$destination/Source/AnchorpointSync/Public" -Recurse -Force
}