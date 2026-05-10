#pragma once

#include <atlcomcli.h>
#include <msctf.h>
#include "InputEngine.h"
#include "CandidateWindow.h"

namespace fcitx {
class Tsf : public ITfTextInputProcessorEx,
            public ITfThreadMgrEventSink,
            public ITfTextEditSink,
            public ITfKeyEventSink,
            public ITfCompositionSink,
            public ITfEditSession {
  public:
    Tsf();
    ~Tsf();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    // ITfTextInputProcessor
    STDMETHODIMP Activate(ITfThreadMgr *pThreadMgr,
                          TfClientId tfClientId) override;
    STDMETHODIMP Deactivate() override;

    // ITfTextInputProcessorEx
    STDMETHODIMP ActivateEx(ITfThreadMgr *pThreadMgr, TfClientId tfClientId,
                            DWORD dwFlags) override;

    // ITfThreadMgrEventSink
    STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr *pDocMgr) override;
    STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr) override;
    STDMETHODIMP OnSetFocus(ITfDocumentMgr *pDocMgrFocus,
                            ITfDocumentMgr *pDocMgrPrevFocus) override;
    STDMETHODIMP OnPushContext(ITfContext *pContext) override;
    STDMETHODIMP OnPopContext(ITfContext *pContext) override;

    // ITfTextEditSink
    STDMETHODIMP OnEndEdit(ITfContext *pic, TfEditCookie ecReadOnly,
                           ITfEditRecord *pEditRecord) override;

    // ITfKeyEventSink
    STDMETHODIMP OnSetFocus(BOOL fForeground) override;
    STDMETHODIMP OnTestKeyDown(ITfContext *pContext, WPARAM wParam,
                               LPARAM lParam, BOOL *pfEaten) override;
    STDMETHODIMP OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam,
                           BOOL *pfEaten) override;
    STDMETHODIMP OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam,
                             BOOL *pfEaten) override;
    STDMETHODIMP OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam,
                         BOOL *pfEaten) override;
    STDMETHODIMP OnPreservedKey(ITfContext *pContext, REFGUID rguid,
                                BOOL *pfEaten) override;

    // ITfCompositionSink
    STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite,
                                         ITfComposition *pComposition) override;

    // ITfEditSession
    STDMETHODIMP DoEditSession(TfEditCookie ec) override;

  private:
    LONG refCount_ = 1;

    // ITfThreadMgrEventSink
    bool initThreadMgrEventSink();
    void uninitThreadMgrEventSink();
    CComPtr<ITfThreadMgr> threadMgr_;
    TfClientId clientId_ = TF_CLIENTID_NULL;
    DWORD threadMgrEventSinkCookie_ = TF_INVALID_COOKIE;

    // ITfTextEditSink
    bool initTextEditSink(CComPtr<ITfDocumentMgr> documentMgr);
    DWORD textEditSinkCookie_ = TF_INVALID_COOKIE;
    CComPtr<ITfContext> textEditSinkContext_;

    // ITfKeyEventSink
    bool initKeyEventSink();
    void uninitKeyEventSink();
    BOOL processKey(WPARAM wParam, LPARAM lParam);
    BOOL keyDownHandled_ = false;

    // 输入法引擎和 UI
    InputEngine* inputEngine_;
    CandidateWindow* candidateWindow_;
    
    // 待提交的文本
    std::wstring pendingCommitText_;
    
    // 辅助函数
    void updateCandidateWindow();
    void commitText(const std::wstring& text);
};
} // namespace fcitx
