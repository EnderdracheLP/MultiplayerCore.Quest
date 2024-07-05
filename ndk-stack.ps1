$NDKPath = $env:NdkPath
# if ($NDKPath -eq "") {
    $NDKPath = Get-Content $PSScriptRoot/ndkpath.txt
# }

$stackScript = "$NDKPath/ndk-stack"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $stackScript += ".cmd"
}

if ($args.Count -eq 0) {
    Get-Content ./log.txt | & $stackScript -sym ./build/ > log_unstripped.log
} else {
    Get-Content $args[0] | & $stackScript -sym ./build/ > "$($args[0])_unstripped.txt"
}
