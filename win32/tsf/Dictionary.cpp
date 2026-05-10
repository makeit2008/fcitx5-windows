/**
 * @file Dictionary.cpp
 * @brief 词库加载系统实现
 */

#include "Dictionary.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <codecvt>
#include <locale>

namespace fcitx {

Dictionary::Dictionary() = default;
Dictionary::~Dictionary() = default;

bool Dictionary::load(const std::string& path, InputMethodType type) {
    currentType_ = type;
    loaded_ = false;
    
    entries_.clear();
    codeIndex_.clear();
    
    // 尝试各种格式
    if (loadTextFormat(path)) {
        buildIndex();
        loaded_ = true;
        return true;
    }
    
    return false;
}

bool Dictionary::loadTextFormat(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // 跳过注释和空行
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // 解析行: 汉字 编码1 编码2... [词频]
        std::istringstream iss(line);
        std::string wordStr;
        std::vector<std::string> codes;
        int32_t freq = 100;
        
        if (!(iss >> wordStr)) {
            continue;
        }
        
        // 读取编码
        std::string code;
        while (iss >> code) {
            // 检查是否是数字（词频）
            if (!code.empty() && std::all_of(code.begin(), code.end(),
                    [](char c) { return std::isdigit(c); })) {
                freq = std::stoi(code);
                break;
            }
            codes.push_back(code);
        }
        
        // 转换汉字到 wstring
        try {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring word = converter.from_bytes(wordStr);
            
            if (!codes.empty()) {
                DictEntry entry;
                entry.word = std::move(word);
                entry.codes = std::move(codes);
                entry.frequency = freq;
                entries_.push_back(std::move(entry));
            }
        } catch (...) {
            continue; // 忽略转换失败的行
        }
    }
    
    return !entries_.empty();
}

void Dictionary::buildIndex() {
    // 清空旧索引
    codeIndex_.clear();
    
    // 构建倒排索引
    for (size_t i = 0; i < entries_.size(); i++) {
        const auto& entry = entries_[i];
        for (const auto& code : entry.codes) {
            codeIndex_[code].push_back(i);
            
            // 同时也添加前缀索引，方便查询
            for (size_t j = 1; j <= code.size(); j++) {
                std::string prefix = code.substr(0, j);
                codeIndex_[prefix].push_back(i);
            }
        }
    }
}

std::vector<Candidate> Dictionary::lookup(const std::string& code) {
    std::vector<Candidate> candidates;
    
    if (!loaded_ || code.empty()) {
        return candidates;
    }
    
    // 使用 map 来按优先级排序并去重
    std::map<int32_t, std::vector<std::pair<std::wstring, std::string>>> sortedMap;
    
    // 查找精确匹配
    auto it = codeIndex_.find(code);
    if (it != codeIndex_.end()) {
        for (size_t idx : it->second) {
            const auto& entry = entries_[idx];
            
            // 检查是否有精确匹配的编码
            bool hasExact = false;
            for (const auto& c : entry.codes) {
                if (c == code) {
                    hasExact = true;
                    break;
                }
            }
            
            // 精确匹配的优先级更高
            int32_t priority = entry.frequency;
            if (hasExact) {
                priority += 10000;
            }
            
            sortedMap[priority].push_back({entry.word, entry.codes[0]});
        }
    }
    
    // 从高到低生成结果
    for (auto it = sortedMap.rbegin(); it != sortedMap.rend(); ++it) {
        for (const auto& [word, c] : it->second) {
            candidates.emplace_back(word, it->first, c);
        }
    }
    
    // 如果候选词太多，限制数量
    if (candidates.size() > 100) {
        candidates.resize(100);
    }
    
    return candidates;
}

} // namespace fcitx
