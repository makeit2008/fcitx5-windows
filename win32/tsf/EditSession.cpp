#include "tsf.h"
#include <cstring>

namespace fcitx {
STDMETHODIMP Tsf::DoEditSession(TfEditCookie ec) {
    // Get the insert-at-selection interface for inserting text
    CComPtr<ITfInsertAtSelection> insertAtSelection;
    if (textEditSinkContext_->QueryInterface(
            IID_ITfInsertAtSelection, (LPVOID *)&insertAtSelection) != S_OK) {
        return E_FAIL;
    }

    // Query for a range where we can insert text
    CComPtr<ITfRange> range;
    if (insertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, nullptr,
                                                 0, &range) != S_OK) {
        return E_FAIL;
    }

    // Get the composition interface for starting composition
    CComPtr<ITfContextComposition> contextComposition;
    if (textEditSinkContext_->QueryInterface(
            IID_ITfContextComposition, (void **)&contextComposition) != S_OK) {
        return E_FAIL;
    }

    // Start a composition session
    CComPtr<ITfComposition> composition;
    if (contextComposition->StartComposition(ec, range, this, &composition) !=
            S_OK ||
        composition == nullptr) {
        return E_FAIL;
    }

    // Demo: Insert sample text
    // In a real implementation, this would insert text from the input method engine
    const wchar_t sampleText[] = L"测试";
    if (range->SetText(ec, 0, sampleText, wcslen(sampleText)) != S_OK) {
        composition->EndComposition(ec);
        return E_FAIL;
    }

    // End the composition
    if (composition->EndComposition(ec) != S_OK) {
        return E_FAIL;
    }

    return S_OK;
}
} // namespace fcitx
