# Fcitx5 Windows 开发指南

## 项目架构

### 组件概述

```
┌─────────────────────────────────────────────────────┐
│           Windows Text Services Framework           │
│                      (TSF)                          │
└─────────────────────────────────────────────────────┘
                        ▲
                        │
        ┌───────────────┼───────────────┐
        │               │               │
    ┌──────────┐   ┌──────────┐   ┌──────────┐
    │ Keyboard │   │ TextEdit │   │ ThreadMgr│
    │ EventSink│   │   Sink   │   │EventSink │
    └──────────┘   └──────────┘   └──────────┘
        │               │               │
        └───────────────┼───────────────┘
                        │
                    ┌─────────┐
                    │ Fcitx5  │
                    │   DLL   │
                    └─────────┘
                        │
        ┌───────────────┼───────────────┐
        │               │               │
    ┌─────────┐   ┌──────────┐    ┌─────────┐
    │COM Init │   │TSF Init  │    │Registry │
    │& Cleanup│   │& Cleanup │    │Handling │
    └─────────┘   └──────────┘    └─────────┘
```

### 主要类和接口

#### 1. Tsf 类 (`win32/tsf/tsf.h`)

**继承接口**:
- `ITfTextInputProcessor`: 输入处理器主接口
- `ITfTextInputProcessorEx`: 扩展接口
- `ITfThreadMgrEventSink`: 线程管理器事件
- `ITfTextEditSink`: 文本编辑事件
- `ITfKeyEventSink`: 键盘事件
- `ITfCompositionSink`: 输入组成事件
- `ITfEditSession`: 编辑会话

**关键成员**:
- `refCount_`: COM 引用计数
- `threadMgr_`: 线程管理器指针
- `clientId_`: 客户端标识
- `textEditSinkContext_`: 当前编辑上下文

#### 2. DLL 导出 (`win32/dll/main.cpp`)

**导出函数**:
- `DllGetClassObject()`: COM 工厂方法
- `DllCanUnloadNow()`: 检查 DLL 是否可卸载
- `DllRegisterServer()`: 注册输入法
- `DllUnregisterServer()`: 卸载输入法
- `DllMain()`: DLL 入口点

## 代码流程

### 初始化流程

```
User selects Fcitx5 IME
    │
    ▼
Windows loads fcitx5-x86_64.dll
    │
    ▼
LoadLibrary → DllMain (DLL_PROCESS_ATTACH)
    │
    ├─ Initialize critical section
    └─ Store HINSTANCE
    │
    ▼
CoCreateInstance(FCITX_CLSID)
    │
    ├─ DllGetClassObject()
    └─ ClassFactory::CreateInstance()
    │
    ▼
Tsf::Tsf() constructor
    │
    ├─ Initialize COM references
    └─ DllAddRef()
    │
    ▼
ITfTextInputProcessor::Activate()
    │
    ├─ initThreadMgrEventSink()
    ├─ initKeyEventSink()
    ├─ initTextEditSink()
    │
    ▼
Ready to process input
```

### 键盘输入处理流程

```
User presses a key
    │
    ▼
Windows sends WM_KEYDOWN
    │
    ▼
Tsf::OnTestKeyDown()
    │
    ├─ Check if key should be processed
    ├─ Call processKey()
    ├─ Set pfEaten flag
    │
    ▼
If key was handled:
    Tsf::OnKeyDown()
    │
    ├─ Request edit session
    ├─ DoEditSession() called
    │
    ▼
DoEditSession()
    │
    ├─ Get insertion point
    ├─ Start composition
    ├─ Insert text (currently demo)
    ├─ End composition
    │
    ▼
Text appears in application
```

### 清理流程

```
User switches to different IME
    │
    ▼
Tsf::Deactivate()
    │
    ├─ uninitKeyEventSink()
    ├─ uninitTextEditSink()
    ├─ uninitThreadMgrEventSink()
    │
    ▼
Release all COM objects
    │
    ▼
Tsf::~Tsf() destructor
    │
    └─ DllRelease()
    │
    ▼
DllCanUnloadNow() → true
    │
    ▼
FreeLibrary()
    │
    ▼
DllMain (DLL_PROCESS_DETACH)
    │
    └─ Cleanup critical section
```

## 文件说明

### 核心 TSF 实现

| 文件 | 说明 |
|------|------|
| `tsf.h` | 主要 Tsf 类定义 |
| `tsf.cpp` | IUnknown 接口实现 |
| `TextInputProcessorEx.cpp` | 输入处理器激活/停用 |
| `ThreadMgrEventSink.cpp` | 线程管理器事件处理 |
| `KeyEventSink.cpp` | 键盘事件处理 |
| `TextEditSink.cpp` | 文本编辑事件处理 |
| `EditSession.cpp` | 编辑会话实现 |
| `CompositionSink.cpp` | 输入组成事件处理 |

### DLL 实现

| 文件 | 说明 |
|------|------|
| `main.cpp` | DLL 导出和 COM 工厂 |
| `register.cpp` | 注册表操作 |
| `register.h` | 注册表函数声明 |
| `util.cpp` | 工具函数 |
| `util.h` | 工具函数声明 |

### 构建和脚本

| 文件 | 说明 |
|------|------|
| `CMakeLists.txt` | CMake 构建脚本 |
| `msys2.toolchain.cmake` | CMake 工具链 |
| `install.ps1` | 安装脚本 |
| `uninstall.ps1` | 卸载脚本 |
| `format.ps1` | 代码格式化 |
| `lint.ps1` | 代码检查 |

## 开发任务

### 集成真实输入法引擎

**文件**: `win32/tsf/EditSession.cpp`

当前实现只插入演示文本。要集成真实输入法：

```cpp
// 替换 DoEditSession 中的硬编码文本
const wchar_t sampleText[] = L"测试";  // 当前

// 改为：
std::wstring inputText = getInputFromEngine();  // 从引擎获取
range->SetText(ec, 0, inputText.c_str(), inputText.length());
```

### 添加配置支持

**文件**: 新文件 `win32/config/config.h`

```cpp
class FcitxConfig {
public:
    static bool loadFromRegistry();
    static bool saveToRegistry();
    
private:
    std::string imEngine;
    bool useFullWidth;
    // ... 其他配置
};
```

### 实现候选词显示

**文件**: 新文件 `win32/ui/candidate.h`

```cpp
class CandidateWindow {
public:
    void show(const std::vector<std::wstring>& candidates);
    void hide();
    
private:
    HWND hwnd_;
    // UI 相关成员
};
```

## 常见修改场景

### 修改特殊键处理

**文件**: `win32/tsf/EventHandler.h`

```cpp
// 添加新的特殊键
enum class SpecialKeyCode : WPARAM {
    MyCustomKey = VK_CUSTOM,  // Add this
};

// 在 isSpecialKey() 中更新检查
inline bool isSpecialKey(WPARAM wParam) {
    // ... existing checks
    if (wParam == VK_CUSTOM) return true;  // Add this
}
```

### 修改输入文本处理

**文件**: `win32/tsf/KeyEventSink.cpp` → `processKey()`

```cpp
BOOL Tsf::processKey(WPARAM wParam, LPARAM lParam) {
    // 添加自定义按键过滤
    if (!isValidInputKey(wParam)) {
        return FALSE;
    }
    
    // 添加自定义处理逻辑
    if (!handleCustomLogic(wParam)) {
        return FALSE;
    }
    
    // ... 继续现有流程
}
```

### 修改组成文本插入

**文件**: `win32/tsf/EditSession.cpp` → `DoEditSession()`

```cpp
STDMETHODIMP Tsf::DoEditSession(TfEditCookie ec) {
    // ... 获取 range 和 context
    
    // 而不是硬编码文本
    std::wstring text = L"要插入的文本";
    
    // 设置颜色和样式（可选）
    ITfProperty* pProp = nullptr;
    // ... 设置文本属性
    
    // 插入文本
    range->SetText(ec, 0, text.c_str(), text.length());
}
```

## 调试技巧

### 启用日志输出

**文件**: `win32/tsf/tsf.cpp`

```cpp
#ifdef DEBUG
    OutputDebugString(L"Initializing Tsf...");
#endif
```

运行时使用 DebugView (https://docs.microsoft.com/en-us/sysinternals/downloads/debugview) 查看输出。

### 使用 Visual Studio 调试

1. 编译调试版本:
   ```bash
   cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
   ```

2. 附加到进程:
   - 在 Visual Studio 中选择 Debug → Attach to Process
   - 选择文本编辑器进程

3. 在 TSF 代码设置断点

### 检查注册表

```powershell
# 查看 Fcitx5 的注册表项
reg query "HKCR\CLSID\{FC3869BA-51E3-4078-8EE2-5FE49493A1F4}"

# 查看 TIP 注册表项
reg query "HKLM\SOFTWARE\Microsoft\CTF\TIP\{FC3869BA-51E3-4078-8EE2-5FE49493A1F4}"
```

## 测试清单

- [ ] 构建成功
- [ ] 代码格式检查通过
- [ ] 单元测试通过
- [ ] DLL 注册成功
- [ ] 在 Windows 设置中显示
- [ ] 能够切换到 Fcitx5
- [ ] 能够接收键盘输入
- [ ] 文本成功插入
- [ ] 按 Esc 取消输入
- [ ] 切换到其他 IME 成功
- [ ] 卸载成功

## 性能优化建议

1. **减少堆分配**: 使用栈分配或对象池
2. **缓存 COM 指针**: 避免重复的 QueryInterface 调用
3. **异步处理**: 使用 TF_ES_ASYNC 进行非关键操作
4. **最小化锁竞争**: 使用细粒度的同步

## 参考资源

- [Microsoft TSF Documentation](https://docs.microsoft.com/en-us/windows/win32/tsf/)
- [COM Programming](https://docs.microsoft.com/en-us/windows/win32/com/)
- [Windows Input Method Editor](https://docs.microsoft.com/en-us/windows/win32/intl/input-method-editor)
- [Fcitx5 Project](https://github.com/fcitx/fcitx5)
- [Weasel Project](https://github.com/rime/weasel) (参考实现)

## 联系方式

- GitHub Issues: https://github.com/fcitx/fcitx5-windows/issues
- Discussions: https://github.com/fcitx/fcitx5-windows/discussions
