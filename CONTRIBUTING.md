# 贡献指南

感谢您对 Fcitx5 Windows 项目的兴趣！本指南将帮助您成功贡献代码。

## 开始贡献

### 1. Fork 和 Clone

```bash
# Fork 项目到您的 GitHub 账号
# 然后 Clone 您的 Fork

git clone https://github.com/YOUR_USERNAME/fcitx5-windows.git
cd fcitx5-windows
git remote add upstream https://github.com/fcitx/fcitx5-windows.git
```

### 2. 创建分支

```bash
# 更新主分支
git fetch upstream
git checkout upstream/main
git pull

# 创建特性分支
git checkout -b feature/your-feature-name
# 或 bug 修复
git checkout -b bugfix/issue-description
```

### 3. 理解项目结构

请查看以下文件了解项目:
- [README.md](README.md) - 项目概览
- [BUILD.md](BUILD.md) - 构建说明
- [DEVELOPMENT.md](DEVELOPMENT.md) - 开发架构
- [IMPROVEMENTS.md](IMPROVEMENTS.md) - 最新改进

## 代码标准

### C++ 编码风格

项目使用 `clang-format` 进行代码格式化。

#### 自动格式化

```powershell
# 在项目根目录运行
.\win32\scripts\format.ps1
```

#### 手动检查

```powershell
# 检查代码格式是否正确
.\win32\scripts\lint.ps1
```

### 代码规范

1. **命名约定**:
   - 类名使用 PascalCase: `class MyClass`
   - 方法名使用 camelCase: `void myMethod()`
   - 成员变量后缀加 `_`: `int myValue_`
   - 常量使用 UPPER_CASE: `const int MAX_SIZE`

2. **注释**:
   ```cpp
   // 单行注释使用 //
   
   /* 多行注释使用 /* */
   
   /// 文档注释（Doxygen）
   /// @param value 参数说明
   /// @return 返回值说明
   ```

3. **包含顺序**:
   ```cpp
   // 标准库
   #include <windows.h>
   #include <vector>
   
   // ATL/COM
   #include <atlcomcli.h>
   
   // 项目文件
   #include "tsf.h"
   ```

4. **错误处理**:
   ```cpp
   // 使用 HRESULT 检查 COM 调用
   if (FAILED(hr)) {
       return hr;
   }
   
   // 使用 nullptr 检查指针
   if (ptr == nullptr) {
       return E_INVALIDARG;
   }
   ```

## 提交流程

### 1. 创建高质量提交

```bash
# 将相关改动分组
git add win32/tsf/tsf.cpp
git commit -m "feat: Add special key filtering in KeyEventSink"

# 避免大型单个提交
# 将逻辑分离为多个小提交
```

### 2. 提交信息格式

遵循 Conventional Commits 格式:

```
<type>(<scope>): <subject>

<body>

<footer>
```

#### 类型 (type)

- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更改
- `style`: 代码格式（不影响功能）
- `refactor`: 代码重构
- `perf`: 性能改进
- `test`: 添加/修改测试
- `chore`: 构建脚本, 依赖更新等

#### 示例

```
feat(tsf): implement special key filtering

Add filtering for special keys to prevent them from being
processed as input method input. This improves compatibility
with Windows IME conventions.

Closes #42
```

### 3. 保持分支最新

```bash
# 定期从上游拉取更新
git fetch upstream
git rebase upstream/main

# 如果有冲突，解决它们
git add .
git rebase --continue
```

### 4. 推送并创建 Pull Request

```bash
git push origin feature/your-feature-name
```

然后在 GitHub 上创建 Pull Request（PR）。

## Pull Request 检查清单

提交 PR 前，请确保：

- [ ] 代码已格式化: `.\win32\scripts\format.ps1`
- [ ] 代码检查通过: `.\win32\scripts\lint.ps1`
- [ ] 编译成功: `cmake --build build/x86_64`
- [ ] 测试通过: `ctest --test-dir build/x86_64`
- [ ] 提交信息清晰明确
- [ ] 添加了必要的注释和文档
- [ ] 更新了相关测试
- [ ] 分支基于最新的 `upstream/main`
- [ ] 没有合并冲突

## PR 描述模板

```markdown
## 描述

简要描述这个 PR 做了什么。

## 类型

- [x] Bug 修复 (bug fix)
- [ ] 新功能 (new feature)
- [ ] 破坏性改动 (breaking change)
- [ ] 文档更新 (documentation)

## 相关 Issue

修复 #XXX

## 修改清单

- [x] 代码格式化
- [x] 添加/更新文档
- [x] 添加测试
- [x] 所有测试通过

## 屏幕截图（如果适用）

附加相关的屏幕截图或视频。
```

## 测试指南

### 运行现有测试

```bash
# 构建并运行所有测试
cmake --build build/x86_64
ctest --test-dir build/x86_64

# 运行特定测试
ctest --test-dir build/x86_64 -V -R test_dll
```

### 添加新测试

```cpp
// win32/tests/test_new_feature.cpp

#include "../dll/util.h"
#include <cassert>

int main() {
    // 测试代码
    assert(condition == expected);
    return 0;
}
```

在 `CMakeLists.txt` 中注册测试:

```cmake
add_test_target(test_new_feature test_new_feature.cpp)
```

## 常见贡献场景

### 修复 Bug

1. 创建 bug 修复分支
2. 编写失败的测试（可选）
3. 实现修复
4. 验证测试通过
5. 创建 PR，引用相关 Issue

### 添加新功能

1. 先讨论：打开 Issue 或 Discussion 讨论设计
2. 获得反馈后开始实现
3. 添加测试和文档
4. 创建 PR

### 改进文档

1. 编辑相关 Markdown 文件
2. 检查语法和格式
3. 创建 PR

## 获取帮助

- 查看 [DEVELOPMENT.md](DEVELOPMENT.md) 了解架构
- 查看 [BUILD.md](BUILD.md) 了解构建
- 提交 Issue 讨论设计问题
- 使用 Discussions 进行一般讨论

## 代码审查

### 作为贡献者

- 响应审查意见
- 进行必要的修改
- 推送更新（不要强制推送）
- 感谢审查者的反馈

### 作为审查者

- 检查代码质量和风格
- 验证逻辑正确性
- 建议改进
- 回答问题

## 许可证

通过贡献此项目，您同意您的贡献符合项目许可证 (LGPL-2.1-or-later)。

## 行为准则

我们致力于提供友好、无骚扰的项目环境。请：

- 尊重所有贡献者
- 给出建设性反馈
- 接受批评
- 专注于最好的项目结果

## 常见问题

**Q: 我能贡献哪些内容？**
A: Bug 修复、新功能、文档改进、测试等。查看 Issues 了解需要帮助的领域。

**Q: 首次贡献者应该做什么？**
A: 从标记为 `good first issue` 的 Issues 开始。

**Q: 需要多久才能合并我的 PR？**
A: 通常 3-7 天，取决于复杂性和审查人员的可用性。

**Q: 我可以贡献多少代码？**
A: 没有限制！无论是一行修复还是完整功能都欢迎。

## 联系方式

- GitHub Issues: https://github.com/fcitx/fcitx5-windows/issues
- Discussions: https://github.com/fcitx/fcitx5-windows/discussions
- Email: [项目维护者邮箱]

感谢您的贡献！ 🎉
