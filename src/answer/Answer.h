#ifndef ANSWER_H
#define ANSWER_H

#include <iostream>

class Answer {
private:
    std::string aID, qID, userID, text;
    int upvotes, downvotes, sentimentScore;

public:
    Answer(std::string id, std::string q, std::string u, std::string txt);
    void upvote();
    void downvote();
    void display();
};

#endif
