/**
 * @file InputEngine.h
 * @brief 输入法引擎，支持拼音和五笔
 */

#pragma once

#include "Dictionary.h"
#include <string>
#include <vector>
#include <memory>

namespace fcitx {

/**
 * 输入法引擎，管理输入状态和候选词
 */
class InputEngine {
  public:
    InputEngine();
    ~InputEngine();

    // ========== 初始化 ==========
    
    /**
     * 加载词典
     */
    bool loadDictionary(const std::string& path, InputMethodType type);
    
    /**
     * 切换输入法类型
     */
    void switchInputMethod(InputMethodType type);
    
    // ========== 输入处理 ==========
    
    /**
     * 添加字符到输入
     */
    void addChar(char ch);
    
    /**
     * 删除最后一个字符
     */
    void removeChar();
    
    /**
     * 清空输入
     */
    void clearComposition();
    
    // ========== 候选词操作 ==========
    
    /**
     * 选择第 index 个候选词（0-based）
     */
    std::wstring selectCandidate(int index);
    
    /**
     * 选择第一个候选词
     */
    std::wstring selectFirst();
    
    /**
     * 翻页
     */
    void pageUp();
    void pageDown();
    
    // ========== 状态查询 ==========
    
    /**
     * 获取当前输入字符串
     */
    const std::string& getComposition() const { return composition_; }
    
    /**
     * 获取当前页的候选词
     */
    const std::vector<Candidate>& getCandidates() const { return currentCandidates_; }
    
    /**
     * 检查是否有正在输入的内容
     */
    bool hasComposition() const { return !composition_.empty(); }
    
    /**
     * 检查是否有候选词
     */
    bool hasCandidates() const { return !currentCandidates_.empty(); }
    
    /**
     * 获取当前页号
     */
    int getCurrentPage() const { return currentPage_; }
    
    /**
     * 获取总页数
     */
    int getTotalPages() const;
    
    /**
     * 获取当前输入法类型
     */
    InputMethodType getType() const { return currentType_; }
    
    /**
     * 获取每页候选词数量
     */
    static constexpr int getCandidatesPerPage() { return 9; }
    
  private:
    /**
     * 更新候选词
     */
    void updateCandidates();
    
    /**
     * 更新当前页的候选词
     */
    void updateCurrentPage();
    
    std::string composition_;
    std::vector<Candidate> allCandidates_;
    std::vector<Candidate> currentCandidates_;
    
    std::unique_ptr<Dictionary> dictionary_;
    InputMethodType currentType_ = InputMethodType::Pinyin;
    
    int currentPage_ = 0;
};

} // namespace fcitx
