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
#pragma once

# include "pch.h"

#define WM_MYTHREAD_MESSAGE (WM_USER + 100)

class CMainDlg : public CDialogEx
{
public:
    CMainDlg(CWnd* pParent = nullptr);
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MAINDIALOG };
#endif

protected:
    CEdit m_NameEdit;
    CEdit m_IpEdit;
    CStatic m_ResponseLabel;

    virtual void PostNcDestroy() override;
    virtual void DoDataExchange(CDataExchange* pDX);
    
    afx_msg void OnBnClickedSendName();
    afx_msg LRESULT OnThreadMessage(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedClose();

    static UINT ClientThreadProc(LPVOID pParam);

    DECLARE_MESSAGE_MAP()
};

