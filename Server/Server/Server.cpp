// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "stdafx.h"
//#include <semaphore>
//#include <Mspthrd.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

using namespace std;

void CleanupWSADLL();

DWORD WINAPI ReceivingThread(LPVOID param);
DWORD WINAPI SendingThread(LPVOID param);

SOCKET acceptSocket;
bool ProcessOver = false;


int main()
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    cout << "\n\nTHIS IS A SERVER!!!\n\n\n";
    
    //
    // Step 1. Set up WSA DLL
    //

    SOCKET serverSocket;
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


    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Send and receive data
    //

    DWORD threadID;
    HANDLE threadHDL[2];
    threadHDL[0] = CreateThread(NULL, 0, ReceivingThread, NULL, 0, &threadID);
    SetThreadPriority(threadHDL[0], THREAD_PRIORITY_ABOVE_NORMAL);
    threadHDL[1] = CreateThread(NULL, 0, SendingThread, NULL, 0, &threadID);
    SetThreadPriority(threadHDL[1], THREAD_PRIORITY_NORMAL);

    //char buffer[200];
    //int byteCount;
    //enum STATUS
    //{
    //    SENDING = 0,
    //    RECEIVING = 1,
    //};

    //STATUS status = RECEIVING;

    //while (strcmp(buffer, "SHUTDOWN") != 0)
    //{
    //    if (status == SENDING)
    //    {
    //        cout << "\nServer: ";
    //        cin.getline(buffer, 200);
    //        byteCount = send(acceptSocket, buffer, 200, 0);
    //        if (byteCount > 0)
    //        {
    //            //cout << "\nMessage sent.\n\n";
    //            status = RECEIVING;
    //        }
    //        else
    //            cout << "\nMessage failed to send.\n\n";
    //    }
    //    else if (status == RECEIVING)
    //    {
    //        byteCount = recv(acceptSocket, buffer, 200, 0);
    //        if (byteCount > 0)
    //        {
    //            cout << "\nClient: " << buffer << "\n";
    //            status = SENDING;
    //        }
    //    }
    //}

    
    if (ProcessOver == true)
    {
        system("pause");
        CleanupWSADLL();
        return 0;
    }
}


void CleanupWSADLL()
{
    // Shut down WSA DLL
    int cleanupError = WSACleanup();
    cout << "\n\n\n\n\n\nCleanup result: " << cleanupError << "\n\n";
}

DWORD __stdcall ReceivingThread(LPVOID param)
{
    char buffer[200];
    int byteCount;
    while (strcmp(buffer, "SHUTDOWN") != 0)
    {
        byteCount = recv(acceptSocket, buffer, 200, 0);
        if (byteCount > 0)
            cout << "\nClient: " << buffer << "\n";
    }
    if (strcmp(buffer, "SHUTDOWN") == 0)
        ProcessOver = true;
    return 0;
}

DWORD __stdcall SendingThread(LPVOID param)
{
    char buffer[200];
    int byteCount;
    while (strcmp(buffer, "SHUTDOWN") != 0)
    {
        cout << "\nEnter your message: ";
        cin.getline(buffer, 200);
        byteCount = send(acceptSocket, buffer, 200, 0);
        if (byteCount < 0)
        {
            cout << "\nMessage failed to send!\n";
        }
    }
    if (strcmp(buffer, "SHUTDOWN") == 0)
        ProcessOver = true;
    return 0;
}
