// ClientTrayStatus.cpp – Tray-Tipp + StatusBar-Text + Countdown bei Wartungshinweis
#include <afxwin.h>
#include <shellapi.h>
#include <string>
#include <sstream>
#define WM_TRAY_NOTIFY (WM_USER + 1001)
#define IDT_MAINTENANCE_TIMER 1234
NOTIFYICONDATA nid = {};
CTime maintenanceStart;
bool countdownActive = false;

// ... (Funktionsdefinitionen wie HandleMaintenanceStatus usw.)
