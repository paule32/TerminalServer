# Server-Architektur (C++ mit WinSock2)

Dieser Server:

- ist non-blocking
- verwendet `WSAEventSelect`
- unterstützt mehrere gleichzeitige Clients
- erkennt `NAME:<user>`-Nachrichten
- loggt Aktivitäten in `log.txt`
- lässt sich auf TLS (OpenSSL) erweitern

## Technik

- `socket()`, `bind()`, `listen()`, `accept()`
- `WSACreateEvent()`, `WSAWaitForMultipleEvents()`
- getrennte Behandlung von `FD_ACCEPT`, `FD_READ`, `FD_CLOSE`
