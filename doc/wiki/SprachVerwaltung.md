# 🌐 Sprachumschaltung – Übersicht

Dieses Projekt unterstützt die dynamische Sprachumschaltung (Deutsch/Englisch) über Menü und `.ini`-Datei.

## 🛠 Komponenten

- **LanguageSwitcher.cpp** – enthält:
  - `LoadLanguageFromIni()` – liest Sprachwahl beim Start
  - `SaveLanguageToIni()` – speichert Sprache beim Beenden
  - `LoadLanguageDLL()` – lädt Sprach-Resource-DLL
  - Menü-Handler `OnLanguageGerman()`, `OnLanguageEnglish()`

## 📁 Ressourcenstruktur

```
resources/
├── resources_de.dll
├── resources_en.dll
├── settings.ini
```

## 📄 Beispiel `settings.ini`

```ini
[Language]
Active = de
```

## 🧩 Integration

```cpp
InitInstance()   => LoadLanguageFromIni();
ExitInstance()   => SaveLanguageToIni();
```

---
