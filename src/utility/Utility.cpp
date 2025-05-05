#include "Utility.h"

std::vector<std::string> Utility::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delimiter)) {
        // Convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
        tokens.push_back(token);
    }
    return tokens;
}



std::vector<std::string> Utility::indexTokens(const std::string& text) {
    std::stringstream ss(text);
    std::string word;
    std::vector<std::string> tokens;
    while (ss >> word) {
        // Remove punctuation
        word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());
        // Convert to lowercase
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty())
            tokens.push_back(word);
    }
    
    // For single-character words or short common words like "is", ensure they're not filtered
    // This is important since these words are valid search terms
    
    return tokens;
}