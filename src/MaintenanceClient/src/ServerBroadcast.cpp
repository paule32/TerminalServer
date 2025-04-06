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
// ServerBroadcast.cpp – Broadcast einer Wartungsnachricht an alle Clients

#include <winsock2.h>
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>

#pragma comment(lib, "ws2_32.lib")

// Beispielhafte Clientverwaltung
std::map<SOCKET, std::string> connectedClients; // z. B. SOCKET → Benutzername

void BroadcastMaintenanceMessage(const std::string& startTime, const std::string& duration, const std::string& note)
{
    std::ostringstream oss;
    oss << "{\"type\":\"maintenance\","
        << "\"message\":\"" << note << "\","
        << "\"start\":\"" << startTime << "\","
        << "\"duration\":\"" << duration << "\"}";

    std::string jsonMsg = oss.str();

    for (const auto& [sock, user] : connectedClients)
    {
        if (sock != INVALID_SOCKET)
        {
            send(sock, jsonMsg.c_str(), static_cast<int>(jsonMsg.length()), 0);
        }
    }
    printf("[Server] Broadcast-Maintenance-Nachricht gesendet an %zu Clients\n", connectedClients.size());
}

// Beispielaufruf zur geplanten Wartung (kann per Admin-Interface oder automatisch erfolgen):
// BroadcastMaintenanceMessage("17:00", "15", "Der Server wird ab 17:00 Uhr für 15 Minuten nicht erreichbar sein.");
