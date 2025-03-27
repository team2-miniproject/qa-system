#include "SentimentAnalyzer.h"

int SentimentAnalyzer::analyze(const std::string& text) {
    int score = 0;
    if (text.find("good") != std::string::npos) score++;
    if (text.find("excellent") != std::string::npos) score += 2;
    if (text.find("bad") != std::string::npos) score--;
    if (text.find("terrible") != std::string::npos) score -= 2;
    return score;
}
