#!/usr/bin/env pwsh
Param(
    [Parameter(Mandatory=$false)]
    [Switch] $self,

    [Parameter(Mandatory=$false)]
    [Switch] $all,

    [Parameter(Mandatory=$false)]
    [String] $custom="",

    [Parameter(Mandatory=$false)]
    [Switch] $file="",

    [Parameter(Mandatory=$false)]
    [Switch] $help,

    [Parameter(Mandatory=$false)]
    [Switch] $excludeHeader
)

if ($help -eq $true) {
    if ($excludeHeader -eq $false) {
        Write-Output "`"Start-Logging`" - Logs Beat Saber using `"adb logcat`""
        Write-Output "`n-- Arguments --`n"
    }

    Write-Output "-Self `t`t Only Logs your mod and Crashes"
    Write-Output "-All `t`t Logs everything, including logs made by the Quest itself"
    Write-Output "-Custom `t Specify a specific logging pattern, e.g `"custom-types|questui`""
    Write-Output "`t`t NOTE: The paterent `"AndriodRuntime|CRASH`" is always appended to a custom pattern"
    Write-Output "-File `t`t Saves the output of the log to the file name given"

    exit
}

$bspid = adb shell pidof com.beatgames.beatsaber
$command = "adb logcat "

# if ($all -eq $false) {
    $loops = 0
    while ([string]::IsNullOrEmpty($bspid) -and $loops -lt 6) {
        Start-Sleep -Milliseconds 100
        $bspid = adb shell pidof com.beatgames.beatsaber
        $loops += 1
    }

    if ([string]::IsNullOrEmpty($bspid)) {
        Write-Output "Could not connect to adb, exiting..."
        exit 1
    }

    $command += "--pid $bspid"
# }

if ($all -eq $false) {
    $pattern = "("
    if ($self -eq $true) {
        $pattern += "MultiplayerCore|"
    }
    if (![string]::IsNullOrEmpty($custom)) {
        $pattern += "$custom|"
    }
    if ($pattern -eq "(") {
        $pattern = "(QuestHook|modloader|"
    }
    $pattern += "AndroidRuntime|CRASH)"
    $command += " | Select-String -pattern `"$pattern`""
}

if ($file -eq $true) {
    $command += " | Out-File -FilePath $PSScriptRoot\..\log.log"
}

Write-Output "Logging using Command `"$command`""
Invoke-Expression $command
