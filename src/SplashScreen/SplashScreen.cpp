// SplashScreen.cpp – Splash-Screen mit Bild + Fortschrittsbalken (Beispiel-Layout-Vorlage)

#include "SplashScreen.h"
#include <afxwin.h>
#include "resource.h"

IMPLEMENT_DYNAMIC(CSplashScreen, CDialog)

CSplashScreen::CSplashScreen(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_SPLASH_DIALOG, pParent)
{
}

BOOL CSplashScreen::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Fortschrittsbalken initialisieren
    m_progressCtrl.SubclassDlgItem(IDC_SPLASH_PROGRESS, this);
    m_progressCtrl.SetRange(0, 100);
    m_progressCtrl.SetPos(0);

    // Splash-Dialog zentrieren
    CenterWindow();
    return TRUE;
}

void CSplashScreen::SetProgress(int value)
{
    if (m_progressCtrl.GetSafeHwnd())
        m_progressCtrl.SetPos(value);
    UpdateWindow();
}

void CSplashScreen::ShowSplash()
{
    Create(IDD_SPLASH_DIALOG);
    ShowWindow(SW_SHOW);
    UpdateWindow();
}

void CSplashScreen::HideSplash()
{
    ShowWindow(SW_HIDE);
    DestroyWindow();
}

BEGIN_MESSAGE_MAP(CSplashScreen, CDialog)
END_MESSAGE_MAP()

/*
Beispiel für Layout im Ressourcen-Editor (Splashscreen.rc):

IDD_SPLASH_DIALOG DIALOGEX 0, 0, 400, 200
STYLE DS_SETFONT | WS_POPUP | WS_VISIBLE
FONT 9, "Segoe UI"
BEGIN
    CONTROL         "",IDC_SPLASH_IMAGE,"Static",SS_BITMAP | SS_CENTERIMAGE,10,10,380,120
    CONTROL         "",IDC_SPLASH_PROGRESS,"msctls_progress32",WS_CHILD | WS_VISIBLE,10,150,380,14
END
*/
