#include "Question.h"

Question::Question(std::string id, std::string txt, User u) 
    : questionID(id), text(txt), author(u) {}

void Question::display() {
    std::cout << "Q: " << text << " (Posted by: " << author.getName() << ")" << std::endl;
}
