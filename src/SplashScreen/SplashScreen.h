#pragma once
#include <afxcmn.h>

class CSplashScreen : public CDialog
{
public:
    CSplashScreen(CWnd* pParent = nullptr);

    void SetProgress(int value);
    void ShowSplash();
    void HideSplash();

protected:
    CProgressCtrl m_progressCtrl;
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};
