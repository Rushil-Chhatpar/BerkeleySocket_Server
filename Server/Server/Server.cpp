// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

using namespace std;

void CleanupWSADLL();

int main()
{
    cout << "\n\nTHIS IS A SERVER!!!\n\n\n";
    
    //
    // Step 1. Set up WSA DLL
    //

    SOCKET serverSocket, acceptSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaError;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaError = WSAStartup(wVersionRequested, &wsaData);
    if (wsaError != 0)
    {
        cout << "The Winsock dll not found :(\n";
        return 0;
    }
    else
    {
        cout << "The Winsock dll found :)\n";
        cout << "Status: " << wsaData.szSystemStatus << "\n\n";
    }


    //
    // Step 2. Set up a socket
    //

    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Error creating socket: " << WSAGetLastError() << "\n";
        CleanupWSADLL();
        return 0;
    }
    else
    {
        cout << "Socket created successfully\n\n";
    }

    
    //
    // Step 3. Bind the socket
    //

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr); // OR &service.sin_addr.S_un.S_addr
    // Loopback address. https://www.geeksforgeeks.org/what-is-a-loopback-address/
    service.sin_port = htons(port);

    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        cout << "Server bind failed: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        CleanupWSADLL();
        return 0;
    }
    else
    {
        cout << "Socket bind successful!\n\n";
    }


    //
    // Step 4. Listen on the socket
    // 
    
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        cout << "Server listen failed: " << WSAGetLastError() << "\n";
    }
    else
    {
        cout << "Server listening...\n\n";
    }


    //
    // Step 5. Accept a connection on the socket
    //

    acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET)
    {
        cout << "Accept connection failed: " << WSAGetLastError() << "\n";
        CleanupWSADLL();
        return 0;
    }

    cout << "Accepted connection\n\n";
    system("pause");


    CleanupWSADLL();

    return 0;
}


void CleanupWSADLL()
{
    // Shut down WSA DLL
    int cleanupError = WSACleanup();
    cout << "\n\n\n\n\n\nCleanup result: " << cleanupError << "\n\n";
}