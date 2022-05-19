param(
    [Parameter(Mandatory=$false, HelpMessage="Whether to set the environment var for user")]
    [Alias("SetSystemEnvironment")]
    [Switch]
    $System
)

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
  [System.Environment]::SetEnvironmentVariable($Name, $Value, $Target)
  # This line below will also set the variable in the current session so no restart is required
  [System.Environment]::SetEnvironmentVariable($Name, $Value)
}

if ($System) {
    If (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(`
    [Security.Principal.WindowsBuiltInRole] "Administrator"))
    {
        Write-Warning "You do not have Administrator rights to run this script!`nRequesting in 5 seconds or press Enter to continue"
        timeout /T 5
        Start-Process -FilePath PowerShell.exe -Verb Runas -ArgumentList "-File `"$($MyInvocation.MyCommand.Path)`" -System"
        exit 0;
    }

    Set-EnvironmentVariable -Name 'ANDROID_SDK_ROOT' -Value "$env:APPDATA\SideQuest" -Target Machine
}
else {
    Set-EnvironmentVariable -Name 'ANDROID_SDK_ROOT' -Value "$env:APPDATA\SideQuest" -Target User
}