# Fcitx5 Windows

This project implements Fcitx5 input method support for Windows using the Text Services Framework (TSF).

## About

Fcitx5 Windows provides a Windows IME (Input Method Editor) that enables input method capabilities on Windows systems, supporting various input methods including Chinese, Japanese, Korean, and other languages.

## Requirements

- **Operating System**: Windows 7 or later (x86-64 or ARM64)
- **Build Tools**:
  - CMake 3.27 or later
  - Ninja build system
  - Clang/LLVM compiler
  - MSYS2 with Clang64 environment (recommended)
- **Dependencies**:
  - Windows SDK (for COM, TSF, and other Windows APIs)
  - ImageMagick (optional, for icon generation from SVG)

## Building

### Setup Development Environment

1. **Install MSYS2 and Clang64**:
   ```bash
   pacman -S mingw-w64-clang-x86_64-cmake
   pacman -S mingw-w64-clang-x86_64-ninja
   pacman -S mingw-w64-clang-x86_64-imagemagick  # optional
   ```

2. **Clone the repository**:
   ```bash
   git clone https://github.com/fcitx/fcitx5-windows.git
   cd fcitx5-windows
   ```

### Build for x86_64

```bash
cmake -B build/x86_64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=AMD64

cmake --build build/x86_64
cmake --install build/x86_64 --prefix dist/x86_64
```

### Build for ARM64

```bash
cmake -B build/arm64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=ARM64

cmake --build build/arm64
cmake --install build/arm64 --prefix dist/arm64
```

## Installation

### Register the Input Method (x86_64)

Run as Administrator:
```powershell
regsvr32 path\to\fcitx5-x86_64.dll
```

Or use the installation script:
```powershell
Set-Location $projectRoot
.\win32\scripts\install.ps1
```

### Register the Input Method (ARM64)

Replace `fcitx5-x86_64.dll` with `fcitx5-arm64.dll` in the above commands.

## Usage

After registration, the input method should appear in Windows Settings:
1. Go to **Settings** → **Time & Language** → **Language**
2. Click **Add a language** and select the language for Fcitx5
3. The input method will be available in the taskbar

## Uninstallation

Run as Administrator:
```powershell
regsvr32 /u path\to\fcitx5-x86_64.dll
```

Or use the uninstall script:
```powershell
.\win32\scripts\uninstall.ps1
```

## Project Structure

```
fcitx5-windows/
├── CMakeLists.txt              # Main CMake configuration
├── fcitx5/                     # Fcitx5 core library (submodule)
├── src/                        # Main application source
├── win32/                      # Windows-specific implementation
│   ├── dll/                    # DLL implementation
│   ├── tsf/                    # Text Services Framework implementation
│   ├── assets/                 # Resources (icons, etc.)
│   ├── tests/                  # Unit tests
│   └── scripts/                # Install/uninstall scripts
└── windows-cross/              # Cross-compilation configuration
```

## Architecture

### Text Services Framework (TSF)

The implementation uses Windows TSF to integrate with the text input system:
- **ITfTextInputProcessor**: Main input processor interface
- **ITfKeyEventSink**: Handles keyboard input events
- **ITfEditSession**: Manages text editing operations
- **ITfCompositionSink**: Handles composition state changes

### DLL Registration

The input method is registered as a COM object in the Windows registry:
- **CLSID**: `{FC3869BA-51E3-4078-8EE2-5FE49493A1F4}`
- **Language**: Chinese Simplified (0x0804)

## Contributing

Contributions are welcome! Please follow the existing code style and submit pull requests with clear descriptions.

## Credits

* [fcitx5](https://github.com/fcitx/fcitx5): Core input method framework (LGPL-2.1-or-later)
* [weasel](https://github.com/rime/weasel): Windows IME implementation reference (GPL-3.0-only)

## License

This project is provided under the LGPL-2.1 or later license. Please see LICENSE file for details.

## Troubleshooting

### Build Fails with "msys2.toolchain.cmake not found"
- Ensure you're running from the project root directory.
- Verify that `windows-cross/msys2.toolchain.cmake` exists.

### Icon generation fails
- This is non-critical. Ensure ImageMagick is installed, or manually provide `penguin.ico`.

### DLL fails to register
- Ensure you're running PowerShell as Administrator.
- Check that the DLL path is correct and accessible.
- Verify Windows SDK headers are properly included.

### No input method appears after registration
- Verify registration succeeded with: `reg query "HKCR\CLSID\{FC3869BA-51E3-4078-8EE2-5FE49493A1F4}"`
- Restart Windows or log out and log in.
- Check Windows Event Viewer for any COM-related errors.

## Security

Always ensure you're building from trusted sources and using the official repository. Be cautious when running scripts as Administrator.

## Future Improvements

- [ ] Support for more input methods
- [ ] Configuration UI
- [ ] Real-time input engine integration
- [ ] Performance optimizations
- [ ] Accessibility features
- [ ] Internationalization improvements
