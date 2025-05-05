#include "template.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <limits>
#include "./utility/Utility.h"

#define PORT 8080
//#define SERVER_IP "192.168.29.176" //my home
#define SERVER_IP "192.168.208.196"
std::string sendCommand(const std::string& command) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return "CONNECTION_ERROR";
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return "CONNECTION_ERROR";
    }

    send(sock, command.c_str(), command.size(), 0);
    char buffer[4096] = {0};
    read(sock, buffer, 4096);
    close(sock);

    return std::string(buffer);
}

void askQuestion(const std::string& username) {
    std::cin.ignore();
    std::cout << "Enter your question: ";
    std::string question;
    std::getline(std::cin, question);
    std::string response = sendCommand("ASK_QUESTION " + username + " " + question);
    std::cout << response << "\n";
}

void answerQuestion(const std::string& username) {
    // Get and display questions
    std::string questionsResponse = sendCommand("GET_QUESTIONS");
    if (questionsResponse == "NO_QUESTIONS") {
        std::cout << "\nNo questions available to answer.\n";
        return;
    }

    std::vector<std::string> questionLines = Utility::split(questionsResponse, '\n');
    std::cout << "\nAvailable Questions:\n";
    for (const auto& qline : questionLines) {
        if (qline.empty()) continue;
        size_t firstSpace = qline.find(' ');
        size_t secondSpace = qline.find(' ', firstSpace + 1);
        std::string qid = qline.substr(0, firstSpace);
        std::string qUser = qline.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        std::string qText = qline.substr(secondSpace + 1);
        std::cout << qid << ".  " << qText << " (Asked by: " << qUser << ")\n";
    }

    // Get question ID input
    std::string qID;
    std::cout << "Enter the Question ID to view answers or answer it: ";
    std::cin >> qID;
    std::cin.ignore();

    // Get question text for display
    std::string qText = "";
    for (const auto& qline : questionLines) {
        if (qline.substr(0, qline.find(' ')) == qID) {
            qText = qline.substr(qline.find(' ', qline.find(' ') + 1) + 1);
            break;
        }
    }

    // Get and display answers
    std::string answersResponse = sendCommand("GET_ANSWERS 0 " + qID);
    std::cout << "\nAnswers for Question: " << qText << "\n";
    
    if (answersResponse == "NO_ANSWERS") {
        std::cout << "No answers yet.\n";
    } else {
        std::vector<std::string> answerLines = Utility::split(answersResponse, '\n');
        
        for (const auto& aline : answerLines) {
            if (aline.empty()) continue;
            
            size_t firstSpace = aline.find(' ');
            size_t secondSpace = aline.find(' ', firstSpace + 1);
            size_t thirdSpace = aline.find(' ', secondSpace + 1);
            
            std::string aID = aline.substr(0, firstSpace);
            std::string aUser = aline.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            std::string sentiment = aline.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
            std::string aText = aline.substr(thirdSpace + 1);
            
            // Get votes
            std::string votes = sendCommand("GET_VOTES 0 " + aID);
            
            std::cout << " by " << aUser << " [" << votes << " votes, " << sentiment << " sentiment]: " << aText << "\n";
            
            // Check if user already voted
            std::string voteCheck = sendCommand("CHECK_VOTED 0 " + aID + " " + username);
            if (voteCheck == "NOT_VOTED") {
                std::cout << "Do you want to vote on this Answer? (y/n): ";
                char voteChoice;
                std::cin >> voteChoice;
                if (tolower(voteChoice) == 'y') {
                    int voteVal;
                    while (true) {
                        std::cout << "Vote on Answer ID " << aID << " (1 for Upvote, -1 for Downvote): ";
                        std::cin >> voteVal;
                        if (voteVal == 1 || voteVal == -1) break;
                        std::cout << "Invalid input. Please enter 1 (Upvote) or -1 (Downvote).\n";
                    }
                    std::string voteCmd = "VOTE_ANSWER 0 " + username + " " + aID + " " + std::to_string(voteVal);
                    std::string voteResult = sendCommand(voteCmd);
                    std::cout << voteResult << "!\n";
                }
            }
        }
    }
    
    // Ask if user wants to answer this question
    std::cout << "\nDo you want to answer this question? (y/n): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (tolower(choice) == 'y') {
        std::cout << "\nEnter your answer: ";
        std::string answerText;
        std::getline(std::cin, answerText);
        
        if (!answerText.empty()) {
            std::string response = sendCommand("ANSWER_QUESTION 0 " + username + " " + qID + " " + answerText);
            std::cout << response << "\n";
        } else {
            std::cout << "Answer cannot be empty.\n";
        }
    } else {
        std::cout << "Returning to menu...\n";
    }
}

// Updated deletion functions
void deleteQuestion(const std::string& username) {
    std::string response = sendCommand("GET_QUESTIONS");
    if (response == "NO_QUESTIONS") {
        std::cout << "No questions to delete.\n";
        return;
    }

    std::vector<std::string> questions = Utility::split(response, '\n');
    std::vector<std::string> userQuestions;
    
    std::cout << "\nYour Questions:\n";
    for (const auto& q : questions) {
        size_t space1 = q.find(' ');
        size_t space2 = q.find(' ', space1 + 1);
        if (q.substr(space1 + 1, space2 - space1 - 1) == username) {
            std::string qid = q.substr(0, space1);
            std::string qtext = q.substr(space2 + 1);
            std::cout << "[" << qid << "] " << qtext << "\n";
            userQuestions.push_back(qid);
        }
    }

    if (userQuestions.empty()) {
        std::cout << "You have no questions to delete.\n";
        return;
    }

    std::string qid;
    while (true) {
        std::cout << "Enter Question ID to delete (0 to cancel): ";
        std::cin >> qid;
        if (qid == "0") return;
        if (std::find(userQuestions.begin(), userQuestions.end(), qid) != userQuestions.end()) break;
        std::cout << "Invalid ID. Please enter one of your question IDs.\n";
    }

    std::string result = sendCommand("DELETE_QUESTION 0 " + username + " " + qid);
    std::cout << result << "\n";
}

void deleteAnswer(const std::string& username) {
    std::string response = sendCommand("GET_ANSWERS 0 all");
    if (response == "NO_ANSWERS") {
        std::cout << "No answers to delete.\n";
        return;
    }

    std::vector<std::string> answers = Utility::split(response, '\n');
    std::vector<std::string> userAnswers;
    
    std::cout << "\nYour Answers:\n";
    for (const auto& a : answers) {
        size_t space1 = a.find(' ');
        size_t space2 = a.find(' ', space1 + 1);
        size_t space3 = a.find(' ', space2 + 1);
        if (a.substr(space2 + 1, space3 - space2 - 1) == username) {
            std::string aid = a.substr(0, space1);
            std::string answerText = a.substr(space3 + 1);
            std::cout << "[" << aid << "] " << answerText << "\n";
            userAnswers.push_back(aid);
        }
    }

    if (userAnswers.empty()) {
        std::cout << "You have no answers to delete.\n";
        return;
    }

    std::string aid;
    while (true) {
        std::cout << "Enter Answer ID to delete (0 to cancel): ";
        std::cin >> aid;
        if (aid == "0") return;
        if (std::find(userAnswers.begin(), userAnswers.end(), aid) != userAnswers.end()) break;
        std::cout << "Invalid ID. Please enter one of your answer IDs.\n";
    }

    std::string result = sendCommand("DELETE_ANSWER 0 " + username + " " + aid);
    std::cout << result << "\n";
}

void deleteContent(const std::string& username) {
    std::cout << "1. Delete Question\n2. Delete Answer\nChoice: ";
    int choice;
    std::cin >> choice;
    if (choice == 1) deleteQuestion(username);
    else if (choice == 2) deleteAnswer(username);
    else std::cout << "Invalid choice\n";
}

void searchQuestions() {
    std::cin.ignore();
    std::cout << "Enter search phrase: ";
    std::string keyword;
    std::getline(std::cin, keyword);
    std::string command = "SEARCH_QUESTIONS 0 " + keyword;

    std::string response = sendCommand(command);
    if (response == "No matches found") {
        std::cout << response << "\n";
    } else {
        std::cout << "\nSearch Results:\n";
        std::vector<std::string> results = Utility::split(response, '\n');
        for (const auto& line : results) {
            if (!line.empty()) {
                size_t firstSpace = line.find(' ');
                size_t secondSpace = line.find(' ', firstSpace + 1);
                std::cout << "Q" << line.substr(0, firstSpace) << " - " 
                          << line.substr(secondSpace + 1) << "\n";
            }
        }
    }
}


void voteAnswer(const std::string& username) {
    std::cout << "Enter Answer ID: ";
    std::string aid;
    std::cin >> aid;
    std::string response = sendCommand("VOTE_ANSWER 0 " + username + " " + aid);
    std::cout << response << "\n";
}

void displayMenu(const std::string &username) {
    int choice;
    while (true) {
        std::cout << "\n1. Ask a Question\n2. Answer Questions\n3. Search Questions\n4. Delete Content\n5. Exit\nChoice: ";
        std::cin >> choice;
        switch(choice) {
            case 1: askQuestion(username); break;
            case 2: answerQuestion(username); break;
            case 3: searchQuestions(); break;
            case 4: deleteContent(username); break;
            case 5: std::cout << "Exiting...\n"; return;
            default: std::cout << "Invalid choice\n";
        }
    }
}

std::string authenticateUser(std::string command) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return "CONNECTION_ERROR";
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        return "CONNECTION_ERROR";
    }

    send(sock, command.c_str(), command.size(), 0);
    char buffer[4096] = {0};
    read(sock, buffer, 4096);
    close(sock);

    return std::string(buffer);
}

int main() {
    std::string username, password;
    int choice;
    std::string authResponse;

    while (true) {
        std::cout << "1. Register\n2. Login\nChoice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Username: ";
        std::getline(std::cin, username);

        std::cout << "Password: ";
        std::getline(std::cin, password);

        if (choice == 1) {
            // Register user
            authResponse = authenticateUser("REGISTER " + username + " " + password);
            std::cout << authResponse << std::endl;
            if (authResponse.find("successful") != std::string::npos) {
                std::cout << "Please login to continue.\n\n";
                continue;  // Loop back to login prompt
            } else {
                std::cout << "Registration failed. Try again.\n\n";
                continue;
            }
        } else if (choice == 2) {
            // Login user
            authResponse = authenticateUser("LOGIN " + username + " " + password);
            if (authResponse.find("successful") != std::string::npos) {
                std::cout << authResponse << std::endl;
                displayMenu(username);
                break;
            } else {
                std::cout << authResponse << "\n\n";
                continue;
            }
        } else {
            std::cout << "Invalid choice. Please enter 1 or 2.\n\n";
        }
    }

    return 0;
}