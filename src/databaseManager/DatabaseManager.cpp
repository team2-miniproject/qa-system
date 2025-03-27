#include "DatabaseManager.h"

void DatabaseManager::saveUser(const User& user) {
    std::ofstream file("data/users.txt", std::ios::app);
    file << user.getID() << "," << user.getName() << "," << user.getPasswordHash() << "\n";
    file.close();
}

std::vector<User> DatabaseManager::loadUsers() {
    std::vector<User> users;
    std::ifstream file("data/users.txt");
    std::string line;
    while (getline(file, line)) {
        std::vector<std::string> data = Utility::split(line, ',');
        if (data.size() == 3) {
            users.emplace_back(data[0], data[1], data[2]);
        }
    }
    file.close();
    return users;
}
