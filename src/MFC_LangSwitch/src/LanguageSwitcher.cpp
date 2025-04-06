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
// LanguageSwitcher.cpp – Sprachumschaltung über Menü + INI-Unterstützung

#include <afxwin.h>
#include <windows.h>
#include "resource.h"

HINSTANCE hLangModule = nullptr;
CString currentLang = _T("de"); // Defaultsprache

void LoadLanguageDLL(LPCTSTR langPath)
{
    if (hLangModule)
        FreeLibrary(hLangModule);

    hLangModule = LoadLibraryEx(langPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (hLangModule)
        AfxSetResourceHandle(hLangModule);
    else
        AfxMessageBox(_T("Sprachdatei konnte nicht geladen werden."), MB_ICONERROR);
}

void LoadLanguageFromIni()
{
    TCHAR lang[10] = { 0 };
    GetPrivateProfileString(L"Language", L"Active", L"de", lang, sizeof(lang) / sizeof(TCHAR), L"settings.ini");

    currentLang = lang;
    if (currentLang == L"en")
        LoadLanguageDLL(L"resources\resources_en.dll");
    else
        LoadLanguageDLL(L"resources\resources_de.dll");
}

void SaveLanguageToIni()
{
    WritePrivateProfileString(L"Language", L"Active", currentLang, L"settings.ini");
}

void CMainFrame::OnLanguageGerman()
{
    LoadLanguageDLL(L"resources\resources_de.dll");
    currentLang = _T("de");
    AfxMessageBox(IDS_SPLASH_LOADING);
}

void CMainFrame::OnLanguageEnglish()
{
    LoadLanguageDLL(L"resources\resources_en.dll");
    currentLang = _T("en");
    AfxMessageBox(IDS_SPLASH_LOADING);
}

// Aufruf:
// - In InitInstance(): LoadLanguageFromIni();
// - In ExitInstance(): SaveLanguageToIni();
