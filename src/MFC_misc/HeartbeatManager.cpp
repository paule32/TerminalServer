// HeartbeatManager.cpp
// Einbindung in MFC-Anwendung

#include <afxwin.h>
#include <afxsock.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <windows.h>

// Globale Session-Datenstruktur
struct SessionInfo {
    CString Benutzername;
    std::vector<CString> MessageBuffer;
    CString AktiverServer;
    UINT AktiverPort;
};

std::atomic<bool> g_Running(true);
std::atomic<bool> g_ServerReachable(true);
std::mutex g_BufferMutex;
SessionInfo g_Session;

UINT HeartbeatIntervalSec = 20; // Standardwert, wird später überschrieben

BOOL TestServerVerbindung(const CString& host, UINT port, DWORD timeoutMs)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        return FALSE;

    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    inet_pton(AF_INET, CStringA(host), &sa.sin_addr);

    connect(sock, (sockaddr*)&sa, sizeof(sa));

    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);
    
    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int sel = select(0, NULL, &writeSet, NULL, &tv);
    closesocket(sock);

    return sel > 0;
}

UINT __stdcall HeartbeatThreadProc(LPVOID pParam)
{
    while (g_Running)
    {
        Sleep(HeartbeatIntervalSec * 1000);
        if (!TestServerVerbindung(g_Session.AktiverServer, g_Session.AktiverPort, 2000)) {
            g_ServerReachable = false;
            // Reconnect versuchen
            ReconnectAndResumeSession();
        } else {
            g_ServerReachable = true;
        }
    }
    return 0;
}

void ReconnectAndResumeSession()
{
    // Serverliste laden (analog vorheriger Ini-Parsing-Code)
    std::vector<std::pair<CString, UINT>> serverList = LoadServerListFromIni();

    for (auto& pair : serverList)
    {
        if (TestServerVerbindung(pair.first, pair.second, 2000)) {
            g_Session.AktiverServer = pair.first;
            g_Session.AktiverPort = pair.second;
            // Verbindung neu aufbauen...
            ReconnectSocket(pair.first, pair.second);

            // Gespeicherte Nachrichten erneut senden
            std::lock_guard<std::mutex> lock(g_BufferMutex);
            for (const auto& msg : g_Session.MessageBuffer)
                SendMessageToServer(msg);

            g_Session.MessageBuffer.clear();
            break;
        }
    }
}

void AddMessageToBuffer(const CString& msg)
{
    std::lock_guard<std::mutex> lock(g_BufferMutex);
    g_Session.MessageBuffer.push_back(msg);
}

// Beispielaufruf im MFC InitDialog
void StartHeartbeatThread()
{
    UINT iniInterval = GetPrivateProfileInt("Client", "HeartbeatIntervalSec", 20, L"client_settings.ini");
    HeartbeatIntervalSec = min(max(iniInterval, 10), 30);

    AfxBeginThread(HeartbeatThreadProc, NULL);
}

// Beim Beenden
void StopHeartbeatThread()
{
    g_Running = false;
}

// Funktions-Signaturen, die du im Projekt umsetzen oder verknüpfen solltest:
// std::vector<std::pair<CString, UINT>> LoadServerListFromIni();
// void ReconnectSocket(const CString& host, UINT port);
// void SendMessageToServer(const CString& msg);
