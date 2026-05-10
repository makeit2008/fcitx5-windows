/**
 * @file CandidateWindow.cpp
 * @brief 候选词窗口实现
 */

#define NOMINMAX
#include "CandidateWindow.h"
#include <CommCtrl.h>
#include <codecvt>
#include <locale>
#include <algorithm>

#pragma comment(lib, "comctl32.lib")

namespace fcitx {

CandidateWindow::CandidateWindow() {
    registerClass();
    createWindow();
}

CandidateWindow::~CandidateWindow() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
    }
}

void CandidateWindow::registerClass() {
    if (classRegistered_) {
        return;
    }

    WNDCLASSEXW wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = windowProc;
    wcex.hInstance = GetModuleHandleW(nullptr);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(248, 248, 250));
    wcex.lpszClassName = L"Fcitx5CandidateWindow";

    RegisterClassExW(&wcex);
    classRegistered_ = true;
}

void CandidateWindow::createWindow() {
    if (hwnd_) {
        return;
    }

    hwnd_ = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"Fcitx5CandidateWindow",
        L"Fcitx5",
        WS_POPUP,
        100, 100,
        windowWidth_, windowHeight_,
        nullptr, nullptr,
        GetModuleHandleW(nullptr),
        this
    );

    SetWindowLongPtrW(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

LRESULT CALLBACK CandidateWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CandidateWindow* pThis = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<CandidateWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    } else {
        pThis = reinterpret_cast<CandidateWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (!pThis) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            pThis->draw(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

void CandidateWindow::draw(HDC hdc) {
    RECT clientRect;
    GetClientRect(hwnd_, &clientRect);

    // 背景
    HBRUSH bgBrush = CreateSolidBrush(RGB(248, 248, 250));
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    // 边框
    HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 210));
    HGDIOBJ oldPen = SelectObject(hdc, borderPen);
    Rectangle(hdc, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);

    // 绘制输入串
    if (!composition_.empty()) {
        RECT compRect = {Margin, Margin, clientRect.right - Margin, HeaderHeight};
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(80, 80, 100));

        HFONT hFont = CreateFontW(
            20, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            L"Consolas"
        );

        HGDIOBJ oldFont = SelectObject(hdc, hFont);

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring compWStr = converter.from_bytes(composition_);
        DrawTextW(hdc, compWStr.c_str(), -1, &compRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
        DeleteObject(hFont);

        // 分隔线
        HPEN linePen = CreatePen(PS_SOLID, 1, RGB(220, 220, 230));
        oldPen = SelectObject(hdc, linePen);
        MoveToEx(hdc, Margin, HeaderHeight, nullptr);
        LineTo(hdc, clientRect.right - Margin, HeaderHeight);
        SelectObject(hdc, oldPen);
        DeleteObject(linePen);
    }

    // 绘制候选词
    int start = currentPage_ * 9; // 每页9个
    int end = std::min(start + 9, static_cast<int>(candidates_.size()));

    for (int i = start; i < end; i++) {
        int idx = i - start;
        RECT itemRect = {
            Margin,
            HeaderHeight + Margin + idx * ItemHeight,
            clientRect.right - Margin,
            HeaderHeight + Margin + (idx + 1) * ItemHeight
        };

        // 高亮选中项
        if (i == selectedIndex_) {
            HBRUSH highlightBrush = CreateSolidBrush(RGB(220, 240, 255));
            FillRect(hdc, &itemRect, highlightBrush);
            DeleteObject(highlightBrush);
        }

        // 绘制序号
        SetTextColor(hdc, RGB(120, 120, 140));
        std::wstring idxStr = std::to_wstring(idx + 1) + L". ";

        HFONT hFont = CreateFontW(
            24, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            L"Microsoft YaHei"
        );

        HGDIOBJ oldFont = SelectObject(hdc, hFont);

        RECT idxRect = itemRect;
        idxRect.right = itemRect.left + 45;
        DrawTextW(hdc, idxStr.c_str(), -1, &idxRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // 绘制候选词
        SetTextColor(hdc, RGB(40, 40, 60));
        RECT textRect = itemRect;
        textRect.left = idxRect.right;
        DrawTextW(hdc, candidates_[i].text.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
    }

    // 绘制页码
    if (totalPages_ > 1) {
        std::wstring pageStr = std::to_wstring(currentPage_ + 1) + L"/" + std::to_wstring(totalPages_);

        HFONT hFont = CreateFontW(
            16, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            L"Microsoft YaHei"
        );

        HGDIOBJ oldFont = SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(100, 100, 120));
        SetBkMode(hdc, TRANSPARENT);

        RECT pageRect = {Margin, clientRect.bottom - 35, clientRect.right - Margin, clientRect.bottom - 5};
        DrawTextW(hdc, pageStr.c_str(), -1, &pageRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
    }
}

void CandidateWindow::calculateSize() {
    int visibleCount = std::min(9, static_cast<int>(candidates_.size()));
    windowHeight_ = HeaderHeight + 2 * Margin + visibleCount * ItemHeight;

    if (totalPages_ > 1) {
        windowHeight_ += 40;
    }

    windowWidth_ = 320;

    SetWindowPos(hwnd_, nullptr, 0, 0, windowWidth_, windowHeight_,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CandidateWindow::show() {
    if (!visible_ && hwnd_) {
        ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
        visible_ = true;
    }
}

void CandidateWindow::hide() {
    if (visible_ && hwnd_) {
        ShowWindow(hwnd_, SW_HIDE);
        visible_ = false;
    }
}

void CandidateWindow::updateCandidates(const std::vector<Candidate>& candidates,
                                       int currentPage, int totalPages,
                                       const std::string& composition) {
    candidates_ = candidates;
    currentPage_ = currentPage;
    totalPages_ = totalPages;
    composition_ = composition;
    selectedIndex_ = currentPage * 9;

    calculateSize();

    if (hwnd_) {
        InvalidateRect(hwnd_, nullptr, TRUE);
    }
}

void CandidateWindow::moveToCaret() {
    POINT caretPos = {100, 200};

    GUITHREADINFO guiInfo = {sizeof(GUITHREADINFO)};
    if (GetGUIThreadInfo(GetCurrentThreadId(), &guiInfo)) {
        if (guiInfo.hwndCaret) {
            GetCaretPos(&caretPos);
            ClientToScreen(guiInfo.hwndCaret, &caretPos);
        } else if (guiInfo.hwndFocus) {
            RECT rect;
            GetWindowRect(guiInfo.hwndFocus, &rect);
            caretPos.x = rect.left;
            caretPos.y = rect.top + 20;
        }
    }

    if (hwnd_) {
        SetWindowPos(hwnd_, HWND_TOPMOST,
                     caretPos.x, caretPos.y + 25,
                     0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

bool CandidateWindow::handleKeyDown(WPARAM wParam) {
    switch (wParam) {
        case VK_DOWN:
        case VK_RIGHT: {
            int maxIdx = std::min((currentPage_ + 1) * 9, static_cast<int>(candidates_.size())) - 1;
            if (selectedIndex_ < maxIdx) {
                selectedIndex_++;
                InvalidateRect(hwnd_, nullptr, TRUE);
            }
            return true;
        }
        case VK_UP:
        case VK_LEFT: {
            int minIdx = currentPage_ * 9;
            if (selectedIndex_ > minIdx) {
                selectedIndex_--;
                InvalidateRect(hwnd_, nullptr, TRUE);
            }
            return true;
        }
        case VK_NEXT: // PageDown
            if (currentPage_ < totalPages_ - 1) {
                currentPage_++;
                selectedIndex_ = currentPage_ * 9;
                calculateSize();
                InvalidateRect(hwnd_, nullptr, TRUE);
            }
            return true;
        case VK_PRIOR: // PageUp
            if (currentPage_ > 0) {
                currentPage_--;
                selectedIndex_ = currentPage_ * 9;
                calculateSize();
                InvalidateRect(hwnd_, nullptr, TRUE);
            }
            return true;
        default:
            return false;
    }
}

} // namespace fcitx
