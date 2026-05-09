# Fcitx5 Windows Uninstallation Script
# This script unregisters the Fcitx5 input method from Windows

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

# Try to resolve the path if it exists, otherwise use relative path
if (Test-Path $dllPath) {
    $dllPath = Resolve-Path $dllPath
} else {
    Write-Warning "DLL file not found at $dllPath, attempting to unregister by CLSID"
}

Write-Host "Uninstalling Fcitx5 Input Method ($Architecture architecture)"

# Check if running as Administrator
$currentPrincipal = [Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "Administrator privileges required. Requesting elevation..."
    Start-Process -Verb RunAs powershell.exe -Args "-executionpolicy bypass -command `"Set-Location '$PWD'; & '$PSCommandPath' -Architecture $Architecture -BuildDir $BuildDir`""
    exit $LASTEXITCODE
}

# Unregister the DLL
Write-Host "Unregistering DLL..."
try {
    if ($dllPath -and (Test-Path $dllPath)) {
        $result = & regsvr32.exe /u /s $dllPath
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ Successfully unregistered $dllName" -ForegroundColor Green
        } else {
            Write-Warning "regsvr32 returned non-zero exit code: $LASTEXITCODE"
        }
    } else {
        Write-Host "DLL file not found, attempting registry cleanup..."
    }
} catch {
    Write-Warning "Error during registration: $_"
}

# Clean up registry entries
Write-Host "Cleaning up registry entries..."
$clsidGuid = "{FC3869BA-51E3-4078-8EE2-5FE49493A1F4}"

try {
    # Remove CLSID registry entry
    $registryPath = "HKCR:\CLSID\$clsidGuid"
    if (Test-Path $registryPath) {
        Remove-Item $registryPath -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "✓ Removed CLSID registry entry" -ForegroundColor Green
    }
    
    # Remove Text Input Processor registry entry
    $tipPath = "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$clsidGuid"
    if (Test-Path $tipPath) {
        Remove-Item $tipPath -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "✓ Removed Text Input Processor registry entry" -ForegroundColor Green
    }
} catch {
    Write-Warning "Error cleaning registry: $_"
}

Write-Host ""
Write-Host "Uninstallation complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Note: You may need to restart your computer for changes to take effect."
Write-Host "To reinstall, run: .\install.ps1 -Architecture $Architecture"
