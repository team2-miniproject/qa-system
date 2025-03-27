
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "../user/User.h"  // Ensure correct relative path
#include "../utility/Utility.h"
#include <vector>
#include <fstream>

class DatabaseManager {
public:
    void saveUser(const User& user);
    std::vector<User> loadUsers();
};

#endif