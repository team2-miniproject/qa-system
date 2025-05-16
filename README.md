# Q&A System (C++ with Sockets)

This project is a simple Question & Answer system developed in C++ using socket programming.It allows multiple users to register, log in, ask questions, and provide answers. 
All interactions are stored in server text files. The system supports multiple clients simultaneously through multithreading.

---

## Features
✅ **User Registration & Login**: Ensures unique usernames and secure access.

✅ **Question Submission**: Users can post questions, each assigned a unique ID. 

✅ **Answer Submission**: Users can answer existing questions by referencing their IDs.

✅ **File-based Storage**: Questions and answers are stored in questions.txt and answers.txt respectively.

✅ **Multi-Client Support**: Handles multiple client connections concurrently using multithreading.

✅ **Search Questions**: Users can search for questions using specific keywords to quickly locate relevant content.

✅ **Delete Questions/Answers**: Authenticated users can delete their own questions and answers, enabling content management and moderation.


---

## Installation & Setup

✅C++ compiler (e.g., g++)
✅Make utility (Install make)
✅POSIX-compliant system (e.g., Linux, macOS)

### **1. Clone the repository**
```sh
git clone https://github.com/team2-miniproject/qa-system.git
cd qa-system
```

### **2. Compile the Server and client and other utilities**
```sh
make clean
make
```

### **3. Run the Server**
```sh
./server
```

### **4. Run the Client**
```sh
./client
```

---

### Searching Questions
1. From the main menu, choose **"Search Questions"**.
2. Enter a keyword to search.
3. The system will display all matching questions containing that **tokenized keyword**.

### Deleting Questions/Answers
1. Choose the **"Delete"** option from the main menu.
2. Specify whether you want to delete a question or an answer.
3. Provide the relevant **ID**.
4. The system verifies ownership before deletion to prevent unauthorized changes.

### User Registration
1. Launch the client application.
2. Select the **"Register"** option when prompted.
3. Enter a unique username and password.
4. If the username already exists, registration will fail.

### User Login
1. Select the **"Login"** option.
2. Enter your registered username and password.
3. Upon successful authentication, you can access the main menu.

### Asking a Question
1. From the main menu, choose **"Ask a Question"**.
2. Input your question when prompted.
3. The question will be saved with a **unique ID** in `questions.txt`.

### Answering a Question
1. Select **"Answer a Question"** from the main menu.
2. Enter the ID of the question you wish to answer.
3. Provide your answer when prompted.
4. The answer will be stored in `answers.txt` linked to the corresponding **question ID**.

---

## File Format
### **questions.txt**
```
<Question ID> <Username> <Question Text>
```
**Example:**
```
1 alice What is a socket in networking?
2 bob How does multithreading work?
```

### **answers.txt**
```
<Answer ID> <Question ID> <Username> <Answer Text>
```
**Example:**
```
1 1 bob A socket is an endpoint for communication between two machines.
2 2 alice Multithreading allows multiple tasks to run concurrently.
```

### **Other Files**
1. **index.txt** (Stores tokens of a question)
```
<keyword> <QuestionID1> <QuestionID2> ...
```
2. **votes.txt** (Stores vote data)
```
<AnswerID> <Username> <Vote>
```
3. **users.txt** (Stores user Auth)
```
<Username> <Password>
```

---
## Project Structure
```
## Project Structure
qa-system/
├── Makefile
├── client                  # Compiled client executable
├── server                  # Compiled server executable
├── config/
│   └── config.txt          # Configuration settings
├── data/
│   ├── answers.txt         # Stored answers
│   ├── index.txt           # Index for fast lookup
│   ├── questions.txt       # Stored questions
│   ├── users.txt           # Registered users info
│   └── votes.txt           # Votes for Q&A
├── src/                    # Source code
│   ├── answer/
│   │   ├── Answer.cpp
│   │   └── Answer.h
│   ├── databaseManager/
│   │   ├── DatabaseManager.cpp
│   │   └── DatabaseManager.h
│   ├── question/
│   │   ├── Question.cpp
│   │   └── Question.h
│   ├── search/
│   │   ├── SearchEngine.cpp
│   │   └── SearchEngine.h
│   ├── sentimentAnalyzer/
│   │   ├── SentimentAnalyzer.cpp
│   │   └── SentimentAnalyzer.h
│   ├── threadManager/
│   │   ├── ThreadManager.cpp
│   │   └── ThreadManager.h
│   ├── user/
│   │   ├── User.cpp
│   │   └── User.h
│   └── utility/
│   │   ├── Utility.cpp
│   │   └── Utility.h
│   ├── client.cpp
│   ├── server.cpp
│   ├── template.h
└── README.md
```

---

## Future Improvements
- Implement **Search Functionality** for questions.
- Add **Upvote/Downvote System** for answers.
- Store passwords securely using **hashing**.
- Implement **Admin Controls** to moderate content.

---

## Author
Developed by **Group 2** as part of an **Operating Systems** mini project. 
