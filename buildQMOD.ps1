Param(
    [Parameter(Mandatory=$false, HelpMessage="The name the output qmod file should have")][String] $qmodname="MultiplayerCore",

    [Parameter(Mandatory=$false, HelpMessage="Switch to create a clean compilation")]
    [Alias("rebuild")]
    [Switch] $clean,

    [Parameter(Mandatory=$false, HelpMessage="Prints the help instructions")]
    [Switch] $help,

    [Parameter(Mandatory=$false, HelpMessage="Tells the script to not compile and only package the existing files")]
    [Alias("actions", "pack")]
    [Switch] $package,

    [Parameter(Mandatory=$false, HelpMessage="The version of the mod")][String] $version,

    [Parameter(Mandatory=$false, HelpMessage="To create a release build")][Alias("publish")][Switch]$release = $false
)

# Builds a .qmod file for loading with QP or BMBF


if ($help -eq $true) {
    echo "`"BuildQmod <qmodName>`" - Copiles your mod into a `".so`" or a `".a`" library"
    echo "`n-- Parameters --`n"
    echo "qmodName `t The file name of your qmod"

    echo "`n-- Arguments --`n"

    echo "-clean `t`t Performs a clean build on both your library and the qmod"
    echo "-release `t`t Creates a release build, please use together with the `"-version`" and the `"-clean`" argument"
    echo "-help `t`t Prints this"
    echo "-package `t Only packages existing files, without recompiling`n"

    exit
}

if ($qmodName -eq "")
{
    echo "Give a proper qmod name and try again"
    exit
}

if ($package -eq $true) {
    $qmodName = "$($env:module_id)_$($env:version)"
    echo "Actions: Packaging QMod $qmodName"
}
if (($args.Count -eq 0) -And $package -eq $false) {
echo "Creating QMod $qmodName"
    if ($version.Length -gt 0) {
        $qmodName += "_$($version)"
        qpm package edit --version $version
    }
    else {
        $qpm = "./qpm.json"
        $qpmJson = Get-Content $qpm -Raw | ConvertFrom-Json
        $qmodName += "_$($qpmJson.info.version)"
    }
    & $PSScriptRoot/build.ps1 -clean:$clean -version:$version -release:$release

    if ($LASTEXITCODE -ne 0) {
        echo "Failed to build, exiting..."
        exit $LASTEXITCODE
    }

    # qpm qmod manifest
}

echo "Creating qmod from mod.json"

$mod = "./mod.json"
# $modJson = Get-Content $mod -Raw | ConvertFrom-Json

# $filelist = @($mod)

# $cover = "./" + $modJson.coverImage
# if ((-not ($cover -eq "./")) -and (Test-Path $cover))
# { 
#     $filelist += ,$cover
# } else {
#     echo "No cover Image found"
# }

# foreach ($mod in $modJson.modFiles)
# {
#     $path = "./build/" + $mod
#     if (-not (Test-Path $path))
#     {
#         $path = "./extern/libs/" + $mod
#     }
#     $filelist += $path
# }

# foreach ($lib in $modJson.libraryFiles)
# {
#     $path = "./extern/libs/" + $lib
#     if (-not (Test-Path $path))
#     {
#         $path = "./build/" + $lib
#     }
#     $filelist += $path
# }

# $zip = $qmodName + ".zip"
$qmod = $qmodName + ".qmod"

# if ((-not ($clean.IsPresent)) -and (Test-Path $qmod))
# {
#     echo "Making Clean Qmod"
#     Move-Item $qmod $zip -Force
# }

# Compress-Archive -Path $filelist -DestinationPath $zip -Update
# Move-Item $zip $qmod -Force

qpm qmod zip -i ./build/
Move-Item multiplayer-core.qmod $qmod -Force

echo "Task Completed"