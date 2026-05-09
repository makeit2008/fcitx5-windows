# Using Fcitx5 on Windows

This guide explains how to install, configure, and use Fcitx5 input method on Windows.

## Installation

### Prerequisites
- Windows 7 or later
- Administrator access for registration

### Installation Steps

1. **Build Fcitx5** (if not pre-built):
   - Follow the instructions in [BUILD.md](BUILD.md)
   - The built DLL will be in `build/x86_64/dll/fcitx5-x86_64.dll` or `build/arm64/dll/fcitx5-arm64.dll`

2. **Register the Input Method**:
   
   Option A: Using the provided script (recommended):
   ```powershell
   # Open PowerShell as Administrator
   cd fcitx5-windows\win32\scripts
   .\install.ps1 -Architecture x86_64
   ```

   Option B: Manual registration:
   ```powershell
   # Open PowerShell as Administrator
   regsvr32 "C:\path\to\fcitx5-x86_64.dll"
   ```

3. **Verify Installation**:
   - Check Registry: `reg query "HKCR\CLSID\{FC3869BA-51E3-4078-8EE2-5FE49493A1F4}"`
   - You should see the registry entry for Fcitx5

### Post-Installation Configuration

1. **Windows Settings**:
   - Go to **Settings** → **Time & Language** → **Language**
   - Click **"Add a language"**
   - Search for and select your desired language (e.g., Chinese - China)
   - Windows will add Fcitx5 to the language options

2. **Activate the Input Method**:
   - In the **Language** settings, select the added language
   - The input method should appear in your taskbar
   - Click on the language indicator to switch between input methods

## Usage

### Switching Input Methods

- **Keyboard shortcut**: Usually `Win + Space` or `Ctrl + Space` (configurable)
- **Taskbar**: Click the language indicator in the system tray
- **Windows Settings**: Go to Language Settings and select the input method

### Typing with Fcitx5

The exact input method depends on the configured input engine. The current implementation is a demonstration that supports basic text input.

### Keyboard Shortcuts

Common shortcuts (may vary by system):
- `Win + Space`: Cycle through input methods
- `Shift`: Toggle between half-width and full-width (if supported)
- `Ctrl + .`: Toggle punctuation mode (if configured)
- `Esc`: Cancel input composition

## Troubleshooting

### Input Method Not Appearing

**Problem**: After registration, the input method doesn't appear in Windows Settings.

**Solution**:
1. Restart Windows or log out and log back in
2. Check if the DLL is properly registered:
   ```powershell
   reg query "HKCR\CLSID\{FC3869BA-51E3-4078-8EE2-5FE49493A1F4}"
   ```
3. Try uninstalling and reinstalling:
   ```powershell
   .\uninstall.ps1 -Architecture x86_64
   .\install.ps1 -Architecture x86_64
   ```

### DLL Loading Fails

**Problem**: Error when loading the DLL (e.g., "The procedure entry point could not be located").

**Solution**:
1. Ensure architecture matches your Windows version (x86_64 for 64-bit Windows)
2. Verify all dependencies are available:
   - Windows SDK headers should be properly installed
   - COM libraries should be accessible
3. Check Windows Event Viewer for detailed error messages:
   - Open Event Viewer
   - Go to **Windows Logs** → **System**
   - Look for COM-related errors

### Input Method Not Responding

**Problem**: The input method seems to be loaded but doesn't respond to key input.

**Solution**:
1. Try switching to a different input method and back
2. Restart the application
3. Restart Windows
4. Check the implementation is properly initialized:
   - The demo implementation only shows test text insertion
   - Full input method engine integration is needed for production use

### Cannot Uninstall

**Problem**: Unregistration fails with an error.

**Solution**:
1. Ensure you're running PowerShell as Administrator
2. Try the manual unregistration command:
   ```powershell
   regsvr32 /u "C:\path\to\fcitx5-x86_64.dll"
   ```
3. If the DLL is locked, close all applications and try again

## Uninstallation

To remove Fcitx5 from your system:

```powershell
# Open PowerShell as Administrator
cd fcitx5-windows\win32\scripts
.\uninstall.ps1 -Architecture x86_64
```

Or manually:
```powershell
regsvr32 /u "C:\path\to\fcitx5-x86_64.dll"
```

After uninstallation:
- Go to **Settings** → **Time & Language** → **Language**
- Remove the language associated with Fcitx5 (optional)

## Advanced Usage

### Multiple Architecture Support

If you have both 32-bit and 64-bit applications, you may want to:

1. **Build and install both versions**:
   ```powershell
   # For x86-64 (64-bit Windows, 64-bit applications)
   cmake --build build/x86_64
   .\install.ps1 -Architecture x86_64

   # For ARM64 (compatible systems)
   cmake --build build/arm64
   .\install.ps1 -Architecture arm64
   ```

### Configuration File

Currently, Fcitx5 Windows doesn't have a user-facing configuration UI. Configuration would typically be stored in:
- `%APPDATA%\fcitx5\` (for user settings)
- Or registry at `HKCU\Software\Fcitx5\` (for registry-based settings)

### Logging and Debugging

To diagnose issues, enable detailed logging:

1. Set environment variable:
   ```powershell
   $env:FCITX5_LOG_LEVEL = "DEBUG"
   ```

2. Check Event Viewer for COM and IME related events:
   - **Event Viewer** → **Windows Logs** → **System**
   - Filter by "IME" or "COM"

3. Check application logs:
   - `%APPDATA%\fcitx5\logs\` (if enabled)

## Performance Tips

1. **Close unnecessary applications**: The input method runs in the same process as the application
2. **Disable unused input methods**: Reduces overhead
3. **Keep Windows updated**: Ensures COM and TSF are up-to-date

## Compatibility

### Supported Applications

Input methods using TSF should work with:
- Microsoft Office (Word, Excel, PowerPoint, etc.)
- Visual Studio and other IDEs
- Web browsers (Edge, Chrome, Firefox)
- Most modern Windows applications

### Known Limitations

- Some legacy applications may not support TSF
- 16-bit applications (legacy DOS/Windows 3.x): Not supported
- Some console applications: Limited support

## Additional Resources

- [Microsoft TSF Documentation](https://docs.microsoft.com/en-us/windows/win32/tsf/text-services-framework)
- [Fcitx5 Project](https://github.com/fcitx/fcitx5)
- [Windows IME Documentation](https://docs.microsoft.com/en-us/windows/win32/intl/input-method-editor)
- Fcitx5 Windows GitHub: https://github.com/fcitx/fcitx5-windows

## Reporting Issues

If you encounter problems:

1. Gather diagnostic information:
   - Windows version (Settings → System → About)
   - Architecture (32-bit or 64-bit)
   - Fcitx5 version or build date
   - Error messages or symptoms

2. Open an issue on GitHub with:
   - Description of the problem
   - Steps to reproduce
   - Diagnostic information
   - Screenshots if applicable

## Support

For issues and questions:
- GitHub Issues: https://github.com/fcitx/fcitx5-windows/issues
- Discussions: https://github.com/fcitx/fcitx5-windows/discussions
- Related Project: https://github.com/fcitx/fcitx5

## Getting More Help

- See [BUILD.md](BUILD.md) for building from source
- See [README.md](README.md) for project overview
- Check the project repository for additional documentation
