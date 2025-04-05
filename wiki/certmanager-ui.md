# Qt5 GUI zur Zertifikatsverwaltung (Python)

Ein grafisches Tool zur Verwaltung deiner eigenen Test-CA mit OpenSSL.

## Tabs

### CA / Server

- Lade `.csr`-Dateien aus `data/requests/`
- Signiere sie mit Laufzeit z. B. 30 Min
- Zertifikate werden in `data/clients/` gespeichert
- Alles wird in `clients.json` dokumentiert

### Clients

- Zeigt signierte Clients
- Spalte: Name, Ablaufdatum, Status (aktiv/gesperrt)
- Buttons: [Sperren] / [Freigeben]

## Intern

- `subprocess` ruft `openssl` auf
- Zertifikate werden mit `-startdate`/`-enddate` versehen
