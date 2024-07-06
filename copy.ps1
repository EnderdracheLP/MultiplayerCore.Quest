Param (
[Parameter(Mandatory=$false, HelpMessage="Switch to create a clean compilation")][Alias("rebuild")][Switch]$clean,
[Parameter(Mandatory=$false, HelpMessage="Switch to build and copy only")][Alias("copyOnly")][Switch]$NoStart,
[Parameter(Mandatory=$false)]
[Switch] $useDebug
)
& $PSScriptRoot/build.ps1 -debugbuild -clean:$clean
if ($?) {

    qpm qmod manifest
    $modJson = Get-Content "./mod.json" -Raw | ConvertFrom-Json

    foreach ($fileName in $modJson.modFiles) {
        if ($useDebug -eq $true) {
            & adb push build/debug/$fileName /sdcard/ModData/com.beatgames.beatsaber/Modloader/early_mods/$fileName
        } else {
            & adb push build/$fileName /sdcard/ModData/com.beatgames.beatsaber/Modloader/early_mods/$fileName
        }
    }

    foreach ($fileName in $modJson.lateModFiles) {
        if ($useDebug -eq $true) {
            & adb push build/debug/$fileName /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/$fileName
        } else {
            & adb push build/$fileName /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/$fileName
        }
    }

    if ($? -and -not $NoStart) {
        adb shell am force-stop com.beatgames.beatsaber
        adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
    }
    if ($?-and $args[0] -eq "--log") {
        & Start-Process PowerShell -ArgumentList "./logging.ps1 --file"
    }
    if ($? -and $args[0] -eq "--debug") {
        $timestamp = Get-Date -Format "MM-dd HH:mm:ss.fff"
        adb logcat -T "$timestamp" main-modloader:W QuestHook[QuestSounds`|v0.3.0]:* AndroidRuntime:E *:S QuestHook[UtilsLogger`|v1.2.3]:*
    }
} else {
    exit 1;
}
Write-Output "Exiting Copy.ps1"