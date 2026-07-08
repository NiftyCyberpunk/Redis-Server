# Networking Notes

These notes document the networking concepts used while building
this Redis-like server.

## 1. Why do we need a port if we already have an IP address?

An IP address identifies the machine or network interface, while a
port identifies the specific application or service on that machine.

For example:

- 127.0.0.1:3000 → Node.js server
- 127.0.0.1:3306 → MySQL
- 127.0.0.1:6379 → Redis-like server

## 2. Why are we using TCP instead of UDP?

TCP provides reliable and ordered byte-stream communication. Lost data
can be retransmitted, and bytes are delivered to the application in
order.

UDP sends independent datagrams and does not guarantee delivery or
ordering.

Since database commands and responses must arrive reliably and in
order, TCP is more suitable for this Redis-like server.

## 3. What is the difference between a listening socket and a connected socket?

A listening socket waits for incoming connection requests.

When a client connects, accept() returns a connected socket that is
used to communicate with that specific client.

The original listening socket remains available to accept additional
connections.

## 4. Why can one recv() call not be assumed to equal one command?

TCP is a byte-stream protocol and does not preserve application-level
message boundaries.

A single command may be split across multiple recv() calls:

    recv #1 → "SET na"
    recv #2 → "me Ary"
    recv #3 → "an\n"

Multiple commands may also arrive in one recv() call:

    recv #1 → "SET name Aryan\nGET name\n"

Therefore, the application needs its own message-framing mechanism.

## Winsock Fundamentals

### Why do we need `WSAStartup()`?

`WSAStartup()` initializes the Windows Sockets API before the program uses Winsock functions.

### What is `WSADATA`?

`WSADATA` is a structure filled by `WSAStartup()` with information about the initialized Winsock implementation.

### Why initialize a socket with `INVALID_SOCKET`?

It gives the socket member a known invalid state before a real socket is created.

### What is `SOCKET`?

`SOCKET` is the Windows-defined handle type used to represent a socket.

### What do the `socket()` arguments mean?

- `AF_INET` → IPv4 address family
- `SOCK_STREAM` → stream-oriented socket
- `IPPROTO_TCP` → TCP protocol

Together, they create an IPv4 TCP stream socket.

### What is `ws2_32`?

`ws2_32` is the Windows Winsock library linked to the executable.

```cmake
target_link_libraries(cpp_redis PRIVATE ws2_32)
```

## Winsock lifecycle
WSAStartup()
    ↓
socket()
    ↓
bind()
    ↓
listen()
    ↓
accept()
    ↓
send() / recv()
    ↓
closesocket()
    ↓
WSACleanup()
