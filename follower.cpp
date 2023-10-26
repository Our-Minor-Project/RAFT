#include <iostream>
#include <winsock2.h>
#include <bits/stdc++.h>
#define PORT 9910
using namespace std;

struct sockaddr_in srv;

fd_set fr, fw, fe; //number of socket descriptors on which we are going to read , write or see any exceptions
                   //at max 64 file descriptors in the fd_set
                   //like [*ptr1,*ptr2,.......,*ptrn] , each of these pointers points to a particular kernel location having file/socket object

int nMaxFD;
#define ACK_PORT 9919
#define LEADER_IP "127.0.0.1"
int current_term = 0;
int main() {

    int nRet = 0;
    //Initialize the Socket
    WSADATA ws;

    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << "WSA Failed";
        exit(EXIT_FAILURE);
    } else {
        cout << "WSA success";
    }

    SOCKET nsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (nsocket == INVALID_SOCKET) {
        cout << "Socket creation failed." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    //Initialize the environment for sockaddr structure

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;

    memset(&(srv.sin_zero), 0, 8);

    //Bind the socket to the local port


    nRet = bind(nsocket, (sockaddr *) &srv, sizeof(sockaddr));
    //check the nRet value
    if (nRet < 0) {
        cout << endl << "failed to bind to  local port";
        closesocket(nsocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    } else {
        cout << endl << "Bind success to the local port";
    }


    //Listen the request from the client (request queue)

    nRet = listen(nsocket, 5); //backlog => maximum 5 request at a time, rest in queue

    if (nRet < 0) {

        closesocket(nsocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    } else {
        cout << endl << "Listening to local port";
    }

    nMaxFD = nsocket;

    struct timeval tv{};
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    //keep on listening

    while (true) {

        FD_ZERO(&fr);
        FD_SET(nsocket, &fr);

        nRet = select(nMaxFD + 1, &fr, nullptr, nullptr, &tv);

        if (nRet < 0) {
            cout << "Select error." << endl;
            closesocket(nsocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(nsocket, &fr)) {
            struct sockaddr_in clientAddr{};
            int addrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(nsocket, (struct sockaddr *) &clientAddr, &addrLen);

            if (clientSocket == INVALID_SOCKET) {
                cout << "Accept error." << endl;
            } else {
                char buffer[1024] = {0};
                int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesReceived > 0) {

                    cout << "Received message: " << endl;
                    cout << buffer << endl;


                    string received_data(buffer); //char array of size buffer

                    string term_in_str;

                    size_t i = 0;

                    // Loop through the message until reaching the delimiter '|'
                    while(i < received_data.size() && received_data[i] != '|') {
                        if(isdigit(received_data[i])) {
                            term_in_str += received_data[i];
                        }
                        i++;                //loop through the received message , and find the term digit
                    }

//                    cout<<received_data<<endl;

                    if(!term_in_str.empty()) {
                        int message_term_number = stoi(term_in_str);

                        // Check the term number
                        if(message_term_number > current_term) {
                            // Update the current term number
                            current_term = message_term_number;

                            //we can add further processing here

                            // Send acknowledgment back to the leader
                            const char *ack_message = "acknowledged from follower";


                            SOCKET ackSocket = socket(AF_INET, SOCK_STREAM, 0);

                            sockaddr_in leaderAckAddr{};
                            leaderAckAddr.sin_family = AF_INET;
                            leaderAckAddr.sin_port = htons(
                                    ACK_PORT);
                            leaderAckAddr.sin_addr.s_addr = inet_addr(
                                    LEADER_IP);

                            if (connect(ackSocket, (struct sockaddr *) &leaderAckAddr, sizeof(leaderAckAddr)) == 0) {
                                send(ackSocket, ack_message, strlen(ack_message), 0);
                            }
                            else{
                                cout<<"ACK NOT SENT"<<endl;
                            }
                        }

                }
                closesocket(clientSocket);
            }
        }
    }
}
}



