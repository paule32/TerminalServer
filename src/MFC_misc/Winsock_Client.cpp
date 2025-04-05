
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
