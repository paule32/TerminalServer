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
