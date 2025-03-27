#include "Answer.h"

Answer::Answer(std::string id, std::string q, std::string u, std::string txt) 
    : aID(id), qID(q), userID(u), text(txt), upvotes(0), downvotes(0), sentimentScore(0) {}

void Answer::upvote() { upvotes++; }
void Answer::downvote() { downvotes++; }

void Answer::display() {
    std::cout << "A: " << text << " | 👍 " << upvotes << " 👎 " << downvotes << std::endl;
}
