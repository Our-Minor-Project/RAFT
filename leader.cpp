//
// Created by saahi on 10/24/2023.
//
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <cstring>
#include "info.cpp"
#define LEADER_PORT 9909
using namespace std;

struct ServerInfo {
    string ip;
    int port;
};

Info follower_information;

map<string,int> followers;

map<string,int> follower_map = follower_information.nodeInfoMap(followers);


void broadcastMessage(const char* message) {

    for (const auto& server : follower_map) {

        SOCKET outSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in outAddr{};
        outAddr.sin_family = AF_INET;
        outAddr.sin_port = htons(server.second);
        outAddr.sin_addr.s_addr = inet_addr(server.first.c_str());

        if (connect(outSocket, (struct sockaddr*)&outAddr, sizeof(outAddr)) == 0) {
            send(outSocket, message, strlen(message), 0);
            closesocket(outSocket);
        }
    }
}

int main() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2,2),&ws) != 0) {
        cout << "WSA Failed" << endl;
        return 1;
    }

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

    while (true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            cout << "Received message: " << buffer << endl;
            // Broadcasting the message to other servers
            broadcastMessage(buffer);
        }
        closesocket(clientSocket);
    }

}
