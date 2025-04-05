@echo off
REM === OpenSSL Test-CA und Zertifikat-Erstellung ===
REM Dieses Skript erstellt:
REM - ca.key / ca.crt       (CA-Zertifikat)
REM - server.key / server.crt (Server-Zertifikat, signiert von CA)
REM - client.key / client.crt (Client-Zertifikat, signiert von CA)

REM === Parameter ===
set OPENSSL=openssl
set DAYS=825
set CA_NAME=Test-CA

echo [1/6] CA-Schlüssel erstellen...
%OPENSSL% genrsa -out ca.key 4096

echo [2/6] CA-Zertifikat erstellen...
%OPENSSL% req -x509 -new -nodes -key ca.key -sha256 -days 3650 -out ca.crt -subj "/C=DE/O=Test CA/CN=Test Root CA"

echo [3/6] Server-Key erstellen...
%OPENSSL% genrsa -out server.key 2048

echo [4/6] Server-Zertifikatsanforderung erstellen...
%OPENSSL% req -new -key server.key -out server.csr -subj "/C=DE/O=TestServer/CN=localhost"

echo [5/6] Server-Zertifikat signieren...
%OPENSSL% x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days %DAYS% -sha256

echo [6/6] Client-Zertifikat erstellen und signieren...
%OPENSSL% genrsa -out client.key 2048
%OPENSSL% req -new -key client.key -out client.csr -subj "/C=DE/O=TestClient/CN=client"
%OPENSSL% x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days %DAYS% -sha256

echo.
echo Zertifikate erstellt:
echo - ca.crt / ca.key
echo - server.crt / server.key
echo - client.crt / client.key
pause
