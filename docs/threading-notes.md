# Threading Notes

## Why Threads?

Without threads, the server can handle only one client at a time.

```text
Client A
    |
Server
    |
disconnect
    |
Client B
```

With threads:

```text
            Client A
               |
               |
Client B ---- Server ---- Client C
```

Each client runs inside its own thread.

---

## std::thread

```cpp
std::thread clientThread(
    &Server::handleClient,
    this,
    clientSocket
);
```

Creates a new thread that executes:

```cpp
this->handleClient(clientSocket);
```

---

## join()

```cpp
thread.join();
```

Waits until the thread finishes.

Not suitable for our server because the main thread would stop accepting new clients.

---

## detach()

```cpp
thread.detach();
```

Allows the thread to run independently while the main thread continues accepting clients.

---

## Race Condition

Multiple threads accessing the same resource simultaneously.

Example:

Thread A

```text
SET name Aryan
```

Thread B

```text
DEL name
```

Both modify the same database simultaneously.

---

## Mutex

```cpp
std::mutex memoryMutex;
```

Allows only one thread to access shared data at a time.

---

## mutable

```cpp
mutable std::mutex memoryMutex;
```

Allows locking inside const member functions because locking changes the mutex state but not the logical state of the database.

---

## std::lock_guard

```cpp
std::lock_guard<std::mutex> lock(memoryMutex);
```

Automatically:

- locks the mutex
- unlocks when leaving scope

Uses RAII.