# OpenSSL Test-CA Tipps & Tools

## 🛠 So richtest du eine lokale Test-CA ein

```bash
openssl genrsa -out ca.key 4096
openssl req -x509 -new -key ca.key -out ca.crt -days 3650 -sha256
```

## 📥 CSR signieren

```bash
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 1 -sha256
```

## ⏱ Laufzeit steuern

```bash
-startdate 20250404100000Z -enddate 20250404103000Z
```

## 🔐 mTLS aktivieren

- Server prüft mit: `SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, ...)`
- Client sendet zusätzlich: `client.crt`, `client.key`
