
#pragma once
void ShowTrayMaintenanceHint(const CString& message);
void RemoveTrayIcon();
void SetStatusBarText(CFrameWnd* pMainFrame, const CString& msg);
void StartMaintenanceCountdown(const CString& startStr);
void HandleMaintenanceStatus(const CString& start, const CString& duration, const CString& note);
