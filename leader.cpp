//
// Created by saahi on 10/24/2023.
//
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <cstring>
#include "info.cpp"
#include <fstream>
#include <thread>
#include <chrono>

using namespace std;

#define LEADER_PORT 9909
#define ACK_PORT 9919


struct ServerInfo {
    string ip;
    int port;
};

Info follower_information;

map<string,int> followers;

map<string,int> follower_map = follower_information.nodeInfoMap(followers);

map<string, int> heartbeat_map = follower_information.nodeHeartbeatMap(followers);

int term_number = 0;

bool keepRunning = true;

void listenForAcks() {

    SOCKET ackListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ackListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed for ack listener." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    sockaddr_in ackSrvAddr{};
    ackSrvAddr.sin_family = AF_INET;
    ackSrvAddr.sin_port = htons(ACK_PORT);
    ackSrvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(ackListenSocket, (struct sockaddr*)&ackSrvAddr, sizeof(ackSrvAddr)) != 0) {
        cout << "Binding failed for ack listener." << endl;
        closesocket(ackListenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(ackListenSocket, 5) != 0) {
        cout << "Listen failed for ack listener." << endl;
        closesocket(ackListenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    cout << "Server is listening for acknowledgements on port " << ACK_PORT << endl;

    while (keepRunning) {
        SOCKET ackClientSocket = accept(ackListenSocket, nullptr, nullptr);
        char buffer[1024] = {0};
        int bytesReceived = recv(ackClientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            cout << "Received acknowledgement: " << buffer << endl;
        }
        closesocket(ackClientSocket);
    }
}

void broadcastHeatBeat(const char* message, int index) {

    for(const auto& server: heartbeat_map){

        SOCKET outSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in outAddr{};
        outAddr.sin_family = AF_INET;
        outAddr.sin_port = htons(server.second);
        outAddr.sin_addr.s_addr = inet_addr(server.first.c_str());

        if(connect(outSocket, (struct sockaddr*)&outAddr, sizeof(outAddr)) == 0){

            string message_with_index = to_string(index) + "|" + message;
            send(outSocket, message_with_index.c_str(), message_with_index.length(), 0);

            closesocket(outSocket);
        }
    }
}

void broadcastMessage(const char* message, int term) {

    for (const auto& server : follower_map) {

        SOCKET outSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in outAddr{};
        outAddr.sin_family = AF_INET;
        outAddr.sin_port = htons(server.second);
        outAddr.sin_addr.s_addr = inet_addr(server.first.c_str());

        if (connect(outSocket, (struct sockaddr*)&outAddr, sizeof(outAddr)) == 0) {

            string message_with_term = to_string(term) + "|" + message;
            send(outSocket, message_with_term.c_str(), message_with_term.length(), 0);

            closesocket(outSocket);
        }
    }
}


void sendHeartbeats() {
    while (true) {
        string heartbeatMessage = "heartbeat|" + to_string(term_number);
        broadcastHeatBeat(heartbeatMessage.c_str(), term_number);
        this_thread::sleep_for(chrono::seconds(5)); // Send heartbeat every 5 seconds
    }
}


int main() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2,2),&ws) != 0) {
        cout << "WSA Failed" << endl;
        return 1;
    }

    thread ackThread(listenForAcks);  // Start a new thread to listen for acknowledgements

    sendHeartbeats(); // Start a new thread to send heartbeats

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in srvAddr{};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(LEADER_PORT);
    srvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (struct sockaddr*)&srvAddr, sizeof(srvAddr)) != 0) {
        cout << "Binding failed." << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }


    if (listen(listenSocket, 5) != 0) {
        cout << "Listen failed." << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }


    cout << "Server is running and listening on port " << LEADER_PORT << endl;

    while (keepRunning) {

//        SOCKET followerSocket = acce
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);


        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {

            ofstream logfile("leader_logs.txt", ios::app);

            logfile<<term_number<< "|" << buffer << endl;

            logfile.close();

            term_number++;

            cout << "Received message: " << buffer << endl;
            // Broadcasting the message to other followers
            broadcastMessage(buffer,term_number);
        }

        if (strcmp(buffer, "terminate") == 0) {
            keepRunning = false;
        }

    }
    closesocket(listenSocket);
      // This line will block the main thread if there's no way to exit the while loop in sendHeartbeats
    ackThread.join();  // This line will block the main thread if there's no way to exit the while loop in listenForAcks

}
