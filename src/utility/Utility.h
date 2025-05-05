#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

class Utility {
public:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> indexTokens(const std::string& text);
};





/*
static std::vector<std::string> indexTokens(const std::string& text) {
    std::vector<std::string> tokens = split(text, ' ');
    std::vector<std::string> processed;
    for (auto& token : tokens) {
        token.erase(std::remove_if(token.begin(), token.end(),
            [](char c) { return !std::isalpha(c); }), token.end());
        std::transform(token.begin(), token.end(), token.begin(),
            [](char c){ return std::tolower(c); });
        if (!token.empty()) processed.push_back(token);
    }
    return processed;
}
    */