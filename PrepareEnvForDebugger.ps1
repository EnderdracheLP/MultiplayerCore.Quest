param(
    [Parameter(Mandatory=$false, HelpMessage="Whether to set the environment var for user")]
    [Alias("SetSystemEnvironment")]
    [Switch]
    $System
)

$CurrentCommandPath = $MyInvocation.MyCommand.Path

function Set-EnvironmentVariable
{
  param
  (
    [Parameter(Mandatory=$true)]
    [String]
    $Name,
    
    [Parameter(Mandatory=$true)]
    [String]
    $Value,
    
    [Parameter(Mandatory=$true)]
    [EnvironmentVariableTarget]
    $Target
  )

  # This line below will also set the variable in the current session so no restart is required
  [System.Environment]::SetEnvironmentVariable($Name, $Value)

    If ($Target -eq [EnvironmentVariableTarget]::Machine -And -NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(`
    [Security.Principal.WindowsBuiltInRole] "Administrator"))
    {
        Write-Warning "Setting environment variables system wide requires Administrator rights!`nRequesting rights and restarting script"
        timeout /T 5
        Start-Process -FilePath PowerShell.exe -Verb Runas -ArgumentList "-File `"$($CurrentCommandPath)`" -System"
        exit 0;
    }

  [System.Environment]::SetEnvironmentVariable($Name, $Value, $Target)
  Write-Host "Environment variable set successfully"
  Write-Host "Restart Visual Studio Code for the new environment to take effect"
  Write-Host "Make sure you have the 'Oculus Debugger' Extension installed"
  timeout /T 10
}

if ($System) {
    Set-EnvironmentVariable -Name 'ANDROID_SDK_ROOT' -Value "$env:APPDATA\SideQuest" -Target Machine
}
else {
    Set-EnvironmentVariable -Name 'ANDROID_SDK_ROOT' -Value "$env:APPDATA\SideQuest" -Target User
}