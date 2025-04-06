# Projektübersicht: TLS Client-Server & Zertifikatsverwaltung

Willkommen im Wiki zum Projekt **TLS-Zertifikatsverwaltung & Client-Server-System**.  
Dieses Projekt enthält Tools für die Arbeit mit OpenSSL, CSR-Verwaltung, TLS-Verbindungen und eine eigene Test-CA.

## 📄 Wiki-Inhalte

- [Server-Architektur](https://github.com/paule32/TerminalServer/wiki/server-architektur.md)
- [Client mit Reconnect & TLS](https://github.com/paule32/TerminalServer/wiki/client-reconnect.md)
- [Zertifikatsverwaltung mit Qt5](https://github.com/paule32/TerminalServer/wiki/certmanager-ui.md)
- [OpenSSL & Test-CA Tipps](https://github.com/paule32/TerminalServer/wiki/openssl-ca-tipps.md)


[Protokoll](https://github.com/paule32/TerminalServer/wiki/TLS_Zertifikats_Session_Doku.md)

---

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

- [Protokoll: Sprach-Umschaltung](https://github.com/paule32/TerminalServer/wiki/SprachVerwaltung.md)

