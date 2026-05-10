/**
 * @file Dictionary.h
 * @brief 词库数据结构和加载系统
 *
 * 支持从文件加载词库，包括拼音和五笔
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>

namespace fcitx {

/**
 * 候选词数据结构
 */
struct Candidate {
    std::wstring text;    // 候选词文本
    int32_t priority;     // 优先级，数值越大越靠前
    std::string code;     // 编码（拼音或五笔码）

    Candidate() : text(), priority(0), code() {}
    Candidate(std::wstring t, int32_t p, std::string c)
        : text(std::move(t)), priority(p), code(std::move(c)) {}
};

/**
 * 输入法类型
 */
enum class InputMethodType {
    Pinyin,  // 拼音
    Wubi     // 五笔
};

/**
 * 词典条目
 */
struct DictEntry {
    std::wstring word;         // 汉字
    std::vector<std::string> codes; // 对应的编码（支持多编码）
    int32_t frequency;         // 词频
};

/**
 * 词典加载器
 */
class Dictionary {
  public:
    Dictionary();
    ~Dictionary();

    /**
     * 从文件加载词典
     * @param path 文件路径
     * @param type 输入法类型
     * @return 是否成功
     */
    bool load(const std::string& path, InputMethodType type);

    /**
     * 根据编码查找候选词
     * @param code 输入的编码
     * @return 候选词列表，按优先级排序
     */
    std::vector<Candidate> lookup(const std::string& code);

    /**
     * 获取当前输入法类型
     */
    InputMethodType getType() const { return currentType_; }

    /**
     * 检查词典是否已加载
     */
    bool isLoaded() const { return loaded_; }

    /**
     * 获取词典大小
     */
    size_t size() const { return entries_.size(); }

  private:
    /**
     * 加载简单文本格式的词库
     * 格式: 汉字 编码1 编码2... [词频]
     */
    bool loadTextFormat(const std::string& path);

    /**
     * 加载 RIME/YAML 格式的词库
     */
    bool loadRimeFormat(const std::string& path);

    /**
     * 构建倒排索引
     */
    void buildIndex();

    InputMethodType currentType_ = InputMethodType::Pinyin;
    std::vector<DictEntry> entries_;

    // 编码到词的映射（倒排索引）
    std::unordered_map<std::string, std::vector<size_t>> codeIndex_;

    bool loaded_ = false;
};

} // namespace fcitx
