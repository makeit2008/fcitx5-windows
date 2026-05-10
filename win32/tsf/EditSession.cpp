#include "tsf.h"
#include <cstring>

namespace fcitx {
STDMETHODIMP Tsf::DoEditSession(TfEditCookie ec) {
    // 如果有待提交的文本，处理它
    if (!pendingCommitText_.empty()) {
        // 获取插入接口
        CComPtr<ITfInsertAtSelection> insertAtSelection;
        if (textEditSinkContext_->QueryInterface(
                IID_ITfInsertAtSelection, (LPVOID *)&insertAtSelection) != S_OK) {
            pendingCommitText_.clear();
            return E_FAIL;
        }
        
        // 插入文本
        CComPtr<ITfRange> range;
        if (insertAtSelection->InsertTextAtSelection(
                ec, TF_IAS_NOQUERY | TF_IAS_QUERYONLY,
                pendingCommitText_.c_str(),
                static_cast<ULONG>(pendingCommitText_.length()),
                &range) != S_OK) {
            pendingCommitText_.clear();
            return E_FAIL;
        }
        
        pendingCommitText_.clear();
        return S_OK;
    }
    
    return S_OK;
}
} // namespace fcitx
