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

// Client.cpp – Non-blocking Konsolen-Client, sendet NAME an Server

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // non-blocking mode aktivieren
    u_long mode = 1;
    ioctlsocket(clientSock, FIONBIO, &mode);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    int result = connect(clientSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
        std::cerr << "Verbindung fehlgeschlagen.\n";
        closesocket(clientSock);
        WSACleanup();
        return 1;
    }

    std::cout << "Bitte Namen eingeben: ";
    std::string name;
    std::getline(std::cin, name);

    std::string message = "NAME:" + name;
    int sent = send(clientSock, message.c_str(), message.length(), 0);
    if (sent == SOCKET_ERROR) {
        std::cerr << "Sendefehler.\n";
        closesocket(clientSock);
        WSACleanup();
        return 1;
    }

    char buffer[512] = {};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(clientSock, &readfds);

    timeval timeout{};
    timeout.tv_sec = 5; // max. 5 Sekunden warten
    timeout.tv_usec = 0;

    int ready = select(0, &readfds, nullptr, nullptr, &timeout);
    if (ready > 0 && FD_ISSET(clientSock, &readfds)) {
        int recvBytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (recvBytes > 0) {
            std::cout << "Antwort vom Server: " << buffer << std::endl;
        } else {
            std::cerr << "Server hat die Verbindung geschlossen.\n";
        }
    } else {
        std::cerr << "Keine Antwort vom Server innerhalb von 5 Sekunden.\n";
    }

    closesocket(clientSock);
    WSACleanup();
    return 0;
}
