// src/template.h - declarations only

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <iostream>
#include <string>

// Define Question and Answer structs here if not already defined elsewhere
struct Question {
    std::string id, user, text;
};

struct Answer {
    std::string answerID, questionID, user, text, sentiment;
};

// Function declarations (no implementation)
extern std::string sendCommand(const std::string& command);
extern void askQuestion(const std::string &username);
extern void answerQuestion(const std::string &username);
extern void deleteQuestion(const std::string& username);
extern void deleteAnswer(const std::string& username);
extern void deleteContent(const std::string& username);
extern void searchQuestions();
extern void voteAnswer(const std::string &answerID, const std::string &username);
extern void displayMenu(const std::string &username);
extern std::string authenticateUser(std::string command);


#endif
