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

