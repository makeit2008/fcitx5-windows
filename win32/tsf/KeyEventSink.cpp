#include "tsf.h"
#include "EventHandler.h"

namespace fcitx {
bool Tsf::initKeyEventSink() {
    CComPtr<ITfKeystrokeMgr> keystrokeMgr;
    if (threadMgr_->QueryInterface(&keystrokeMgr) != S_OK) {
        return false;
    }
    return keystrokeMgr->AdviseKeyEventSink(clientId_, (ITfKeyEventSink *)this,
                                            TRUE) == S_OK;
}

void Tsf::uninitKeyEventSink() {
    CComPtr<ITfKeystrokeMgr> keystrokeMgr;
    if (threadMgr_->QueryInterface(&keystrokeMgr) != S_OK) {
        return;
    }
    keystrokeMgr->UnadviseKeyEventSink(clientId_);
}

BOOL Tsf::processKey(WPARAM wParam, LPARAM lParam) {
    // 如果没有文本上下文，不处理
    if (textEditSinkContext_ == nullptr) {
        return FALSE;
    }
    
    // 首先让候选词窗口处理导航键
    if (inputEngine_->hasComposition()) {
        if (candidateWindow_->handleKeyDown(wParam)) {
            updateCandidateWindow();
            return TRUE;
        }
    }
    
    // 处理退格键
    if (wParam == VK_BACK) {
        if (inputEngine_->hasComposition()) {
            inputEngine_->removeChar();
            updateCandidateWindow();
        }
        return inputEngine_->hasComposition();
    }
    
    // 处理 Esc 键取消
    if (wParam == VK_ESCAPE) {
        inputEngine_->clearComposition();
        updateCandidateWindow();
        return TRUE;
    }
    
    // 处理 Enter 和 Space 键提交
    if (wParam == VK_RETURN || wParam == VK_SPACE) {
        if (inputEngine_->hasComposition() && inputEngine_->hasCandidates()) {
            std::wstring text = inputEngine_->selectFirst();
            if (!text.empty()) {
                commitText(text);
            }
            updateCandidateWindow();
        }
        return inputEngine_->hasComposition();
    }
    
    // 处理数字键选择
    if (wParam >= '1' && wParam <= '9') {
        if (inputEngine_->hasComposition()) {
            int idx = (wParam - '1');
            std::wstring text = inputEngine_->selectCandidate(idx);
            if (!text.empty()) {
                commitText(text);
            }
            updateCandidateWindow();
        }
        return inputEngine_->hasComposition();
    }
    
    // 处理字母键输入
    if ((wParam >= 'A' && wParam <= 'Z') || (wParam >= 'a' && wParam <= 'z')) {
        inputEngine_->addChar(static_cast<char>(wParam));
        updateCandidateWindow();
        return TRUE;
    }
    
    // 处理翻页
    if (wParam == VK_NEXT) {
        if (inputEngine_->hasComposition()) {
            inputEngine_->pageDown();
            updateCandidateWindow();
            return TRUE;
        }
    }
    
    if (wParam == VK_PRIOR) {
        if (inputEngine_->hasComposition()) {
            inputEngine_->pageUp();
            updateCandidateWindow();
            return TRUE;
        }
    }
    
    return FALSE;
}

STDMETHODIMP Tsf::OnSetFocus(BOOL fForeground) {
    if (fForeground && (inputEngine_->hasComposition() || inputEngine_->hasCandidates())) {
        updateCandidateWindow();
    }
    return S_OK;
}

STDMETHODIMP Tsf::OnTestKeyDown(ITfContext *pContext, WPARAM wParam,
                               LPARAM lParam, BOOL *pfEaten) {
    if (keyDownHandled_) {
        *pfEaten = TRUE;
    } else {
        *pfEaten = keyDownHandled_ = processKey(wParam, lParam);
    }
    return S_OK;
}

STDMETHODIMP Tsf::OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam,
                           BOOL *pfEaten) {
    if (keyDownHandled_) {
        keyDownHandled_ = FALSE;
        *pfEaten = TRUE;
    } else {
        *pfEaten = keyDownHandled_ = processKey(wParam, lParam);
    }
    return S_OK;
}

STDMETHODIMP Tsf::OnTestKeyUp(ITfContext *pContext, WPARAM wParam,
                             LPARAM lParam, BOOL *pfEaten) {
    *pfEaten = FALSE;
    return S_OK;
}

STDMETHODIMP Tsf::OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam,
                         BOOL *pfEaten) {
    *pfEaten = FALSE;
    return S_OK;
}

STDMETHODIMP Tsf::OnPreservedKey(ITfContext *pContext, REFGUID rguid,
                                BOOL *pfEaten) {
    *pfEaten = FALSE;
    return S_OK;
}
} // namespace fcitx
