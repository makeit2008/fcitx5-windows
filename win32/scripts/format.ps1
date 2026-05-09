# Fcitx5 Windows Code Formatting Script
# Formats C++ source code using clang-format

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent (Split-Path -Parent $scriptDir)

Write-Host "Formatting C++ source files..."
Write-Host "Project root: $projectRoot"

# Get all C++ source files tracked by git
try {
    $files = & git -C $projectRoot ls-files -- '*.cpp' '*.h'
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to get file list from git"
        exit 1
    }
} catch {
    Write-Error "Git command failed: $_"
    exit 1
}

if (-not $files) {
    Write-Host "No C++ files found"
    exit 0
}

$fileCount = if ($files -is [array]) { $files.Count } else { 1 }
Write-Host "Found $fileCount C++ files to format"

# Format each file
$formattedCount = 0
foreach ($file in $files) {
    $filePath = Join-Path $projectRoot $file
    try {
        & clang-format -i $filePath
        $formattedCount++
        Write-Host "✓ Formatted: $file"
    } catch {
        Write-Error "Failed to format $file : $_"
    }
}

Write-Host ""
Write-Host "Formatting complete! Processed $formattedCount files." -ForegroundColor Green
