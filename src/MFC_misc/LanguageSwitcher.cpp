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
        LoadLanguageDLL(L"resources\\resources_en.dll");
    else
        LoadLanguageDLL(L"resources\\resources_de.dll");
}

void SaveLanguageToIni()
{
    WritePrivateProfileString(L"Language", L"Active", currentLang, L"settings.ini");
}

void CMainFrame::OnLanguageGerman()
{
    LoadLanguageDLL(L"resources\\resources_de.dll");
    currentLang = _T("de");
    AfxMessageBox(IDS_SPLASH_LOADING);
}

void CMainFrame::OnLanguageEnglish()
{
    LoadLanguageDLL(L"resources\\resources_en.dll");
    currentLang = _T("en");
    AfxMessageBox(IDS_SPLASH_LOADING);
}

// Aufruf:
// - In InitInstance(): LoadLanguageFromIni();
// - In ExitInstance(): SaveLanguageToIni();
