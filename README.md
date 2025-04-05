# 📋 Sprachmodul – Statusübersicht

## ✅ Implementiert

- [x] Sprachumschaltung im Menü (Deutsch / Englisch)
- [x] Dynamisches Nachladen per DLL (`LoadLibraryEx`)
- [x] `settings.ini` zur Speicherung der Sprachwahl
- [x] Verwendung von `LoadString()` für lokalisierte Texte

## 🔄 Status

| Funktion                 | Beschreibung                     |
|--------------------------|----------------------------------|
| `LanguageSwitcher.cpp`   | zentrale Logik                   |
| `resources_*.rc2`        | Sprachdefinition pro Sprache     |
| `build_resources.bat`    | erstellt `*.dll` für Ressourcen  |

## ⏭️ Optional

- [ ] Unterstützung für weitere Sprachen (z. B. Französisch)
- [ ] Kontextmenü für Sprachumschaltung in Taskleiste

---
- [Protokoll](TerminalServer/wiki/SprachVerwaltung.md)
