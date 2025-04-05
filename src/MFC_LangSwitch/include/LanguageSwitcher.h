#pragma once

void LoadLanguageDLL(LPCTSTR langPath);
void LoadLanguageFromIni();
void SaveLanguageToIni();

extern CString currentLang;