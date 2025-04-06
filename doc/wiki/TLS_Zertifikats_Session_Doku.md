# Zertifikatsverwaltung und TLS-Client-Server-System mit MFC & OpenSSL

Diese Session dokumentiert ein vollständiges Projekt zur Erstellung eines **TLS-gesicherten Client-Server-Systems** mit **C++ (MFC)** und **Python + Qt5** zur **Zertifikatsverwaltung**.  

## 📌 Überblick der Inhalte

- Non-blocking **TCP-Server mit WSAEventSelect** (C++)
- **MFC-TCP-Client** mit reconnect & optional OpenSSL-Unterstützung
- **Python + PyQt5 GUI-Tool** zur Verwaltung von Zertifikaten (CSR, CA, Clients)
- Verwendung von **OpenSSL** zur Erstellung selbstsignierter Zertifikate
- Unterstützung von:
  - Selbstsignierter CA
  - Server-Zertifikaten
  - Client-Zertifikaten (optional)
  - Sperren & Freigeben von Clients
  - CSR-Verwaltung & Signierung mit Laufzeitkontrolle (z. B. 30 Minuten)
  - Strukturierte Kommunikation & Zertifikatserneuerung

---

## 🔐 Zertifikatsverwaltungstool (Python + Qt5)

Ein Desktop-Tool zur Verwaltung von CSR-Dateien, Signierung über OpenSSL, Verwaltung von Clients und Zertifikatslaufzeiten.

### Features:
- CA-Tab: Anzeige von CSR-Dateien, Signieren, Konfiguration der Laufzeit (z. B. 30 Min)
- Clients-Tab: Liste aller Clients, Statusanzeige, Sperren/Freigeben
- Speicherung der Clientdaten in `clients.json`
- Unterstützt `openssl` über `subprocess` unter Windows

---

## 🧰 Server & Client Architektur

- TCP-Server: non-blocking, multi-client mit WSAEventSelect
- TCP-Client (MFC): mit reconnect & TLS (OpenSSL) möglich
- Optionaler mTLS-Modus mit Client-Zertifikaten
- Logging in `log.txt`
- Zertifikate: `server.crt`, `client.crt`, `ca.crt`

---

## 🔧 Tools & Technik

- C++, Visual Studio, WinSock2
- Python 3.10+, PyQt5
- OpenSSL (Windows-kompatibel)
- JSON als Datenbank
- Qt Designer für GUI (.ui)

---

## ⚖️ Lizenz

```text
MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

---

## 🧩 Weitere Entwicklung

Diese Markdown-Datei kann aktualisiert werden, um den Projektstatus weiterzugeben.  
Aktuelle Features sind vollständig funktionsfähig und können öffentlich geteilt werden.



---

## 🧵 Sessionzusammenfassung (Stichpunkte)

- Ziel war ein thread-sicherer, verschlüsselter Client-Server mit GUI und Zertifikatsverwaltung
- Server: C++ mit WSAEventSelect, mehrbenutzerfähig, non-blocking
- Client: MFC GUI, reconnect-fähig, optional SSL
- Zertifikatsverwaltung: Python + Qt5 GUI mit Tabstruktur, OpenSSL-Schnittstelle
- Features: CSR-Verarbeitung, Signierung, Sperrung, Ablaufsteuerung, Speicherung in JSON
- Tools: OpenSSL, PyQt5, Visual Studio, Qt Designer, subprocess, JSON
- Lizenz: MIT, frei verwendbar

