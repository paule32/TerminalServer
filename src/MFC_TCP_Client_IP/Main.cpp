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
# include "pch.h"
# include "resource.h"
# include "MFC_TCP_Client_IP.h"

class CMainFrame : public CFrameWnd {
public:
    CMainFrame() = default;

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

// ---------------------------------------------------------------------------------------
// Nachrichten-Map für Klasse: CMainFrame
// ---------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CMainFrame::OnPaint()
{
    CPaintDC dc(this);
    dc.TextOutA(10, 10, L"Hallo MFC!");
}

// ---------------------------------------------------------------------------------------
// MFC-Anwendungsklasse
// ---------------------------------------------------------------------------------------
class CMiniApp : public CWinApp
{
private:
public:
    virtual BOOL InitInstance() override {
        auto *pFrame = new CMainFrame();
        
        if (!pFrame->Create(NULL, _T("Meine MFC-Anwendung"))) {
            AfxMessageBox(_T("Fenster konnte nicht erzeugt werden"));
            delete pFrame;
            return FALSE;
        }

        m_pMainWnd = pFrame;

        pFrame->ShowWindow(SW_SHOW);
        pFrame->UpdateWindow();

        auto* dialog = new CMainDlg();
        if (!dialog->Create(IDD_MAINDIALOG)) {
            AfxMessageBox(_T("Fenster konnte nicht erzeugt werden"));
            delete dialog;
            return FALSE;
        }
        dialog->ShowWindow(SW_SHOW);

        return TRUE;
    }
};

// ---------------------------------------------------------------------------------------
// globale Instanz
// ---------------------------------------------------------------------------------------
CMiniApp theApp;
