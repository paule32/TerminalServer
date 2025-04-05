# 🌐 LanguageSwitcherProject

Ein C++/MFC-Projekt zur dynamischen Sprachumschaltung mittels DLL und .ini-Datei. Ermöglicht mehrsprachige GUI mit manueller oder automatischer Umschaltung.

## 🔧 Features

- Umschaltung per Menüeintrag (Deutsch / Englisch)
- Speicherung der Sprachwahl in `settings.ini`
- Ressourcen getrennt in `.rc2`-Dateien
- Sprachpakete als DLL (mit `LoadLibraryEx`)
- Unterstützt `LoadString()`-basierte Lokalisierung

## 📁 Verzeichnisstruktur

```
├── src/                # Quellcode
├── include/            # Header-Dateien
├── resources/          # .rc/.def Dateien
├── docs/               # Dokumentation
├── settings.ini        # Spracheinstellung
├── LICENSE             # MIT Lizenz
```

## ▶️ Kompilierung

Nutze Visual Studio oder den Developer Command Prompt:

```bat
build_resources.bat
```

## 📜 Lizenz

Dieses Projekt steht unter der MIT-Lizenz.
