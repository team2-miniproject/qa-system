#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <mutex>
#include <set>
#include <map>
#include "./utility/Utility.h"
#include "./sentimentAnalyzer/SentimentAnalyzer.h"

#define PORT 8080
#define MAX_CLIENTS 5

std::unordered_map<std::string, std::string> users;
std::mutex users_mutex;

std::vector<std::string> loadData(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

// Helper to save lines to file
void saveData(const std::string& filename, const std::vector<std::string>& lines) {
    std::ofstream file(filename);
    for (const auto& line : lines) {
        file << line << "\n";
    }
}

void handleClient(int clientSocket, const struct sockaddr_in& clientAddress) {
    char buffer[4096] = {0};
    int bytesRead = read(clientSocket, buffer, 4096);

    //meainingful error handling
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    
    std::cout << "Connection from: " << clientIP << ":" 
              << ntohs(clientAddress.sin_port) << std::endl;

    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }

    std::string command(buffer);
    std::istringstream iss(command);
    std::string action, username, password, questionID, answerID, keyword;
    std::string questionText, answerText;

    iss >> action >> username;

    std::string response;
    std::lock_guard<std::mutex> lock(users_mutex);

    if (action == "REGISTER") {
        iss >> password;
        if (users.find(username) != users.end()) {
            response = "Registration failed: Username exists";
        } else {
            std::ofstream userFile("data/users.txt", std::ios::app);
            if (userFile) {
                userFile << username << " " << password << "\n";
                users[username] = password;
                response = "Registration successful";
            } else {
                response = "Registration failed: File error";
            }
        }
    } 
    else if (action == "LOGIN") {
        iss >> password;
        auto it = users.find(username);
        if (it != users.end() && it->second == password) {
            response = "Login successful";
        } else {
            response = "Login failed: Invalid credentials";
        }
    }
    else if (action == "ASK_QUESTION") {
        std::getline(iss >> std::ws, questionText);
        std::vector<std::string> questions = loadData("data/questions.txt");
        int nextID = questions.empty() ? 1 : std::stoi(questions.back().substr(0, questions.back().find(' '))) + 1;
        
        // Save question
        std::string questionLine = std::to_string(nextID) + " " + username + " " + questionText;
        questions.push_back(questionLine);
        saveData("data/questions.txt", questions);
    
        // --- Load existing inverted index ---
        std::map<std::string, std::set<int>> invertedIndex;
        std::ifstream existingIndexFile("data/index.txt");
        std::string indexLine;
        while (std::getline(existingIndexFile, indexLine)) {
            std::istringstream iss(indexLine);
            std::string token;
            iss >> token;
            int id;
            while (iss >> id) {
                invertedIndex[token].insert(id);
            }
        }

        // --- Tokenize the question text ---
        std::vector<std::string> tokens = Utility::indexTokens(questionText); 
    
        // --- Add tokens to index ---
        for (const auto& token : tokens) {
            invertedIndex[token].insert(nextID);
        }
    
        // --- Save updated index ---
        std::ofstream indexFile("data/index.txt");
        for (const auto& [token, ids] : invertedIndex) {
            indexFile << token;
            for (int id : ids)
                indexFile << " " << id;
            indexFile << "\n";
        }
    
        response = "Question posted successfully with ID: " + std::to_string(nextID);
    }
    else if (action == "ANSWER_QUESTION") {
        std::string answerUser, questionID, answerText;

        // Read the user ID and question ID
        iss >> answerUser;
        //std::cout << "[server] answerUser is " + answerUser << std::endl;
        iss >> questionID;
        //std::cout << "[server] questionID is " + questionID << std::endl;
        // Read the rest of the line as the answer text
        std::getline(iss >> std::ws, answerText); // ws skips leading whitespace
        //std::cout << "[server] answerText is " + answerText << std::endl;
        if (answerText.empty()) {
            response = "Answer cannot be empty";
        } else {
            // Load existing answers
            std::vector<std::string> answers = loadData("data/answers.txt");
    
            // Calculate next ID for the new answer
            int nextID = answers.empty() ? 1 : std::stoi(answers.back().substr(0, answers.back().find(' ') != std::string::npos ? answers.back().find(' ') : answers.back().size())) + 1;

            // Analyze sentiment
            std::string sentiment = SentimentAnalyzer::analyzeSentiment(answerText);
    
            // Save the answer in the correct format
            std::ofstream file("data/answers.txt", std::ios::app);
            if (file) {
                // Save in format: answerID questionID username answerText [sentiment]
                file << nextID << " " << questionID << " " << answerUser << " " 
                     << answerText << " [" << sentiment << "]\n";
            } else {
                response = "Failed to save answer";
            }
        }
    }   
    
    else if (action == "DELETE_QUESTION") {
        std::string username, questionID;
        iss >> username >> questionID;  // read all expected tokens

        std::vector<std::string> questions = loadData("data/questions.txt");
        std::vector<std::string> updatedQuestions;
        bool deleted = false;
        for (const auto& q : questions) {
            std::istringstream qss(q);
            std::string id, user;
            qss >> id >> user;
            if (id == questionID && user == username) {
                deleted = true;  // skip this question to delete it
            } else {
                updatedQuestions.push_back(q);
            }
        }
        // Remove answers for this question
        std::vector<std::string> answers = loadData("data/answers.txt");
        std::vector<std::string> updatedAnswers;
        std::set<std::string> deletedAnswerIDs;
        for (const auto& a : answers) {
            std::istringstream ass(a);
            std::string aid, qid, user;
            ass >> aid >> qid >> user;
            if (qid == questionID) {
                deletedAnswerIDs.insert(aid); // Mark for vote deletion
            } else {
                updatedAnswers.push_back(a);
            }
        }
        saveData("data/answers.txt", updatedAnswers);

        // Remove votes for deleted answers
        std::vector<std::string> votes = loadData("data/votes.txt");
        std::vector<std::string> updatedVotes;
        for (const auto& v : votes) {
            std::istringstream viss(v);
            std::string vAnswerID, vUser;
            int vVal;
            viss >> vAnswerID >> vUser >> vVal;
            if (deletedAnswerIDs.find(vAnswerID) == deletedAnswerIDs.end()) {
                updatedVotes.push_back(v);
            }
        }
        saveData("data/votes.txt", updatedVotes);

        // Save updated questions
        if (deleted) {
            saveData("data/questions.txt", updatedQuestions);
            response = "Question deleted successfully";
        } else {
            response = "You can only delete your own questions or question not found";
        }
    }
    
    else if (action == "DELETE_ANSWER") {
        std::string username, answerID;
        iss >> username >> answerID;
        std::vector<std::string> answers = loadData("data/answers.txt");
        std::vector<std::string> updatedAnswers;
        bool deleted = false;
    
        for (const auto& a : answers) {
            std::istringstream ass(a);
            std::string id, qid, user;
            ass >> id >> qid >> user;
            if (id == answerID && user == username) {
                deleted = true;
            } else {
                updatedAnswers.push_back(a);
            }
        }
    
        // Now remove votes for this answer
        std::vector<std::string> votes = loadData("data/votes.txt");
        std::vector<std::string> updatedVotes;
        for (const auto& v : votes) {
            std::istringstream viss(v);
            std::string vAnswerID, vUser;
            int vVal;
            viss >> vAnswerID >> vUser >> vVal;
            if (vAnswerID != answerID) {
                updatedVotes.push_back(v);
            }
        }
        saveData("data/votes.txt", updatedVotes);

        //save updated answers
        if (deleted) {
            saveData("data/answers.txt", updatedAnswers);
            response = "Answer deleted successfully";
        } else {
            response = "You can only delete your own answers or answer not found";
        }
    }
    
    else if (action == "SEARCH_QUESTIONS") {

        std::string keyword;
        std::getline(iss, keyword);
        
        std::vector<std::string> terms = Utility::indexTokens(keyword);  // Tokenize the keyword

        
        // Load questions
        std::vector<std::string> questions = loadData("data/questions.txt");
        
        // Load index: token -> set of question IDs
        std::unordered_map<std::string, std::set<int>> index;
        std::ifstream indexFile("data/index.txt");
        std::string line;
        
        // Populate the index with data from the inverted index file
        while (std::getline(indexFile, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            
            int id;
            while (iss >> id) {
                index[token].insert(id);  // Insert question ID for the token
            }
        }
        
        // Initialize result IDs as an empty set
        std::set<int> resultIDs;
        bool first = true;
        
        // Intersect the IDs for all tokens
        for (const auto& term : terms) {
            auto it = index.find(term);
            
            // If the term is not in the index, no results
            if (it == index.end()) {
                std::cout << "Not found in index" << std::endl;
                if (terms.size() == 1) {
                    // If only one search term and not found, return empty results
                    resultIDs.clear();
                    break;
                } else {
                    // Skip this term and continue with others
                    continue;
                }
            }
            
            // If it's the first valid token, assign its IDs directly to resultIDs
            if (first) {
                resultIDs = it->second;
                first = false;
            } else {
                std::set<int> temp;
                // Perform set intersection with current resultIDs and the current token's IDs
                std::set_intersection(resultIDs.begin(), resultIDs.end(),
                                      it->second.begin(), it->second.end(),
                                      std::inserter(temp, temp.begin()));
                resultIDs = temp;
            }
        }
        
        // If no valid tokens found or no results after intersection
        if (first || resultIDs.empty()) {
            response = "No matches found";
        } else {
            // Now gather all the questions that matched the IDs in resultIDs
            std::string results;
            for (const auto& q : questions) {
                std::istringstream qss(q);
                int id;
                qss >> id;  // Get the ID from the question
                
                // If this question's ID is in the result set, add it to the results
                if (resultIDs.count(id)) {
                    results += q + "\n";  // Append question to results
                }
            }
            
            // If no results were found, set response to "No matches found"
            response = results.empty() ? "No matches found" : results;
        }
    }
    
    else if (action == "VOTE_ANSWER") {
        std::string voteUser, answerID;
        int voteVal;
        iss >> voteUser >> answerID >> voteVal;
    
        // Check if user already voted
        std::vector<std::string> votes = loadData("data/votes.txt");
        bool alreadyVoted = false;
        for (const auto& v : votes) {
            std::istringstream viss(v);
            std::string vAnswerID, vUser;
            int vVal;
            viss >> vAnswerID >> vUser >> vVal;
            if (vAnswerID == answerID && vUser == voteUser) {
                alreadyVoted = true;
                break;
            }
        }
        if (alreadyVoted) {
            response = "You have already voted on this answer.";
        } else {
            std::ofstream file("data/votes.txt", std::ios::app);
            if (file) {
                file << answerID << " " << voteUser << " " << voteVal << "\n";
                response = "Vote recorded successfully";
            } else {
                response = "Failed to record vote";
            }
        }
    }
     else if (action == "GET_QUESTIONS") {
        std::vector<std::string> questions = loadData("data/questions.txt");
        if (questions.empty()) {
            response = "NO_QUESTIONS";
        } else {
            std::ostringstream oss;
            for (const auto& q : questions) {
                oss << q << "\n";
            }
            response = oss.str();
        }
    }
    else if (action == "GET_ANSWERS") {
        iss >> questionID;
        std::vector<std::string> answers = loadData("data/answers.txt");
        std::ostringstream oss;
        bool found = false;
        
        if (questionID == "all") {
            // Return all answers without filtering
            found = true;
            for (const auto& a : answers) {
                oss << a << "\n"; 
            }
        } else {
            for (const auto& a : answers) {
                std::istringstream aiss(a);
                std::string answerID, qID, user;
                aiss >> answerID >> qID >> user;            
                
                if (qID == questionID) {
                    found = true;
                    std::string text;
                    std::getline(aiss, text); // Get the rest including sentiment
                    
                    // Format: answerID username sentiment answer_text
                    oss << answerID << " " << user << " ";
                    
                    // Extract sentiment if present
                    std::string sentiment = "Neutral";
                    size_t pos = text.rfind(" [");
                    if (pos != std::string::npos && text.back() == ']') {
                        sentiment = text.substr(pos + 2, text.size() - pos - 3);
                        text = text.substr(0, pos);
                    }
                    
                    oss << sentiment << text << "\n";
                }
            }   
        }
        response = (found) ? oss.str() : "NO_ANSWERS";
        
    }
    
    else if (action == "GET_VOTES") {
        std::string answerID;
        iss >> answerID;
        std::vector<std::string> votesData = loadData("data/votes.txt");
        int totalVotes = 0;
        for (const auto& v : votesData) {
            std::istringstream viss(v);
            std::string vAnswerID, vUser;
            int voteVal;
            viss >> vAnswerID >> vUser >> voteVal;
            if (vAnswerID == answerID) totalVotes += voteVal;
        }
        response = std::to_string(totalVotes);
    }
    else if (action == "CHECK_VOTED") {
        std::string answerID, voteUser;
        iss >> answerID >> voteUser;
        std::vector<std::string> votes = loadData("data/votes.txt");
        bool alreadyVoted = false;
        for (const auto& v : votes) {
            std::istringstream viss(v);
            std::string vAnswerID, vUser;
            int vVal;
            viss >> vAnswerID >> vUser >> vVal;
            if (vAnswerID == answerID && vUser == voteUser) {
                alreadyVoted = true;
                break;
            }
        }
        response = alreadyVoted ? "VOTED" : "NOT_VOTED";
    }else {
        response = "Invalid command";
    }

    send(clientSocket, response.c_str(), response.size(), 0);
    close(clientSocket);
}

int main() {
    // Load existing users
    std::ifstream userFile("data/users.txt");
    std::string line;
    while (std::getline(userFile, line)) {
        size_t space = line.find(' ');
        if (space != std::string::npos) {
            users[line.substr(0, space)] = line.substr(space + 1);
        }
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, MAX_CLIENTS);
    std::cout << "Server running on port " << PORT << "\n";

    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
        std::thread(handleClient, clientSocket, clientAddress).detach();
    }

    close(serverSocket);
    return 0;
}
