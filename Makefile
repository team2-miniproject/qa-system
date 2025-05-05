CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread

# Paths
SRC_DIR = src
OBJ_DIR = obj
DATA_DIR = data

# Source Files
SERVER_SOURCES = $(SRC_DIR)/server.cpp \
                 $(SRC_DIR)/user/User.cpp \
                 $(SRC_DIR)/question/Question.cpp \
                 $(SRC_DIR)/answer/Answer.cpp \
                 $(SRC_DIR)/sentimentAnalyzer/SentimentAnalyzer.cpp \
                 $(SRC_DIR)/search/SearchEngine.cpp \
                 $(SRC_DIR)/threadManager/ThreadManager.cpp \
                 $(SRC_DIR)/databaseManager/DatabaseManager.cpp \
                 $(SRC_DIR)/utility/Utility.cpp

CLIENT_SOURCES = $(SRC_DIR)/client.cpp \
                 $(SRC_DIR)/utility/Utility.cpp  # Critical fix here

# Object Files
SERVER_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SERVER_SOURCES))
CLIENT_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CLIENT_SOURCES))

# Targets
all: server client

# Compile rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Server
server: $(SERVER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_OBJECTS)

# Client
client: $(CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o client $(CLIENT_OBJECTS)

clean:
	rm -rf $(OBJ_DIR) server client
