#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

struct Patient {
    int id;
    string name;
    int age;
    string contact;
    string bloodGroup;
    string reason;
    Patient *left;
    Patient *right;

    Patient(int i, string n, int a, string c, string b, string r)
        : id(i), name(n), age(a), contact(c), bloodGroup(b), reason(r), left(nullptr), right(nullptr) {}
};

struct Doctor {
    int id;
    string name;
    string contact;
    string specialty;
    Doctor *next;

    Doctor(int i, string n, string c, string s) : id(i), name(n), contact(c), specialty(s), next(nullptr) {}
};

struct Action {
    string type;   // add, delete, update
    string target; // patient or doctor
    int id;
    string name;
    int age;
    string contact;
    string bloodGroup;
    string reason;
    string specialty;
};

class HospitalManagementSystem {
private:
    Patient *patientRoot;
    Doctor *doctorHead;
    stack<Action> undoStack;
    const string DATA_FILE = "hospital_data.json";

    Patient *insertIntoBST(Patient *root, int id, string name, int age, string contact, string bloodGroup, string reason) {
        if (!root)
            return new Patient(id, name, age, contact, bloodGroup, reason);
        if (id < root->id)
            root->left = insertIntoBST(root->left, id, name, age, contact, bloodGroup, reason);
        else if (id > root->id)
            root->right = insertIntoBST(root->right, id, name, age, contact, bloodGroup, reason);
        return root;
    }

    Patient *deleteFromBST(Patient *root, int id) {
        if (!root)
            return nullptr;
        if (id < root->id)
            root->left = deleteFromBST(root->left, id);
        else if (id > root->id)
            root->right = deleteFromBST(root->right, id);
        else {
            if (!root->left) {
                Patient *temp = root->right;
                delete root;
                return temp;
            } else if (!root->right) {
                Patient *temp = root->left;
                delete root;
                return temp;
            }
            Patient *successor = root->right;
            while (successor->left)
                successor = successor->left;
            root->id = successor->id;
            root->name = successor->name;
            root->age = successor->age;
            root->contact = successor->contact;
            root->bloodGroup = successor->bloodGroup;
            root->reason = successor->reason;
            root->right = deleteFromBST(root->right, successor->id);
        }
        return root;
    }

    Patient *searchBST(Patient *root, int id) {
        if (!root || root->id == id)
            return root;
        return id < root->id ? searchBST(root->left, id) : searchBST(root->right, id);
    }

    void inOrder(Patient *root, json &patients) {
        if (!root)
            return;
        inOrder(root->left, patients);
        patients.push_back({
            {"id", root->id},
            {"name", root->name},
            {"age", root->age},
            {"contact", root->contact},
            {"bloodGroup", root->bloodGroup},
            {"reason", root->reason}
        });
        inOrder(root->right, patients);
    }

    void saveState() {
        json state;
        // Save patients
        state["patients"] = json::array();
        inOrder(patientRoot, state["patients"]);
        // Save doctors
        state["doctors"] = json::array();
        Doctor *doc = doctorHead;
        while (doc) {
            state["doctors"].push_back({
                {"id", doc->id},
                {"name", doc->name},
                {"contact", doc->contact},
                {"specialty", doc->specialty}
            });
            doc = doc->next;
        }
        // Save to file
        ofstream file(DATA_FILE);
        if (file.is_open()) {
            file << state.dump(4);
            file.close();
        } else {
            cerr << "Error: Could not open " << DATA_FILE << " for writing" << endl;
        }
    }

    void loadState() {
        ifstream file(DATA_FILE);
        if (!file.is_open()) {
            return; // No file exists yet
        }

        json state;
        try {
            file >> state;
            file.close();
        } catch (const json::exception &e) {
            file.close();
            cerr << "Error: Failed to parse JSON from " << DATA_FILE << ": " << e.what() << endl;
            return;
        }

        // Clear existing data
        clearBST(patientRoot);
        patientRoot = nullptr;
        while (doctorHead) {
            Doctor *temp = doctorHead;
            doctorHead = doctorHead->next;
            delete temp;
        }

        // Load patients
        for (const auto &p : state["patients"]) {
            int id = p["id"].get<int>();
            string name = p["name"].get<string>();
            int age = p["age"].get<int>();
            string contact = p["contact"].get<string>();
            string bloodGroup = p["bloodGroup"].get<string>();
            string reason = p["reason"].get<string>();
            patientRoot = insertIntoBST(patientRoot, id, name, age, contact, bloodGroup, reason);
        }

        // Load doctors
        for (const auto &d : state["doctors"]) {
            Doctor *newDoctor = new Doctor(
                d["id"].get<int>(),
                d["name"].get<string>(),
                d["contact"].get<string>(),
                d["specialty"].get<string>()
            );
            newDoctor->next = doctorHead;
            doctorHead = newDoctor;
        }
    }

    void clearBST(Patient *root) {
        if (!root) return;
        clearBST(root->left);
        clearBST(root->right);
        delete root;
    }

public:
    HospitalManagementSystem() : patientRoot(nullptr), doctorHead(nullptr) {
        loadState();
    }

    ~HospitalManagementSystem() {
        saveState();
        clearBST(patientRoot);
        while (doctorHead) {
            Doctor *temp = doctorHead;
            doctorHead = doctorHead->next;
            delete temp;
        }
    }

    json addPatient(int id, string name, int age, string contact, string bloodGroup, string reason) {
        patientRoot = insertIntoBST(patientRoot, id, name, age, contact, bloodGroup, reason);
        undoStack.push({"add", "patient", id, name, age, contact, bloodGroup, reason, ""});
        saveState();
        return {{"message", "Patient Added: " + name}};
    }

    json viewPatients() {
        json patients = json::array();
        inOrder(patientRoot, patients);
        return patients;
    }

    json viewSortedPatients() {
        json patients = json::array();
        inOrder(patientRoot, patients);
        return patients;
    }

    json updatePatient(int id, string newName, int newAge) {
        Patient *found = searchBST(patientRoot, id);
        if (found) {
            undoStack.push({"update", "patient", found->id, found->name, found->age, found->contact, found->bloodGroup, found->reason, ""});
            found->name = newName;
            found->age = newAge;
            saveState();
            return {{"message", "Patient ID " + to_string(id) + " updated"}};
        }
        return {{"error", "Patient ID not found"}};
    }

    json deletePatient(int id) {
        Patient *found = searchBST(patientRoot, id);
        if (!found) {
            return {{"error", "Patient ID not found for deletion"}};
        }
        undoStack.push({"delete", "patient", found->id, found->name, found->age, found->contact, found->bloodGroup, found->reason, ""});
        patientRoot = deleteFromBST(patientRoot, id);
        saveState();
        return {{"message", "Patient Deleted"}};
    }

    json addDoctor(int id, string name, string contact, string specialty) {
        Doctor *newDoctor = new Doctor(id, name, contact, specialty);
        newDoctor->next = doctorHead;
        doctorHead = newDoctor;
        undoStack.push({"add", "doctor", id, name, 0, contact, "", "", specialty});
        saveState();
        return {{"message", "Doctor Added: " + name + " (" + specialty + ")"}};
    }

    json viewDoctors() {
        json doctors = json::array();
        Doctor *temp = doctorHead;
        while (temp) {
            doctors.push_back({
                {"id", temp->id},
                {"name", temp->name},
                {"contact", temp->contact},
                {"specialty", temp->specialty}
            });
            temp = temp->next;
        }
        return doctors;
    }

    json recommendDoctor(string reason) {
        json doctors = json::array();
        Doctor *temp = doctorHead;
        bool found = false;
        transform(reason.begin(), reason.end(), reason.begin(), ::tolower);

        while (temp) {
            string specialty = temp->specialty;
            transform(specialty.begin(), specialty.end(), specialty.begin(), ::tolower);
            if (specialty.find(reason) != string::npos || reason.find(specialty) != string::npos) {
                doctors.push_back({
                    {"id", temp->id},
                    {"name", temp->name},
                    {"contact", temp->contact},
                    {"specialty", temp->specialty}
                });
                found = true;
            }
            temp = temp->next;
        }
        if (!found) {
            return {{"error", "No doctor found for the specified reason"}};
        }
        return doctors;
    }

    json deleteDoctor(int id) {
        Doctor *curr = doctorHead;
        Doctor *prev = nullptr;
        while (curr) {
            if (curr->id == id) {
                undoStack.push({"delete", "doctor", curr->id, curr->name, 0, curr->contact, "", "", curr->specialty});
                if (prev)
                    prev->next = curr->next;
                else
                    doctorHead = curr->next;
                delete curr;
                saveState();
                return {{"message", "Doctor Deleted"}};
            }
            prev = curr;
            curr = curr->next;
        }
        return {{"error", "Doctor ID not found"}};
    }

    json undo() {
        if (undoStack.empty()) {
            return {{"error", "No actions to undo"}};
        }

        Action last = undoStack.top();
        undoStack.pop();

        if (last.type == "add" && last.target == "patient") {
            json result = deletePatient(last.id);
            saveState();
            return result;
        } else if (last.type == "delete" && last.target == "patient") {
            json result = addPatient(last.id, last.name, last.age, last.contact, last.bloodGroup, last.reason);
            saveState();
            return result;
        } else if (last.type == "update" && last.target == "patient") {
            json result = updatePatient(last.id, last.name, last.age);
            saveState();
            return result;
        } else if (last.type == "add" && last.target == "doctor") {
            json result = deleteDoctor(last.id);
            saveState();
            return result;
        } else if (last.type == "delete" && last.target == "doctor") {
            json result = addDoctor(last.id, last.name, last.contact, last.specialty);
            saveState();
            return result;
        }
        return {{"error", "Invalid undo action"}};
    }
};

int main(int argc, char *argv[]) {
    HospitalManagementSystem hms;
    json result;

    if (argc < 2) {
        result = {{"error", "No command provided"}};
        cout << result.dump() << endl;
        return 1;
    }

    string command = argv[1];

    try {
        if (command == "add_patient" && argc == 8) {
            int id = stoi(argv[2]);
            string name = argv[3];
            int age = stoi(argv[4]);
            string contact = argv[5];
            string bloodGroup = argv[6];
            string reason = argv[7];
            result = hms.addPatient(id, name, age, contact, bloodGroup, reason);
        } else if (command == "view_patients" && argc == 2) {
            result = hms.viewPatients();
        } else if (command == "view_sorted_patients" && argc == 2) {
            result = hms.viewSortedPatients();
        } else if (command == "update_patient" && argc == 5) {
            int id = stoi(argv[2]);
            string newName = argv[3];
            int newAge = stoi(argv[4]);
            result = hms.updatePatient(id, newName, newAge);
        } else if (command == "delete_patient" && argc == 3) {
            int id = stoi(argv[2]);
            result = hms.deletePatient(id);
        } else if (command == "add_doctor" && argc == 6) {
            int id = stoi(argv[2]);
            string name = argv[3];
            string contact = argv[4];
            string specialty = argv[5];
            result = hms.addDoctor(id, name, contact, specialty);
        } else if (command == "view_doctors" && argc == 2) {
            result = hms.viewDoctors();
        } else if (command == "recommend_doctor" && argc == 3) {
            string reason = argv[2];
            result = hms.recommendDoctor(reason);
        } else if (command == "delete_doctor" && argc == 3) {
            int id = stoi(argv[2]);
            result = hms.deleteDoctor(id);
        } else if (command == "undo" && argc == 2) {
            result = hms.undo();
        } else {
            result = {{"error", "Invalid command or arguments"}};
        }
    } catch (const exception &e) {
        result = {{"error", "Exception: " + string(e.what())}};
    }

    cout << result.dump() << endl;
    return 0;
}