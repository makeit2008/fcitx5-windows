/**
 * @file CandidateWindow.h
 * @brief 候选词显示窗口
 */

#pragma once

#include "Dictionary.h"
#include <Windows.h>
#include <vector>
#include <string>

namespace fcitx {

/**
 * 候选词窗口类
 */
class CandidateWindow {
  public:
    CandidateWindow();
    ~CandidateWindow();

    /**
     * 显示窗口
     */
    void show();
    
    /**
     * 隐藏窗口
     */
    void hide();
    
    /**
     * 更新候选词显示
     */
    void updateCandidates(const std::vector<Candidate>& candidates,
                         int currentPage, int totalPages,
                         const std::string& composition);
    
    /**
     * 移动到光标位置
     */
    void moveToCaret();
    
    /**
     * 检查是否可见
     */
    bool isVisible() const { return visible_; }
    
    /**
     * 处理键盘输入
     */
    bool handleKeyDown(WPARAM wParam);

  private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    void registerClass();
    void createWindow();
    void draw(HDC hdc);
    void calculateSize();
    
    HWND hwnd_ = nullptr;
    bool visible_ = false;
    bool classRegistered_ = false;
    
    std::vector<Candidate> candidates_;
    int currentPage_ = 0;
    int totalPages_ = 0;
    std::string composition_;
    
    int windowWidth_ = 320;
    int windowHeight_ = 240;
    int selectedIndex_ = 0;
    
    static const int Margin = 12;
    static const int HeaderHeight = 40;
    static const int ItemHeight = 36;
};

} // namespace fcitx
