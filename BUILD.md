# Building Fcitx5 Windows

This document provides detailed instructions for building Fcitx5 for Windows.

## Prerequisites

### System Requirements
- Windows 7 or later
- 64-bit system (for development, though compilation supports both x86-64 and ARM64 targets)

### Required Software

1. **MSYS2** - A software distribution and building platform for Windows
   - Download from: https://www.msys2.org/
   - Install to default location: `C:\msys64`

2. **Visual Studio or Windows SDK** - For Windows headers and libraries
   - Option A: Visual Studio Community Edition (free)
   - Option B: Windows SDK standalone

### Installation Steps

#### 1. Install MSYS2

Download and run the MSYS2 installer from https://www.msys2.org/

After installation, open **MSYS2 MINGW64** terminal and update packages:
```bash
pacman -Syu
```

#### 2. Install Build Tools

```bash
# Install CMake and Ninja
pacman -S mingw-w64-clang-x86_64-cmake
pacman -S mingw-w64-clang-x86_64-ninja

# Install Git (if not already installed)
pacman -S git

# Optional: Install ImageMagick for icon generation
pacman -S mingw-w64-clang-x86_64-imagemagick
```

#### 3. Install Windows SDK Headers

Install either Visual Studio or download Windows SDK headers. Make sure you have:
- `windows.h` and other Windows API headers
- `msctf.h` (for Text Services Framework)
- `atlcomcli.h` (for COM support)

## Building

### Clone Repository

```bash
git clone https://github.com/fcitx/fcitx5-windows.git
cd fcitx5-windows
git submodule update --init --recursive
```

### Build Configuration

The project uses CMake with a custom toolchain for cross-compilation. Key variables:

- `CMAKE_BUILD_TYPE`: Debug or Release
- `ARCH`: AMD64 or ARM64 (target architecture)
- `CMAKE_INSTALL_PREFIX`: Installation prefix

### Building for x86-64

```bash
# From MSYS2 MINGW64 terminal
cd fcitx5-windows

# Configure
cmake -B build/x86_64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=AMD64 \
  -DCMAKE_INSTALL_PREFIX=C:/fcitx5-x86_64

# Build
cmake --build build/x86_64

# Install
cmake --install build/x86_64
```

### Building for ARM64

```bash
# Configure
cmake -B build/arm64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=ARM64 \
  -DCMAKE_INSTALL_PREFIX=C:/fcitx5-arm64

# Build
cmake --build build/arm64

# Install
cmake --install build/arm64
```

### Debug Build

For debugging, use `CMAKE_BUILD_TYPE=Debug`:

```bash
cmake -B build/debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=AMD64

cmake --build build/debug
```

## Troubleshooting

### CMake not found

Ensure CMake is installed and in PATH:
```bash
which cmake
cmake --version
```

If not found, install it:
```bash
pacman -S mingw-w64-clang-x86_64-cmake
```

### Ninja not found

Install Ninja:
```bash
pacman -S mingw-w64-clang-x86_64-ninja
```

### Clang not found

The MSYS2 Clang64 environment includes clang. Verify:
```bash
which clang
clang --version
```

### Windows SDK headers not found

Ensure Windows SDK is installed. Try setting the SDK path:

```bash
cmake -B build/x86_64 \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=AMD64 \
  -DWINDOWS_SDK_PATH="C:/Program Files/Windows Kits/10"
```

### Build errors with COM headers

Try using ATL from MSYS2:
```bash
pacman -S mingw-w64-clang-x86_64-windows-default-manifest
```

## Running Tests

```bash
# Build tests
cmake -B build/x86_64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=AMD64

cmake --build build/x86_64

# Run tests
ctest --test-dir build/x86_64
```

## Code Formatting

Check code formatting:
```powershell
cd win32/scripts
.\lint.ps1
```

Fix formatting issues:
```powershell
.\format.ps1
```

## Installation

After building, install the input method:

```powershell
# Run from Visual Studio Developer PowerShell or regular PowerShell as Administrator
cd win32/scripts
.\install.ps1 -Architecture x86_64
```

## Development Workflow

1. **Setup**: Follow the installation steps above
2. **Build**: Use CMake to build the project
3. **Format**: Run `.\format.ps1` before committing
4. **Lint**: Run `.\lint.ps1` to check code style
5. **Test**: Run tests after making changes
6. **Commit**: Push changes to your branch

## Advanced Configuration

### Custom Installation

Build to a custom location:
```bash
cmake -B build/custom \
  -DCMAKE_INSTALL_PREFIX="C:/MyFcitx5"
```

### Different C++ Standard

```bash
cmake -B build/x86_64 \
  -DCMAKE_CXX_STANDARD=23
```

## Clean Up

Remove build artifacts:
```bash
rm -rf build/
rm -rf dist/
```

## Getting Help

- Check the main README.md for general information
- Review CMakeLists.txt files for build configuration
- Check GitHub Issues for known problems
- See the project repository for additional resources

## Next Steps

- Read [DEVELOPMENT.md](DEVELOPMENT.md) for architecture details
- Check [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines
- Review the Windows TSF documentation for implementation details
