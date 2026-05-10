/**
 * @file InputEngine.cpp
 * @brief 输入法引擎实现
 */

#include "InputEngine.h"
#include <algorithm>
#include <cctype>

namespace fcitx {

InputEngine::InputEngine() {
    dictionary_ = std::make_unique<Dictionary>();
}

InputEngine::~InputEngine() = default;

bool InputEngine::loadDictionary(const std::string& path, InputMethodType type) {
    currentType_ = type;
    bool success = dictionary_->load(path, type);
    updateCandidates();
    return success;
}

void InputEngine::switchInputMethod(InputMethodType type) {
    currentType_ = type;
    clearComposition();
}

void InputEngine::addChar(char ch) {
    // 只接受字母
    if (std::isalpha(static_cast<unsigned char>(ch))) {
        composition_ += std::tolower(static_cast<unsigned char>(ch));
        updateCandidates();
    }
}

void InputEngine::removeChar() {
    if (!composition_.empty()) {
        composition_.pop_back();
        updateCandidates();
    }
}

void InputEngine::clearComposition() {
    composition_.clear();
    allCandidates_.clear();
    currentCandidates_.clear();
    currentPage_ = 0;
}

std::wstring InputEngine::selectCandidate(int index) {
    int globalIndex = currentPage_ * getCandidatesPerPage() + index;
    if (globalIndex < 0 || globalIndex >= static_cast<int>(allCandidates_.size())) {
        return {};
    }
    
    std::wstring result = allCandidates_[globalIndex].text;
    clearComposition();
    return result;
}

std::wstring InputEngine::selectFirst() {
    return selectCandidate(0);
}

void InputEngine::pageUp() {
    if (currentPage_ > 0) {
        currentPage_--;
        updateCurrentPage();
    }
}

void InputEngine::pageDown() {
    if (currentPage_ < getTotalPages() - 1) {
        currentPage_++;
        updateCurrentPage();
    }
}

int InputEngine::getTotalPages() const {
    if (allCandidates_.empty()) {
        return 0;
    }
    return (allCandidates_.size() + getCandidatesPerPage() - 1) / getCandidatesPerPage();
}

void InputEngine::updateCandidates() {
    allCandidates_.clear();
    currentPage_ = 0;
    
    if (!composition_.empty()) {
        allCandidates_ = dictionary_->lookup(composition_);
    }
    
    updateCurrentPage();
}

void InputEngine::updateCurrentPage() {
    currentCandidates_.clear();
    
    int start = currentPage_ * getCandidatesPerPage();
    int end = std::min(start + getCandidatesPerPage(), static_cast<int>(allCandidates_.size()));
    
    for (int i = start; i < end; i++) {
        currentCandidates_.push_back(allCandidates_[i]);
    }
}

} // namespace fcitx
