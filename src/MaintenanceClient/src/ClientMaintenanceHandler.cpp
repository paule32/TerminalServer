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
// ClientMaintenanceHandler.cpp – Wartungsnachricht empfangen und auswerten

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <thread>
#include <iostream>
#include <sstream>
#include <afxwin.h>

#pragma comment(lib, "ws2_32.lib")

void HandleMaintenanceMessage(const std::string& jsonMsg)
{
    // Einfaches Parsen (besser: JSON-Library verwenden)
    if (jsonMsg.find("\"type\":\"maintenance\"") != std::string::npos)
    {
        std::string message, start, duration;

        auto extract = [](const std::string& src, const std::string& key) -> std::string {
            size_t pos = src.find(key);
            if (pos == std::string::npos) return "";
            pos = src.find(':', pos);
            if (pos == std::string::npos) return "";
            size_t startQuote = src.find('"', pos + 1);
            size_t endQuote = src.find('"', startQuote + 1);
            if (startQuote == std::string::npos || endQuote == std::string::npos) return "";
            return src.substr(startQuote + 1, endQuote - startQuote - 1);
        };

        message = extract(jsonMsg, "message");
        start = extract(jsonMsg, "start");
        duration = extract(jsonMsg, "duration");

        std::ostringstream info;
        info << "⚠️ Wartungshinweis:\n" << message
             << "\nBeginn: " << start << " Uhr"
             << "\nDauer: " << duration << " Minuten";

        AfxMessageBox(CA2W(info.str().c_str()), MB_ICONINFORMATION | MB_OK);
    }
}

// Beispielhafte Verwendung innerhalb eines Socket-Empfangsloops:
// char buffer[1024] = {0};
// int len = recv(socket, buffer, sizeof(buffer), 0);
// if (len > 0)
// {
//     buffer[len] = '\0';
//     HandleMaintenanceMessage(buffer);
// }
