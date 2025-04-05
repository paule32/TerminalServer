# Client mit Reconnect & TLS (MFC)

## Features

- GUI-basiert (MFC)
- Benutzername & Server-IP werden eingegeben
- Bei Verbindungsverlust: automatischer Reconnect
- Unterstützt TLS (OpenSSL) optional
- Zwei Varianten:
  - 🔓 Nur TLS-Verschlüsselung
  - 🔐 TLS + Client-Zertifikat (mTLS)

## Erweiterungen

- Fortschrittsanzeige
- Logische Trennung von Verbindungsversuch und Authentifizierung
