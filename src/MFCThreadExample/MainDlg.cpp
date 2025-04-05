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

#include "pch.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_STARTTHREAD, &CMainDlg::OnBnClickedStartThread)
    ON_BN_CLICKED(IDC_BUTTON_CANCELTHREAD, &CMainDlg::OnBnClickedCancelThread)
    ON_MESSAGE(WM_MYTHREAD_PROGRESS, &CMainDlg::OnThreadProgress)
    ON_MESSAGE(WM_MYTHREAD_FINISHED, &CMainDlg::OnThreadFinished)
END_MESSAGE_MAP()

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MAINDIALOG, pParent)
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
    DDX_Control(pDX, IDC_BUTTON_CANCELTHREAD, m_CancelButton);
}

void CMainDlg::OnBnClickedStartThread()
{
    m_CancelRequested = false;
    m_ProgressBar.SetRange(0, 100);
    m_ProgressBar.SetPos(0);
    m_CancelButton.EnableWindow(TRUE);
    AfxBeginThread(ThreadProc, this);
}

void CMainDlg::OnBnClickedCancelThread()
{
    m_CancelRequested = true;
    m_CancelButton.EnableWindow(FALSE);
}

UINT CMainDlg::ThreadProc(LPVOID pParam)
{
    CMainDlg* dlg = reinterpret_cast<CMainDlg*>(pParam);

    for (int i = 0; i <= 100; i += 10)
    {
        if (dlg->m_CancelRequested) {
            dlg->PostMessage(WM_MYTHREAD_FINISHED, 1, 0);
            return 0;
        }

        dlg->PostMessage(WM_MYTHREAD_PROGRESS, i, 0);
        Sleep(300);
    }

    dlg->PostMessage(WM_MYTHREAD_FINISHED, 0, 0);
    return 0;
}

LRESULT CMainDlg::OnThreadProgress(WPARAM wParam, LPARAM)
{
    m_ProgressBar.SetPos(static_cast<int>(wParam));
    return 0;
}

LRESULT CMainDlg::OnThreadFinished(WPARAM wParam, LPARAM)
{
    CString result;
    if (wParam == 0)
        result = _T("Vorgang erfolgreich abgeschlossen.");
    else
        result = _T("Vorgang wurde abgebrochen.");

    MessageBox(result, _T("Thread beendet"));
    m_CancelButton.EnableWindow(FALSE);
    return 0;
}
