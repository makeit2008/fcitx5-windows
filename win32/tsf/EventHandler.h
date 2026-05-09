// This file provides enhanced event handling for Fcitx5 Windows
// It processes keyboard input and manages text composition

#pragma once

#include <Windows.h>
#include <msctf.h>

namespace fcitx {
    // Helper structure for managing keystroke information
    struct KeystrokeInfo {
        WPARAM virtualKeyCode;
        LPARAM lParam;
        UINT scanCode;
        BOOL isExtendedKey;
    };

    // Extract keystroke information from Windows message parameters
    inline KeystrokeInfo extractKeystrokeInfo(WPARAM wParam, LPARAM lParam) {
        KeystrokeInfo info{};
        info.virtualKeyCode = wParam;
        info.lParam = lParam;
        info.scanCode = (lParam >> 16) & 0xFF;
        info.isExtendedKey = (lParam >> 24) & 0x1;
        return info;
    }

    // Common key codes
    enum class SpecialKeyCode : WPARAM {
        Escape = VK_ESCAPE,
        Enter = VK_RETURN,
        Backspace = VK_BACK,
        Tab = VK_TAB,
        Space = VK_SPACE,
        Delete = VK_DELETE,
        Home = VK_HOME,
        End = VK_END,
        LeftArrow = VK_LEFT,
        RightArrow = VK_RIGHT,
        UpArrow = VK_UP,
        DownArrow = VK_DOWN,
        PageUp = VK_PRIOR,
        PageDown = VK_NEXT,
        LeftShift = VK_LSHIFT,
        RightShift = VK_RSHIFT,
        LeftCtrl = VK_LCONTROL,
        RightCtrl = VK_RCONTROL,
        LeftAlt = VK_LMENU,
        RightAlt = VK_RMENU,
    };

    // Helper to check if a key code is a special key
    inline bool isSpecialKey(WPARAM wParam) {
        return (wParam >= VK_ESCAPE && wParam <= VK_HELP) ||
               (wParam >= VK_LWIN && wParam <= VK_APPS);
    }

} // namespace fcitx
