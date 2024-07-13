# Find QPM path or check for local github actions path
Param(
    [string]$QPMPath,
    [string]$QPMJsonPath
)
if ([string]::IsNullOrEmpty($QPMPath)) {
    $QPMPath = Get-Command qpm -ErrorAction SilentlyContinue
    if ($QPMPath -eq $null) {
        $QPMPath = Get-Command ./QPM/qpm -ErrorAction SilentlyContinue
        if ($QPMPath -eq $null) {
            Write-Host "QPM not found. Please install QPM or specify it's path with '-QPMPath'."
            exit 1
        }
    }
}

# Try reading qpm.json
if ([string]::IsNullOrEmpty($QPMJsonPath)) {
    $QPMJsonPath = "./qpm.json"

    Write-Debug "No qpm.json path specified. Using default path: $QPMJsonPath"
}
try {
    Write-Debug "Reading qpm.json from $QPMJsonPath"
    $QPMJson = Get-Content $QPMJsonPath | ConvertFrom-Json
} catch {
    Write-Host "Error reading qpm.json. Please make sure you're running on the correct path or specify it's path with '-QPMJsonPath'."
    exit 1
}

# Bump dependencies
$QPMJson.dependencies | ForEach-Object -Begin { $i = -1 } -Process {
    $i++
    $dependency = $_
    $dependencyName = $dependency.id
    $dependencyVersion = $dependency.versionRange.replace('^','')
    $newVersion = (& $QPMPath list versions $dependencyName | Select-Object -Last 1).replace(' - ', '')
    if ($dependencyVersion -eq ($newVersion -replace '\x1b\[[0-9;]*m', '')) {
        Write-Host "$dependencyName version $dependencyVersion is already up to date"
        return
    }
    Write-Host "Bumping $dependencyName from $($dependencyVersion) to $newVersion"
    $QPMJson.dependencies[$i].versionRange = "^$(($newVersion -replace '\x1b\[[0-9;]*m', ''))"
}

# Write back to qpm.json and reduce indentation, 
# since it's not that readable with that much indentation
$QPMJson | ConvertTo-Json -Depth 100 | foreach-object {($_ -replace "(?m)    (?<=^(?:  )*)", " ") -replace '(?m)\:  (?<=(?: )*)', ": " } | Set-Content $QPMJsonPath


# Restore depemdencies via QPM
#Write-Host "Restoring dependencies"
#& $QPMPath restore

#Write-Host "Dependencies bumped successfully"