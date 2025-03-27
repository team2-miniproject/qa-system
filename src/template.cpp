#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

struct Question {
    std::string id, user, text;
};

struct Answer {
    std::string answerID, questionID, user, text;
};

std::vector<Question> questions;
std::vector<Answer> answers;
std::map<std::string, int> votes;

void loadQuestions() {
    std::ifstream file("data/questions.txt");
    std::string id, user, text, line;
    questions.clear();
    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> id >> user;
        std::getline(iss, text);
        questions.push_back({id, user, text});
    }
}

void loadAnswers() {
    std::ifstream file("data/answers.txt");
    std::string answerID, questionID, user, text, line;
    answers.clear();

    while (getline(file, line)) {
        if (line.empty()) continue;  // Ignore blank lines
        
        std::istringstream iss(line);
        if (!(iss >> answerID >> questionID >> user)) {
            std::cerr << "Error: Corrupt line in answers.txt: " << line << "\n";
            continue;
        }

        std::getline(iss, text);
        answers.push_back({answerID, questionID, user, text});
    }
}

void loadVotes() {
    std::ifstream file("data/votes.txt");
    std::string answerID, user;
    int vote;
    votes.clear();
    while (file >> answerID >> user >> vote) {
        votes[answerID] += vote;
    }
}

void saveQuestion(const std::string &question, const std::string &username) {
    loadQuestions();
    int nextID = questions.empty() ? 1 : std::stoi(questions.back().id) + 1;
    std::ofstream file("data/questions.txt", std::ios::app);
    if (file.is_open()) {
        file << nextID << " " << username << " " << question << "\n";
        file.close();
        questions.push_back({std::to_string(nextID), username, question});
        std::cout << "Question posted successfully with ID: " << nextID << "\n";
    }
}

void askQuestion(const std::string &username) {
    std::string question;
    std::cin.ignore();
    std::cout << "Enter your question: ";
    std::getline(std::cin, question);
    saveQuestion(question, username);
}

void voteAnswer(const std::string &answerID, const std::string &username) {
    std::cout << "Do you want to vote on this Answer? (y/n): ";
    char votechoice;
  
    std::cin >> votechoice;
    if (votechoice == 'y' || votechoice == 'Y') {
        int vote;
        while (true) {
            std::cout << "Vote on Answer ID " << answerID << " (1 for Upvote, -1 for Downvote): ";
            std::cin >> vote;
            if (vote == 1 || vote == -1) break;
            std::cout << "Invalid input. Please enter 1 (Upvote) or -1 (Downvote).\n";
        }

        std::ofstream file("data/votes.txt", std::ios::app);
        if (file.is_open()) {
            file << answerID << " " << username << " " << vote << "\n";
            file.close();
            votes[answerID] += vote;
            std::cout << "Vote recorded successfully!\n";
        }
    }else if (votechoice == 'n' || votechoice == 'N') {
        std::cout << "ok...\n";
        return; // **Exit without recording an answer**
    }

}

void answerQuestion(const std::string &username) {
    loadQuestions();
    loadAnswers();
    loadVotes();

    char answerChoice;

    if (questions.empty()) {
        std::cout << "No questions available.\n";
        return;
    }

    std::cout << "\nAvailable Questions:\n";
    for (const auto &q : questions) {
        std::cout << q.id << ". " << q.text << " (Asked by: " << q.user << ")\n";
    }

    std::string qID;
    std::cout << "Enter the Question ID to view answers or answer it: ";
    std::cin >> qID;
    std::cin.ignore();

    bool found = false;
    for (const auto &q : questions) {
        if (q.id == qID) {
            found = true;
            std::cout << "\nAnswers for Question: " << q.text << "\n";
            bool hasAnswers = false;
            for (const auto &a : answers) {
                if (a.questionID == qID) {
                    int voteCount = votes[a.answerID];
                    std::cout << " by " << a.user << " [" << voteCount << " votes]: " << a.text << "\n";
                    hasAnswers = true;
                    voteAnswer(a.answerID, username); // **Asks if they want to vote**
                }
            }
            if (!hasAnswers) std::cout << "No answers yet.\n";
            
            std::cout << "\nDo you want to answer this question? (y/n): ";
            std::cin >> answerChoice;
            std::cin.ignore();

            if (answerChoice == 'n' || answerChoice == 'N') {
                std::cout << "Returning to menu...\n";
                return; // **Exit without recording an answer**
            }else if (answerChoice != 'y' && answerChoice != 'Y') {
                std::cout << "Invalid choice. try again\n";
                return;
            }

            std::string answer;
            std::cout << "\nEnter your answer: ";
            std::getline(std::cin, answer);

            int nextAID;
            try {
                nextAID = answers.empty() ? 1 : std::stoi(answers.back().answerID) + 1;
            } catch (const std::exception &e) {
                std::cerr << "Error: Invalid Answer ID format in answers.txt\n";
                nextAID = 1; // Reset ID safely
            }

            std::ofstream file("data/answers.txt", std::ios::app);
            if (file.is_open()) {
                file << nextAID << " " << qID << " " << username << " " << answer << "\n";
                file.close();
                answers.push_back({std::to_string(nextAID), qID, username, answer});
                std::cout << "Answer posted successfully with Answer ID: " << nextAID << "\n";
            }
            break;
        }
    }

    if (!found) std::cout << "Invalid Question ID. Try again.\n";
}

void displayMenu(const std::string &username) {
    int choice;
    while (true) {
        std::cout << "\n1. Ask a Question\n2. Questions\n3. Sentiment Analysis\n4. Exit\nChoice: ";
        std::cin >> choice;
        switch (choice) {
            case 1: askQuestion(username); break;
            case 2: answerQuestion(username); break;
            case 3: std::cout << "Performing Sentiment Analysis...\n"; break;
            case 4: std::cout << "Exiting...\n"; return;
            default: std::cout << "Invalid choice. Try again.\n";
        }
    }
}
