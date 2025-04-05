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
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_MYTHREAD_MESSAGE (WM_USER + 100)

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SENDNAME, &CMainDlg::OnBnClickedSendName)
    ON_MESSAGE(WM_MYTHREAD_MESSAGE, &CMainDlg::OnThreadMessage)
END_MESSAGE_MAP()

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MAINDIALOG, pParent)
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_NAME, m_NameEdit);
    DDX_Control(pDX, IDC_STATIC_RESPONSE, m_ResponseLabel);
}

void CMainDlg::OnBnClickedSendName()
{
    CString name;
    m_NameEdit.GetWindowTextW(name);
    AfxBeginThread(ClientThreadProc, new CString(name));
    m_ResponseLabel.SetWindowTextW(L"Sende...");
}

UINT CMainDlg::ClientThreadProc(LPVOID pParam)
{
    CString* namePtr = reinterpret_cast<CString*>(pParam);
    CString name = *namePtr;
    delete namePtr;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    
    CString ipStr;
    dlg->m_IpEdit.GetWindowTextW(ipStr);
    std::string ip = CT2A(ipStr);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);
    

    int result = connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::string message = "NAME:" + std::string(CT2A(name));
    send(sock, message.c_str(), static_cast<int>(message.length()), 0);

    char buffer[512] = {};
    int recvLen = recv(sock, buffer, sizeof(buffer) - 1, 0);
    closesocket(sock);
    WSACleanup();

    if (recvLen > 0) {
        buffer[recvLen] = '\0';
        CString* resultStr = new CString(CA2T(buffer));
        AfxGetMainWnd()->PostMessage(WM_MYTHREAD_MESSAGE, 0, reinterpret_cast<LPARAM>(resultStr));
    }

    return 0;
}

LRESULT CMainDlg::OnThreadMessage(WPARAM, LPARAM lParam)
{
    CString* str = reinterpret_cast<CString*>(lParam);
    m_ResponseLabel.SetWindowTextW(*str);
    delete str;
    return 0;
}
