# Fcitx5 Windows Installation Script
# This script registers the Fcitx5 input method on Windows

param(
    [string]$Architecture = "x86_64",
    [string]$BuildDir = "build"
)

# Validate input
$validArchs = @("x86_64", "arm64")
if ($Architecture -notin $validArchs) {
    Write-Error "Invalid architecture: $Architecture. Valid options are: $($validArchs -join ', ')"
    exit 1
}

# Determine DLL path
$dllName = "fcitx5-$Architecture.dll"
$dllPath = Join-Path $BuildDir $Architecture "dll" $dllName
$dllPath = Resolve-Path $dllPath -ErrorAction Stop

Write-Host "Installing Fcitx5 Input Method ($Architecture architecture)"
Write-Host "DLL Path: $dllPath"

# Check if running as Administrator
$currentPrincipal = [Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "Administrator privileges required. Requesting elevation..."
    Start-Process -Verb RunAs powershell.exe -Args "-executionpolicy bypass -command `"Set-Location '$PWD'; & '$PSCommandPath' -Architecture $Architecture -BuildDir $BuildDir`""
    exit $LASTEXITCODE
}

# Register the DLL
Write-Host "Registering DLL..."
try {
    $result = & regsvr32.exe /s $dllPath
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Successfully registered $dllName" -ForegroundColor Green
    } else {
        Write-Error "Failed to register DLL. Exit code: $LASTEXITCODE"
        exit $LASTEXITCODE
    }
} catch {
    Write-Error "Error during registration: $_"
    exit 1
}

Write-Host ""
Write-Host "Installation complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:"
Write-Host "1. Go to Settings → Time & Language → Language & Region"
Write-Host "2. Add the language for Fcitx5"
Write-Host "3. The input method should now be available in your taskbar"
Write-Host ""
Write-Host "To uninstall, run: .\uninstall.ps1 -Architecture $Architecture"
