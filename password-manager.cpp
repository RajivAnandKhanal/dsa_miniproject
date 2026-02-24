#include<iostream>
#include <fstream> // this header is to provide classes for file input and output
#include <cstring>
#include <cstdlib>
using namespace std;

#define WEBSITE 100
#define USERNAME 18
#define PASSWORD 27
#define TABLE 100

// this function is linked-ljist-node
struct Node{
char website[WEBSITE];
char username[USERNAME];
char password[PASSWORD];
Node* next; // pointer to new node for this linkedlist

// constructor to initilize object with some default value
Node(const char* site, const char* user,const char* pass){
strcpy(website,site);
strcpy(username,user);
strcpy(password,pass);
next = NULL;
}};

// hash functin 
unsigned int hash(const char* str)
{
        unsigned int hash = 0;

        for (int i = 0; str[i] !='\0'; i++)
        {
                hash = hash * 31 + str[i];
        }

        return hash % TABLE;
}
