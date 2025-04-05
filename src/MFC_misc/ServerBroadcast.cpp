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
