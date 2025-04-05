// ClientTrayStatus.cpp – Tray-Tipp + StatusBar-Text bei Wartungshinweis

#include <afxwin.h>
#include <shellapi.h>
#include <string>
#include <sstream>

#define WM_TRAY_NOTIFY (WM_USER + 1001)

NOTIFYICONDATA nid = {};

void ShowTrayMaintenanceHint(const CString& message)
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = AfxGetMainWnd()->GetSafeHwnd();
    nid.uID = 1;
    nid.uFlags = NIF_INFO | NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY_NOTIFY;
    nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    wcscpy_s(nid.szTip, L"Zertifikat-Tool aktiv");

    wcscpy_s(nid.szInfo, message.Left(255));
    wcscpy_s(nid.szInfoTitle, L"🛠 Server-Wartung");
    nid.dwInfoFlags = NIIF_WARNING;

    Shell_NotifyIcon(NIM_ADD, &nid);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void RemoveTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void SetStatusBarText(CFrameWnd* pMainFrame, const CString& msg)
{
    if (pMainFrame && pMainFrame->GetStatusBar())
    {
        pMainFrame->GetStatusBar()->SetPaneText(0, msg);
    }
}

// Beispiel aus Wartungsnachricht
void HandleMaintenanceStatus(const CString& start, const CString& duration, const CString& note)
{
    CString trayText;
    trayText.Format(L"%s\nBeginn: %s Uhr\nDauer: %s Minuten", note, start, duration);
    ShowTrayMaintenanceHint(trayText);

    CString statusMsg;
    statusMsg.Format(L"🛠 Wartung geplant ab %s Uhr (%s Min)", start, duration);
    SetStatusBarText((CFrameWnd*)AfxGetMainWnd(), statusMsg);
}
