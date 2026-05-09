# Fcitx5 Windows 快速参考

## 快速开始

### 安装依赖 (MSYS2)

```bash
pacman -S mingw-w64-clang-x86_64-cmake
pacman -S mingw-w64-clang-x86_64-ninja
pacman -S mingw-w64-clang-x86_64-imagemagick  # 可选
```

### 构建

```bash
# x86-64 版本
cmake -B build/x86_64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=AMD64
cmake --build build/x86_64

# ARM64 版本
cmake -B build/arm64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=ARM64
cmake --build build/arm64
```

### 安装 (PowerShell as Administrator)

```powershell
cd win32/scripts
.\install.ps1 -Architecture x86_64
```

### 卸载

```powershell
cd win32/scripts
.\uninstall.ps1 -Architecture x86_64
```

## 常用命令

### 开发

```powershell
# 格式化代码
.\win32\scripts\format.ps1

# 检查代码格式
.\win32\scripts\lint.ps1
```

### 构建

```bash
# 清空构建
rm -rf build/

# 调试构建
cmake -B build/debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE='windows-cross/msys2.toolchain.cmake' \
  -DARCH=AMD64
cmake --build build/debug

# 运行测试
ctest --test-dir build/debug
```

## 文件结构

```
├── README.md              # 项目概览
├── BUILD.md               # 构建指南
├── USAGE.md               # 使用指南
├── IMPROVEMENTS.md        # 改进详情
├── QUICK_START.md         # 本文件
├── win32/dll/             # DLL 实现
├── win32/tsf/             # TSF 实现
├── win32/scripts/         # 工具脚本
└── windows-cross/         # 工具链配置
```

## 关键文件

| 文件 | 说明 |
|------|------|
| `windows-cross/msys2.toolchain.cmake` | CMake 工具链配置 |
| `win32/dll/main.cpp` | DLL 导出函数 |
| `win32/tsf/tsf.h` | TSF 主要接口 |
| `win32/scripts/install.ps1` | 自动安装脚本 |
| `win32/scripts/format.ps1` | 代码格式化脚本 |

## 常见问题

**Q: CMake 配置失败**
A: 确保 MSYS2 Clang64 工具链已安装，并且使用了正确的 CMake 工具链文件

**Q: 无法注册 DLL**
A: 使用管理员权限运行 PowerShell，然后执行 `.\install.ps1` 脚本

**Q: 输入法未出现**
A: 重启 Windows，检查注册表，尝试重新安装

**Q: 图标生成失败？**
A: 这不影响功能。安装 ImageMagick 或手动提供 penguin.ico 文件

## 架构支持

- **x86-64 (AMD64)**: 标准 64 位 Windows 系统，基于 x86_64 处理器
- **ARM64**: Windows on ARM 系统，使用 ARM64 处理器

## 许可证

本项目使用 LGPL-2.1 或更高版本许可证。

## 相关链接

- 项目: https://github.com/fcitx/fcitx5-windows
- fcitx5: https://github.com/fcitx/fcitx5
- Windows TSF: https://docs.microsoft.com/en-us/windows/win32/tsf/

## 获取帮助

- 查看 [README.md](README.md) 了解项目信息
- 查看 [BUILD.md](BUILD.md) 了解构建详情
- 查看 [USAGE.md](USAGE.md) 了解使用方法
- 查看 [IMPROVEMENTS.md](IMPROVEMENTS.md) 了解改进详情
- 提交 Issue: https://github.com/fcitx/fcitx5-windows/issues
