//
// Created by saahi on 10/24/2023.
//
#include <iostream>
#include <winsock2.h>
//#include "main.cpp"
#include <bits/stdc++.h>
#define SERVER_IP "127.0.0.1"
#define PORT 9909
using namespace std;

int main() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << "WSA Failed" << endl;
        exit(EXIT_FAILURE);
    } else {
        cout << "WSA success" << endl;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Socket creation failed." << endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in srvAddr{};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(PORT);
    srvAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(clientSocket, (struct sockaddr*)&srvAddr, sizeof(srvAddr)) < 0) {
        cout << "Connection to server failed." << endl;
        closesocket(clientSocket);
        exit(EXIT_FAILURE);
    }

    const char* message = "Hello Server!";
    send(clientSocket, message, strlen(message), 0);

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
