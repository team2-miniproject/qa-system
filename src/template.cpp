#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

struct Question {
    std::string id;
    std::string user;
    std::string text;
};

struct Answer {
    std::string answerID;
    std::string questionID;
    std::string user;
    std::string text;
};

std::vector<Question> questions;

void loadQuestions() {
    std::ifstream file("data/questions.txt");
    std::string id, user, text, line;

    questions.clear(); // Clear the existing questions list
    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> id >> user;
        std::getline(iss, text);
        questions.push_back({id, user, text});
    }
    file.close();
}

void saveQuestion(const std::string &question, const std::string &username) {
    loadQuestions(); // Load existing questions to determine the next ID
    int nextID = questions.empty() ? 1 : std::stoi(questions.back().id) + 1; // Get next available ID

    std::ofstream file("data/questions.txt", std::ios::app);
    if (file.is_open()) {
        file << nextID << " " << username << " " << question << "\n";
        file.close();
        questions.push_back({std::to_string(nextID), username, question});
        std::cout << "Question posted successfully with ID: " << nextID << "\n";
    } else {
        std::cout << "Error saving question.\n";
    }
}

void askQuestion(const std::string &username) {
    std::string question;
    std::cin.ignore(); // Clear buffer
    std::cout << "Enter your question: ";
    std::getline(std::cin, question);
    saveQuestion(question, username);
}

void answerQuestion(const std::string &username) {
    loadQuestions(); // Load questions before displaying
    if (questions.empty()) {
        std::cout << "No questions available to answer.\n";
        return;
    }

    std::cout << "Available Questions:\n";
    for (const auto &q : questions) {
        std::cout << q.id << ". " << q.text << " (Asked by: " << q.user << ")\n";
    }

    std::string qID;
    std::cout << "Enter the Question ID to answer: ";
    std::cin >> qID;
    std::cin.ignore(); // Clear input buffer

    bool found = false;
    for (const auto &q : questions) {
        if (q.id == qID) {
            found = true;
            std::string answer;
            std::cout << "Enter your answer: ";
            std::getline(std::cin, answer);

            // Determine next answer ID
            std::ifstream answerFile("data/answers.txt");
            std::string line, lastAnswerID;
            while (getline(answerFile, line)) {
                std::istringstream iss(line);
                iss >> lastAnswerID;
            }
            answerFile.close();
            int nextAID = lastAnswerID.empty() ? 1 : std::stoi(lastAnswerID) + 1;

            // Store the answer
            std::ofstream file("data/answers.txt", std::ios::app);
            if (file.is_open()) {
                file << nextAID << " " << qID << " " << username << " " << answer << "\n";
                file.close();
                std::cout << "Answer posted successfully with Answer ID: " << nextAID << "\n";
            } else {
                std::cout << "Error saving answer.\n";
            }
            break;
        }
    }

    if (!found) {
        std::cout << "Invalid Question ID. Try again.\n";
    }
}

void displayMenu(const std::string &username) {
    int choice;
    while (true) {
        std::cout << "\n1. Search Questions\n2. Ask a Question\n3. Answer a Question\n4. Upvote/Downvote\n5. Sentiment Analysis\n6. Exit\nChoice: ";
        std::cin >> choice;
        switch (choice) {
            case 1: std::cout << "Searching Questions...\n"; break;
            case 2: askQuestion(username); break;
            case 3: answerQuestion(username); break;
            case 4: std::cout << "Voting...\n"; break;
            case 5: std::cout << "Performing Sentiment Analysis...\n"; break;
            case 6: std::cout << "Exiting...\n"; return;
            default: std::cout << "Invalid choice. Try again.\n";
        }
    }
}
