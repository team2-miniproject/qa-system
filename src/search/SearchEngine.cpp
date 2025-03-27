#include "SearchEngine.h"

std::vector<Question> SearchEngine::searchQuestions(const std::string& keyword, const std::vector<Question>& allQuestions) {
    std::vector<Question> results;
    for (const auto& question : allQuestions) {
        if (question.getText().find(keyword) != std::string::npos) {
            results.push_back(question);
        }
    }
    return results;
}
