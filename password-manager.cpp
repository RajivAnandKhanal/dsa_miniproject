#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

#define WEBSITE 50
#define USERNAME 18
#define PASSWORD 27
#define TABLE 100

// basic hash function
unsigned int Hash(const char* str) {
    unsigned int hash = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hash = hash * 31 + str[i];
    }
    return hash % TABLE;
}

// linked list node creation
struct Node {
    char website[WEBSITE];
    char username[USERNAME];
    char password[PASSWORD];
    Node* next;
    
    Node(const char* site, const char* user, const char* pass) {
        strcpy(website, site);
        strcpy(username, user);
        strcpy(password, pass);
        next = NULL;
    }
};

// STACK 
struct StackNode {
    char operation[4];  // "ADD", "DEL", "UPD"
    char website[WEBSITE];
    char username[USERNAME];
    char password[PASSWORD];
    StackNode* next;
    
    StackNode(const char* op, const char* site, const char* user, const char* pass) {
        strcpy(operation, op);
        strcpy(website, site);
        strcpy(username, user);
        strcpy(password, pass);
        next = NULL;
    }
};

class Stack {
private:
    StackNode* top;
    
public:
    Stack() {
        top = NULL;
    }
    
    void push(const char* op, const char* site, const char* user, const char* pass) {
        StackNode* newNode = new StackNode(op, site, user, pass);
        newNode->next = top;
        top = newNode;
    }
    
    StackNode* pop() {
        if (isEmpty()) return NULL;
        StackNode* temp = top;
        top = top->next;
        return temp;
    }
    
    bool isEmpty() {
        return top == NULL;
    }
    
    void clear() {
        while (!isEmpty()) {
            StackNode* temp = pop();
            delete temp;
        }
    }
};

// ==================== HASH MAP FOR FAST LOOKUP ====================
class HashMap {
private:
    Node* table[TABLE];
    
public:
    HashMap() {
        for (int i = 0; i < TABLE; i++) {
            table[i] = NULL;
        }
    }
    
    void insert(Node* node) {
        int index = Hash(node->website);
        // We're just storing reference, not creating new node
        // The actual node is managed by linked list
        table[index] = node;  // Simple chaining not implemented for simplicity
    }
    
    Node* find(const char* website) {
        int index = Hash(website);
        return table[index];
    }
    
    void remove(const char* website) {
        int index = Hash(website);
        table[index] = NULL;
    }
    
    void update(const char* website, Node* node) {
        int index = Hash(website);
        table[index] = node;
    }
};

// ==================== MAIN CREDENTIAL MANAGER ====================
class CredentialManager {
private:
    Node* head;              // Linked List head
    Stack undoStack;          // Stack for undo
    HashMap hashMap;          // HashMap for quick lookup
    
    char masterUser[50];
    char masterPass[50];
    bool isAuthenticated;
    
public:
    CredentialManager() {
        head = NULL;
        isAuthenticated = false;
    }
    
    ~CredentialManager() {
        // Clean up linked list
        while (head != NULL) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        undoStack.clear();
    }
    
    // ========== AUTHENTICATION ==========
    bool setupMasterPassword() {
        cout << "\n=== FIRST TIME SETUP ===\n";
        cout << "Create Username: ";
        cin.getline(masterUser, 50);
        
        char pass1[50], pass2[50];
        cout << "Create Master Password: ";
        cin.getline(pass1, 50);
        cout << "Confirm Master Password: ";
        cin.getline(pass2, 50);
        
        if (strcmp(pass1, pass2) == 0) {
            strcpy(masterPass, pass1);
            cout << "\nMaster password created successfully!\n";
            return true;
        } else {
            cout << "\nPasswords do not match!\n";
            return false;
        }
    }
    
    bool login() {
        cout << "\n=== LOGIN ===\n";
        char user[50], pass[50];
        
        cout << "Username: ";
        cin.getline(user, 50);
        cout << "Master Password: ";
        cin.getline(pass, 50);
        
        if (strcmp(user, masterUser) == 0 && strcmp(pass, masterPass) == 0) {
            isAuthenticated = true;
            cout << "Login successful!\n";
            loadFromFile();
            return true;
        } else {
            cout << "Invalid credentials!\n";
            return false;
        }
    }
    
    // ========== FILE OPERATIONS ==========
    void saveToFile() {
        ofstream file("vault.dat", ios::binary);
        if (!file) {
            cout << "Error saving file!\n";
            return;
        }
        
        // Save master credentials
        file.write(masterUser, 50);
        file.write(masterPass, 50);
        
        // Count nodes
        int count = 0;
        Node* temp = head;
        while (temp) {
            count++;
            temp = temp->next;
        }
        
        // Save count
        file.write((char*)&count, sizeof(count));
        
        // Save all credentials
        temp = head;
        while (temp) {
            file.write(temp->website, WEBSITE);
            file.write(temp->username, USERNAME);
            file.write(temp->password, PASSWORD);
            temp = temp->next;
        }
        
        file.close();
        cout << "Data saved to vault.dat\n";
    }
    
    void loadFromFile() {
        ifstream file("vault.dat", ios::binary);
        if (!file) {
            return; // File doesn't exist yet
        }
        
        // Clear existing data
        while (head != NULL) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        
        // Load master credentials (skip for now)
        char tempUser[50], tempPass[50];
        file.read(tempUser, 50);
        file.read(tempPass, 50);
        
        // Load count
        int count;
        file.read((char*)&count, sizeof(count));
        
        // Load all credentials
        for (int i = 0; i < count; i++) {
            char site[WEBSITE], user[USERNAME], pass[PASSWORD];
            file.read(site, WEBSITE);
            file.read(user, USERNAME);
            file.read(pass, PASSWORD);
            
            // Add to linked list and hash map
            Node* newNode = new Node(site, user, pass);
            newNode->next = head;
            head = newNode;
            hashMap.insert(newNode);
        }
        
        file.close();
    }
    
    // ========== CREDENTIAL OPERATIONS ==========
    void addCredential() {
        if (!isAuthenticated) return;
        
        char site[WEBSITE], user[USERNAME], pass[PASSWORD];
        
        cout << "\n=== ADD CREDENTIAL ===\n";
        cout << "Website: ";
        cin.getline(site, WEBSITE);
        
        // Check if exists
        if (hashMap.find(site) != NULL) {
            cout << "Website already exists!\n";
            return;
        }
        
        cout << "Username: ";
        cin.getline(user, USERNAME);
        cout << "Password: ";
        cin.getline(pass, PASSWORD);
        
        // Create new node
        Node* newNode = new Node(site, user, pass);
        
        // Add to linked list (at beginning)
        newNode->next = head;
        head = newNode;
        
        // Add to hash map
        hashMap.insert(newNode);
        
        // Add to undo stack
        undoStack.push("ADD", site, user, pass);
        
        // Save to file
        saveToFile();
        
        cout << "Credential added successfully!\n";
    }
    
    void viewCredential() {
        if (!isAuthenticated) return;
        
        char site[WEBSITE];
        
        cout << "\n=== VIEW CREDENTIAL ===\n";
        cout << "Website: ";
        cin.getline(site, WEBSITE);
        
        Node* node = hashMap.find(site);
        if (node) {
            cout << "\nWebsite: " << node->website << endl;
            cout << "Username: " << node->username << endl;
            cout << "Password: " << node->password << endl;
        } else {
            cout << "Website not found!\n";
        }
    }
    
    void updateCredential() {
        if (!isAuthenticated) return;
        
        char site[WEBSITE];
        
        cout << "\n=== UPDATE CREDENTIAL ===\n";
        cout << "Website: ";
        cin.getline(site, WEBSITE);
        
        Node* node = hashMap.find(site);
        if (!node) {
            cout << "Website not found!\n";
            return;
        }
        
        // Save old values for undo
        char oldUser[USERNAME], oldPass[PASSWORD];
        strcpy(oldUser, node->username);
        strcpy(oldPass, node->password);
        
        char newUser[USERNAME], newPass[PASSWORD];
        
        cout << "New Username (Enter to keep '" << node->username << "'): ";
        cin.getline(newUser, USERNAME);
        if (strlen(newUser) > 0) {
            strcpy(node->username, newUser);
        }
        
        cout << "New Password (Enter to keep current): ";
        cin.getline(newPass, PASSWORD);
        if (strlen(newPass) > 0) {
            strcpy(node->password, newPass);
        }
        
        // Add to undo stack
        undoStack.push("UPD", site, oldUser, oldPass);
        
        // Update hash map (re-insert)
        hashMap.update(site, node);
        
        // Save to file
        saveToFile();
        
        cout << "Credential updated successfully!\n";
    }
    
    void deleteCredential() {
        if (!isAuthenticated) return;
        
        char site[WEBSITE];
        
        cout << "\n=== DELETE CREDENTIAL ===\n";
        cout << "Website: ";
        cin.getline(site, WEBSITE);
        
        Node* node = hashMap.find(site);
        if (!node) {
            cout << "Website not found!\n";
            return;
        }
        
        // Save for undo
        undoStack.push("DEL", site, node->username, node->password);
        
        // Remove from hash map
        hashMap.remove(site);
        
        // Remove from linked list
        if (head == node) {
            head = head->next;
        } else {
            Node* temp = head;
            while (temp->next != node) {
                temp = temp->next;
            }
            temp->next = node->next;
        }
        
        delete node;
        
        // Save to file
        saveToFile();
        
        cout << "Credential deleted successfully!\n";
    }
    
    void listAllCredentials() {
        if (!isAuthenticated) return;
        
        cout << "\n=== ALL CREDENTIALS ===\n";
        
        if (head == NULL) {
            cout << "No credentials stored.\n";
            return;
        }
        
        Node* temp = head;
        int count = 1;
        while (temp != NULL) {
            cout << count++ << ". " << temp->website 
                 << " (Username: " << temp->username << ")\n";
            temp = temp->next;
        }
    }
    
    void undo() {
        if (!isAuthenticated) return;
        
        if (undoStack.isEmpty()) {
            cout << "Nothing to undo!\n";
            return;
        }
        
        StackNode* op = undoStack.pop();
        
        if (strcmp(op->operation, "ADD") == 0) {
            // Undo add - delete the credential
            Node* node = hashMap.find(op->website);
            if (node) {
                hashMap.remove(op->website);
                
                if (head == node) {
                    head = head->next;
                } else {
                    Node* temp = head;
                    while (temp->next != node) {
                        temp = temp->next;
                    }
                    temp->next = node->next;
                }
                delete node;
                cout << "Undid ADD operation for " << op->website << endl;
            }
            
        } else if (strcmp(op->operation, "DEL") == 0) {
            // Undo delete - add back
            Node* newNode = new Node(op->website, op->username, op->password);
            newNode->next = head;
            head = newNode;
            hashMap.insert(newNode);
            cout << "Undid DELETE operation for " << op->website << endl;
            
        } else if (strcmp(op->operation, "UPD") == 0) {
            // Undo update - restore old values
            Node* node = hashMap.find(op->website);
            if (node) {
                strcpy(node->username, op->username);
                strcpy(node->password, op->password);
                cout << "Undid UPDATE operation for " << op->website << endl;
            }
        }
        
        delete op;
        saveToFile();
    }
    
    void searchByWebsite() {
        if (!isAuthenticated) return;
        
        char searchTerm[WEBSITE];
        
        cout << "\n=== SEARCH CREDENTIALS ===\n";
        cout << "Enter website name (or part of it): ";
        cin.getline(searchTerm, WEBSITE);
        
        Node* temp = head;
        bool found = false;
        int count = 0;
        
        while (temp != NULL) {
            if (strstr(temp->website, searchTerm) != NULL) {
                if (!found) {
                    cout << "\nMatching websites:\n";
                    found = true;
                }
                cout << ++count << ". " << temp->website 
                     << " (Username: " << temp->username << ")\n";
            }
            temp = temp->next;
        }
        
        if (!found) {
            cout << "No matching websites found.\n";
        }
    }
};

// ==================== MAIN FUNCTION ====================
int main() {
    CredentialManager manager;
    
    cout << "================================\n";
    cout << "   CREDENTIAL MANAGEMENT SYSTEM\n";
    cout << "================================\n";
    
    // Check if first time setup needed
    ifstream file("vault.dat");
    bool isFirstTime = !file;
    file.close();
    
    if (isFirstTime) {
        cout << "First time setup required.\n";
        if (!manager.setupMasterPassword()) {
            cout << "Setup failed. Exiting...\n";
            return 1;
        }
    }
    
    // Login
    int attempts = 0;
    while (!manager.login() && attempts < 3) {
        attempts++;
        if (attempts < 3) {
            cout << "Attempts remaining: " << 3 - attempts << endl;
        }
    }
    
    if (attempts >= 3) {
        cout << "Too many failed attempts. Exiting...\n";
        return 1;
    }
    
    // Main menu
    int choice;
    do {
        cout << "\n==================== MENU ====================\n";
        cout << "1. Add Credential\n";
        cout << "2. View Credential\n";
        cout << "3. Update Credential\n";
        cout << "4. Delete Credential\n";
        cout << "5. List All Credentials\n";
        cout << "6. Search by Website\n";
        cout << "7. Undo Last Operation\n";
        cout << "8. Exit\n";
        cout << "==============================================\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline
        
        switch(choice) {
            case 1:
                manager.addCredential();
                break;
            case 2:
                manager.viewCredential();
                break;
            case 3:
                manager.updateCredential();
                break;
            case 4:
                manager.deleteCredential();
                break;
            case 5:
                manager.listAllCredentials();
                break;
            case 6:
                manager.searchByWebsite();
                break;
            case 7:
                manager.undo();
                break;
            case 8:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 8);
    
    return 0;
}
