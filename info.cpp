#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;


#define LEADER_PORT 9909

class Node {
public:
    int port;
    Node* next;

public:
    Node(int port) {   //Constructor to initialize {port,next}

        this->port=port;
        next = nullptr;
    }

    void printServerPorts(Node* serverHead){

        Node* current_ptr = serverHead;

        while(current_ptr!=nullptr){

            ofstream portFile("servers_port_info.txt", ios::app);

            portFile<<current_ptr->port<<" --> "<<endl;
            portFile.close();
            current_ptr=current_ptr->next;
        }
    }
};

class Info {
public:
    Node* serverHead;  // Pointer to the head of the list of servers

    // Default constructor to initialize serverHead to nullptr
    Info(){
        serverHead = nullptr;
    }

    void insertPort(int new_port) {
        Node* newNode = new Node(new_port);
        if (serverHead == nullptr) {
            serverHead = newNode;  // If list is empty, new node becomes the head
        } else {
            Node* temp = serverHead;
            while (temp->next != nullptr) {
                temp = temp->next;  // Traverse to the end of the list
            }
            temp->next = newNode;  // Insert new node at the end of the list
        }
    }

    map<string, int> nodeInfoMap(map<string, int>& followers) {
        followers.insert({"127.0.0.1", 9910});
        followers.insert({"127.0.0.1", 9911});
        followers.insert({"127.0.0.1", 9912});
        followers.insert({"127.0.0.1", 9913});
        return followers;
    }
};