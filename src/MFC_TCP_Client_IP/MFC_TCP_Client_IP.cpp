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

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_MYTHREAD_MESSAGE (WM_USER + 100)

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SENDNAME, &CMainDlg::OnBnClickedSendName)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CMainDlg::OnBnClickedClose)
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
    DDX_Control(pDX, IDC_EDIT_IP, m_IpEdit);
    DDX_Control(pDX, IDC_BUTTON_SENDNAME, m_IpEdit);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_IpEdit);
    DDX_Control(pDX, IDC_STATIC_RESPONSE, m_ResponseLabel);
}

void CMainDlg::OnBnClickedClose()
{
    if (GetStyle() & DS_MODALFRAME)  // prüfe ob modal
    EndDialog(IDC_BUTTON_CANCEL); else
    DestroyWindow();  // modelless
}
void CMainDlg::PostNcDestroy() {
    delete this;
}

void CMainDlg::OnBnClickedSendName()
{
    CString name;
    CString ip;

    m_NameEdit.GetWindowTextA(name);
    m_IpEdit.GetWindowTextA(ip);

    // Werte an Thread übergeben (kombiniert als Struktur oder CString-Pointer-Array)
    auto* params = new std::pair<CString, CString>(name, ip);
    AfxBeginThread(ClientThreadProc, params);

    m_ResponseLabel.SetWindowTextA("Sende...");
}

UINT CMainDlg::ClientThreadProc(LPVOID pParam)
{
    auto* params = reinterpret_cast<std::pair<CString, CString>*>(pParam);
    CString name = params->first;
    CString ipStr = params->second;
    delete params;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return 1;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    
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
    m_ResponseLabel.SetWindowTextA(*str);
    delete str;
    return 0;
}
