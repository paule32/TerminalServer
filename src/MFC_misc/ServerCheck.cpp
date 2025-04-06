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

// ServerCheck.cpp
// MFC-kompatibler C++ Code für Serververbindung mit .ini-Parsing + select()

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

struct ServerEntry {
    std::string hostname;
    int port;
};

bool ParseServerLine(const std::string& line, ServerEntry& entry) {
    size_t colon = line.find(":");
    if (colon == std::string::npos) return false;
    entry.hostname = line.substr(0, colon);
    entry.port = std::stoi(line.substr(colon + 1));
    return true;
}

std::vector<ServerEntry> LoadServerList(const std::string& iniPath) {
    std::vector<ServerEntry> servers;
    char buffer[4096] = {0};
    GetPrivateProfileSection("ServerList", buffer, sizeof(buffer), iniPath.c_str());

    const char* ptr = buffer;
    while (*ptr) {
        std::string line = ptr;
        size_t eq = line.find("=");
        if (eq != std::string::npos) {
            std::string value = line.substr(eq + 1);
            ServerEntry entry;
            if (ParseServerLine(value, entry)) {
                servers.push_back(entry);
            }
        }
        ptr += line.length() + 1;
    }
    return servers;
}

int GetTimeout(const std::string& iniPath) {
    return GetPrivateProfileInt("ServerList", "TimeoutMS", 3000, iniPath.c_str());
}

bool TryConnect(const std::string& host, int port, int timeoutMs) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    inet_pton(AF_INET, host.c_str(), &sa.sin_addr);

    connect(sock, (sockaddr*)&sa, sizeof(sa));

    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);

    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int sel = select(0, NULL, &writefds, NULL, &tv);
    if (sel > 0) {
        int err = 0;
        socklen_t len = sizeof(err);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len);
        closesocket(sock);
        return err == 0;
    }
    closesocket(sock);
    return false;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    std::string iniPath = "client_settings.ini";
    auto servers = LoadServerList(iniPath);
    int timeout = GetTimeout(iniPath);

    for (const auto& server : servers) {
        std::cout << "Prüfe: " << server.hostname << ":" << server.port << std::endl;
        if (TryConnect(server.hostname, server.port, timeout)) {
            std::cout << "✅ Verbindung möglich zu: " << server.hostname << ":" << server.port << std::endl;
            break;
        } else {
            std::cout << "❌ Keine Verbindung zu: " << server.hostname << std::endl;
        }
    }

    WSACleanup();
    return 0;
}
