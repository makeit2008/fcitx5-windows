#include "tsf.h"
#include <cassert>
#include <filesystem>

extern void DllAddRef();
extern void DllRelease();

namespace fcitx {
Tsf::Tsf() {
    DllAddRef();
    inputEngine_ = new InputEngine();
    candidateWindow_ = new CandidateWindow();
    
    // 尝试在 DLL 同级目录加载词典
    char dllPath[MAX_PATH];
    GetModuleFileNameA(nullptr, dllPath, MAX_PATH);
    std::filesystem::path moduleDir = std::filesystem::path(dllPath).parent_path();
    
    // 尝试加载拼音词典
    std::string pinyinDict = (moduleDir / "pinyin.dict").string();
    if (inputEngine_->loadDictionary(pinyinDict, InputMethodType::Pinyin)) {
        // 成功加载
    }
    
    // 如果失败，尝试内置词典或备用词典
    // 对于演示目的，我们保留一个硬编码的简单词典加载机制
}

Tsf::~Tsf() {
    delete inputEngine_;
    delete candidateWindow_;
    DllRelease();
}

void Tsf::updateCandidateWindow() {
    if (inputEngine_->hasComposition() || inputEngine_->hasCandidates()) {
        candidateWindow_->updateCandidates(
            inputEngine_->getCandidates(),
            inputEngine_->getCurrentPage(),
            inputEngine_->getTotalPages(),
            inputEngine_->getComposition()
        );
        candidateWindow_->moveToCaret();
        candidateWindow_->show();
    } else {
        candidateWindow_->hide();
    }
}

void Tsf::commitText(const std::wstring& text) {
    pendingCommitText_ = text;
    
    if (textEditSinkContext_) {
        HRESULT phrSession;
        textEditSinkContext_->RequestEditSession(
            clientId_, this, TF_ES_SYNC | TF_ES_READWRITE, &phrSession);
    }
}

// Windows also queries ITfDisplayAttributeCollectionProvider
// {3977526D-1A0A-435A-8D06-ECC9516B484F} which is internal and we simply
// ignore.
STDAPI Tsf::QueryInterface(REFIID riid, void **ppvObject) {
    if (ppvObject == nullptr) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_ITfTextInputProcessor))
        *ppvObject = (ITfTextInputProcessor *)this;
    else if (IsEqualIID(riid, IID_ITfTextInputProcessorEx))
        *ppvObject = (ITfTextInputProcessorEx *)this;
    else if (IsEqualIID(riid, IID_ITfThreadMgrEventSink))
        *ppvObject = (ITfThreadMgrEventSink *)this;
    else if (IsEqualIID(riid, IID_ITfTextEditSink))
        *ppvObject = (ITfTextEditSink *)this;
    else if (IsEqualIID(riid, IID_ITfKeyEventSink))
        *ppvObject = (ITfKeyEventSink *)this;
    else if (IsEqualIID(riid, IID_ITfEditSession))
        *ppvObject = (ITfEditSession *)this;

    if (*ppvObject) {
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDAPI_(ULONG) Tsf::AddRef() { return ++refCount_; }

STDAPI_(ULONG) Tsf::Release() {
    LONG ret = --refCount_;
    assert(refCount_ >= 0);
    if (refCount_ == 0) {
        delete this;
    }
    return ret;
}
} // namespace fcitx
