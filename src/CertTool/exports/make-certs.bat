@echo off
REM === OpenSSL Test-CA und Zertifikat-Erstellung ===
REM Dieses Skript erstellt:
REM - ca.key / ca.crt       (CA-Zertifikat)
REM - server.key / server.crt (Server-Zertifikat, signiert von CA)
REM - client.key / client.crt (Client-Zertifikat, signiert von CA)

REM === Parameter ===
set OPENSSL=@openssl
set DAYS=825
set CA_NAME=Test-CA

echo [ 1/12] Verzeichnisse erstellen...
if not exist data                            mkdir data                              >nul 2>&1
if not exist data\CA                         mkdir data\CA                           >nul 2>&1
if not exist data\CA\private                 mkdir data\CA\private                   >nul 2>&1
if not exist data\CA\private\serverA         mkdir data\CA\private\serverA           >nul 2>&1
if not exist data\CA\private\serverB         mkdir data\CA\private\serverB           >nul 2>&1
if not exist data\CA\private\serverC         mkdir data\CA\private\serverC           >nul 2>&1

if not exist data\CA\private\serverA\clients mkdir data\CA\private\serverA\clients   >nul 2>&1
if not exist data\CA\private\serverB\clients mkdir data\CA\private\serverB\clients   >nul 2>&1
if not exist data\CA\private\serverC\clients mkdir data\CA\private\serverC\clients   >nul 2>&1

if not exist data\CA\public                  mkdir data\CA\public                    >nul 2>&1
if not exist data\CA\public\serverA          mkdir data\CA\public\serverA            >nul 2>&1
if not exist data\CA\public\serverB          mkdir data\CA\public\serverB            >nul 2>&1
if not exist data\CA\public\serverC          mkdir data\CA\public\serverC            >nul 2>&1

if not exist data\CA\public\serverA\clients  mkdir data\CA\public\serverA\clients    >nul 2>&1
if not exist data\CA\public\serverB\clients  mkdir data\CA\public\serverB\clients    >nul 2>&1
if not exist data\CA\public\serverC\clients  mkdir data\CA\public\serverC\clients    >nul 2>&1

if not exist data\CA\crl mkdir data\CA\crl

echo 1000 > data\CA\serial
echo 1000 > data\CA\crl\crlnumber
type nul  > data\CA\crl\index.txt

echo [ 2/12] CA-Schlüssel erstellen...
%OPENSSL% genrsa -out data\CA\private\ca.key 4096

echo [ 3/12] CA-Zertifikat erstellen...
%OPENSSL% req -x509 -new -nodes ^
                   -key data\CA\private\ca.key -sha256 ^
        -days 3650 -out data\CA\public\ca.crt  -subj "/C=DE/O=Test CA/CN=Test Root CA"
echo.
echo === Erstelle Certificate Revocation List (CRL) ===
%OPENSSL% ca -gencrl -out data\CA\crl.pem -config openssl.cnf
echo.
echo [ 4/12] Server-Key erstellen...
%OPENSSL% genrsa   -out data\CA\private\serverA\server.key 2048
%OPENSSL% genrsa   -out data\CA\private\serverB\server.key 2048
%OPENSSL% genrsa   -out data\CA\private\serverC\server.key 2048

echo [ 5/12] Server-Zertifikatsanforderung TestServer A erstellen...
%OPENSSL% req -new -key data\CA\private\serverA\server.key ^
                   -out data\CA\public\serverA\server.csr -subj "/C=DE/O=TestServer1/CN=localhost"

echo [ 6/12] Server-Zertifikatsanforderung TestServer B erstellen...
%OPENSSL% req -new -key data\CA\private\serverB\server.key ^
                   -out data\CA\public\serverB\server.csr -subj "/C=DE/O=TestServer2/CN=localhost"

echo [ 7/12] Server-Zertifikatsanforderung TestServer C erstellen...
%OPENSSL% req -new -key data\CA\private\serverC\server.key ^
                   -out data\CA\public\serverC\server.csr -subj "/C=DE/O=TestServer3/CN=localhost"

echo.
echo [ 8/12] Server-Zertifikat für TestServer A signieren...
%OPENSSL% x509 -req -in data\CA\public\serverA\server.csr ^
                    -CA data\CA\public\ca.crt       ^
                 -CAkey data\CA\private\ca.key      ^
-CAcreateserial    -out data\CA\public\serverA\server.crt  -days %DAYS% -sha256 > nul

echo.
echo [ 9/12] Server-Zertifikat für TestServer B signieren...
%OPENSSL% x509 -req -in data\CA\public\serverB/server.csr ^
                    -CA data\CA\public\ca.crt       ^
                 -CAkey data\CA\private\ca.key      ^
-CAcreateserial    -out data\CA\public\serverB\server.crt  -days %DAYS% -sha256 > nul

echo.
echo [10/12] Server-Zertifikat für TestServer C signieren...
%OPENSSL% x509 -req -in data\CA\public\serverC\server.csr ^
                    -CA data\CA\public\ca.crt       ^
                 -CAkey data\CA\private\ca.key      ^
-CAcreateserial    -out data\CA\public\serverC\server.crt  -days %DAYS% -sha256 > nul

echo.
echo [11/12] Client-Zertifikat für Client A Server A erstellen und signieren...
%OPENSSL% genrsa     -out data\CA\private\serverA\clients\clientA.key 2048 > nul
%OPENSSL% req -new   -key data\CA\private\serverA\clients\clientA.key ^
                     -out data\CA\public\serverA\clients\clientA.csr -subj "/C=DE/O=TestClientA/CN=client" > nul
%OPENSSL% x509 -req   -in data\CA\public\serverA\clients\clientA.csr  ^
                      -CA data\CA\public\ca.crt  ^
                   -CAkey data\CA\private\ca.key ^
-CAcreateserial      -out data\CA\public\serverA\clients\clientA.crt -days %DAYS% -sha256 > nul

echo.
echo [12/12] Client-Zertifikat für Client B Server A erstellen und signieren...
%OPENSSL% genrsa     -out data\CA\private\serverA\clients\clientB.key 2048 > nul
%OPENSSL% req -new   -key data\CA\private\serverA\clients\clientB.key ^
                     -out data\CA\public\serverA\clients\clientB.csr -subj "/C=DE/O=TestClientA/CN=client" > nul
%OPENSSL% x509 -req   -in data\CA\public\serverA\clients\clientB.csr  ^
                      -CA data\CA\public\ca.crt  ^
                   -CAkey data\CA\private\ca.key ^
-CAcreateserial      -out data\CA\public\serverA\clients\clientB.crt -days %DAYS% -sha256 > nul

echo.
echo Zertifikate erstellt:
echo - ca.crt / ca.key
echo - server.crt / server.key
echo - client.crt / client.key
pause
