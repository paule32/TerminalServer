// ---------------------------------------------------------------------------------------
// Copyright(c) 2025 Jens Kallup
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files(the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and /or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions :
// 
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ---------------------------------------------------------------------------------------
// Server.cpp - Konsolenbasierter TCP-Server mit WSAEventSelect

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

struct ClientInfo {
    SOCKET socket;
    std::string ip;
    std::string name;
};

std::map<SOCKET, ClientInfo> clients;
std::ofstream logFile("log.txt", std::ios::app);

void log(const std::string& msg) {
    SYSTEMTIME time;
    GetLocalTime(&time);
    char timestamp[64];
    sprintf_s(timestamp, "[%04d-%02d-%02d %02d:%02d:%02d] ",
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

    logFile << timestamp << msg << std::endl;
    std::cout << timestamp << msg << std::endl;
}

void handleClientMessage(SOCKET clientSock) {
    char buffer[512] = {};
    int recvBytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (recvBytes <= 0) {
        log("Client getrennt.");
        closesocket(clientSock);
        clients.erase(clientSock);
        return;
    }

    std::string message(buffer);
    log("Empfangen: " + message);

    // Erwartetes Format: NAME:xyz
    if (message.find("NAME:") == 0) {
        std::string name = message.substr(5);
        sockaddr_in addr;
        int addrlen = sizeof(addr);
        getpeername(clientSock, (sockaddr*)&addr, &addrlen);
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ipstr, sizeof(ipstr));

        clients[clientSock] = { clientSock, ipstr, name };

        std::ostringstream response;
        if (name.empty() || name.length() > 20) {
            response << "STATUS:1|MESSAGE:Name ungueltig.";
        } else {
            response << "STATUS:0|MESSAGE:Willkommen, " << name << "!";
            log("Client " + std::string(ipstr) + " registriert sich als '" + name + "'");
        }

        send(clientSock, response.str().c_str(), response.str().length(), 0);
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(listenSock, SOMAXCONN);

    WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
    SOCKET sockets[WSA_MAXIMUM_WAIT_EVENTS];
    int eventCount = 0;

    WSAEVENT listenEvent = WSACreateEvent();
    WSAEventSelect(listenSock, listenEvent, FD_ACCEPT);
    events[0] = listenEvent;
    sockets[0] = listenSock;
    eventCount = 1;

    log("Server gestartet auf Port 5555.");

    while (true) {
        int index = WSAWaitForMultipleEvents(eventCount, events, FALSE, WSA_INFINITE, FALSE);
        if (index == WSA_WAIT_FAILED) break;

        index -= WSA_WAIT_EVENT_0;
        WSANETWORKEVENTS netEvents;
        WSAEnumNetworkEvents(sockets[index], events[index], &netEvents);

        if (netEvents.lNetworkEvents & FD_ACCEPT) {
            SOCKET clientSock = accept(listenSock, NULL, NULL);
            if (eventCount < WSA_MAXIMUM_WAIT_EVENTS) {
                WSAEVENT clientEvent = WSACreateEvent();
                WSAEventSelect(clientSock, clientEvent, FD_READ | FD_CLOSE);
                events[eventCount] = clientEvent;
                sockets[eventCount] = clientSock;
                eventCount++;
                log("Neuer Client verbunden.");
            }
        }

        if (netEvents.lNetworkEvents & FD_READ) {
            handleClientMessage(sockets[index]);
        }

        if (netEvents.lNetworkEvents & FD_CLOSE) {
            closesocket(sockets[index]);
            clients.erase(sockets[index]);
            WSACloseEvent(events[index]);

            for (int i = index; i < eventCount - 1; ++i) {
                sockets[i] = sockets[i + 1];
                events[i] = events[i + 1];
            }
            eventCount--;
            log("Client getrennt (FD_CLOSE).");
        }

        WSAResetEvent(events[index]);
    }

    closesocket(listenSock);
    WSACleanup();
    logFile.close();
    return 0;
}
