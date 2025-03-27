#ifndef QUESTION_H
#define QUESTION_H

#include <iostream>
#include <string>
#include "../user/User.h"  // ✅ Ensure case matches actual folder name

class Question {
private:
    std::string questionID, text;
    User author; 

public:
    Question(std::string id, std::string txt, User u);
    void display();

    std::string toString() {
        return "UserID: " + author.getID() + ", Name: " + author.getName() + 
               ", Rating: " + std::to_string(author.getRating());
    }
    std::string getText() const { return text; }
};

#endif  // QUESTION_H
