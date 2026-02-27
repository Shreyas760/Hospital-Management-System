🏥 Hospital Management System
A full-stack Hospital Management System built using C++ (Data Structures) integrated with a Node.js + Express backend and a simple web frontend.
This project demonstrates how core data structures can be connected to a modern web interface.

🚀 Features
👨‍⚕️ Patient Management (BST Based)
Add Patient
View All Patients
Delete Patient
Update Patient
Sorted View (Inorder Traversal)
🩺 Doctor Management (Linked List Based)
Add Doctor
View Doctors
Delete Doctor
Recommend Doctor (based on patient reason)
🔄 Undo Feature
Undo last operation
Works for:
Add
Delete
Update
💾 Data Persistence
Data stored in JSON file
Automatically loads previous state on restart

🏗️ Architecture
Frontend (HTML)
        ↓
Node.js (Express Server)
        ↓
C++ Executable
        ↓
JSON File (Storage)

Flow Explanation
User performs action from browser.
Frontend sends request to Node.js server.
Node server runs C++ executable using child_process.
C++ processes logic using:
Binary Search Tree (Patients)
Linked List (Doctors)
Stack (Undo)
Result is returned as JSON.
Frontend displays response.

🧠 Data Structures Used
📌 Binary Search Tree (BST)
Used for storing patients.
Efficient search: O(log n)
Sorted storage by ID
Inorder traversal for sorted view
📌 Linked List
Used for storing doctors.
Dynamic memory allocation
Easy insertion and deletion
📌 Stack
Used for undo functionality.

🛠️ Technologies Used
C++
Object Oriented Programming
Binary Search Tree
Linked List
Stack
File Handling
JSON (nlohmann/json)
Node.js
Express.js
HTML
TailwindCSS

⚙️ How to Run the Project
1️⃣ Clone Repository
git clone https://github.com/YOUR_USERNAME/Hospital-Management-System.git
cd Hospital-Management-System
2️⃣ Install Node Dependencies
npm install
3️⃣ Compile C++ Backend
🖥 Windows
g++ backend.cpp -o hms.exe
🐧 Linux / Mac
g++ backend.cpp -o hms
If using Linux/Mac:
Update executable name inside server.js
4️⃣ Start Server
node server.js

📂 Project Structure
Hospital-Management-System/
│
├── backend.cpp        # C++ core logic
├── server.js          # Node.js backend
├── index.html         # Frontend UI
├── package.json
├── .gitignore
└── README.md

👨‍💻 Author
Shreyas Kudale

⭐ If You Like This Project
Give it a star ⭐ on GitHub!
