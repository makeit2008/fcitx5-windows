# Fcitx5 Windows Code Linting Script
# Checks C++ source code formatting with clang-format

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent (Split-Path -Parent $scriptDir)

Write-Host "Linting C++ source files..."
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
Write-Host "Checking $fileCount C++ files"

# Check formatting of each file
$failedFiles = @()
$passedCount = 0
foreach ($file in $files) {
    $filePath = Join-Path $projectRoot $file
    try {
        & clang-format -Werror --dry-run $filePath | Out-Null
        if ($LASTEXITCODE -eq 0) {
            $passedCount++
        } else {
            $failedFiles += $file
            Write-Host "✗ Format check failed: $file"
        }
    } catch {
        Write-Error "Error checking $file : $_"
        $failedFiles += $file
    }
}

Write-Host ""
if ($failedFiles.Count -eq 0) {
    Write-Host "All files passed formatting check! ($passedCount/$fileCount)" -ForegroundColor Green
    exit 0
} else {
    Write-Host "Formatting check failed for $($failedFiles.Count) file(s):" -ForegroundColor Red
    foreach ($file in $failedFiles) {
        Write-Host "  - $file"
    }
    Write-Host ""
    Write-Host "Run '.\format.ps1' to fix formatting issues"
    exit 1
}
