# Redis-Server

A Redis-inspired in-memory key-value database server built from scratch in **modern C++**.

This project is an educational implementation of a Redis-like server, focused on understanding how a real database server works internally — from TCP sockets and command parsing to persistence, expiration, concurrency, RESP, Pub/Sub, and replication.

> **Note:** This is not intended to be a drop-in replacement for Redis. It is a learning-oriented implementation that recreates selected Redis concepts from the ground up.

---

## ✨ Features

### 🗄️ In-Memory Database

The server provides an in-memory key-value store with support for:

* `SET`
* `GET`
* `DEL`
* `EXISTS`
* `MGET`
* `MSET`
* `KEYS`
* `DBSIZE`
* `FLUSHDB`
* `TYPE`
* `INCR`
* `DECR`

Values are stored in memory and accessed through the database abstraction.

---

### ⏱️ Key Expiration

Keys can automatically expire after a specified amount of time.

Supported commands:

* `EXPIRE`
* `TTL`
* `PERSIST`

The server also runs background cleanup for expired keys.

---

### 🌐 TCP Networking

The server communicates with clients over TCP using **Winsock2**.

The networking layer handles:

* Socket creation
* Binding
* Listening
* Accepting clients
* Receiving commands
* Sending responses
* Client disconnection
* Multiple connected clients

Each connected client is handled using a dedicated `std::thread`.

---

### 🧵 Multi-Threading & Thread Safety

Multiple clients can interact with the server concurrently.

The project uses:

* `std::thread`
* `std::mutex`
* `std::lock_guard`
* Thread-safe database operations
* Synchronization around replica connections and shared state

This makes the project a practical exploration of concurrent server design.

---

### 📡 RESP Support

The project includes a RESP parser and protocol formatting layer:

* RESP command parsing
* Simple strings
* Integers
* Bulk strings
* Error responses
* Structured command results

This provides a foundation for communicating with Redis-compatible clients while keeping protocol handling separate from command execution.

---

### 💾 Persistence

The server includes persistence support so data can survive beyond the lifetime of a single process.

Implemented components include:

* Persistence manager
* Append Only File (AOF)
* Database serialization
* Loading persisted data
* Command logging for write operations

Write commands can be recorded and persisted through the persistence layer.

---

### 📢 Pub/Sub

The project includes a Pub/Sub subsystem for message-based communication between clients.

The implementation provides infrastructure for:

* Channels
* Subscribers
* Publishing messages
* Client subscription management
* Removing disconnected clients

---

### 🔁 Replication

Replication-related functionality is included in the server architecture.

The project contains:

* Replica client implementation
* Replica connection management
* Command broadcasting
* `REPLCONF`
* `PSYNC`
* Replication socket tracking

Write commands can be identified and propagated to connected replicas.

> Replication is still an evolving part of the project and should be considered experimental rather than production-grade Redis replication.

---

### 📊 Server Statistics

The server tracks runtime information such as:

* Server uptime
* Connected clients
* Total commands processed
* Number of keys

The `INFO` command exposes these statistics.

---

### ⚙️ Configuration

Server configuration is separated from the core server implementation, allowing settings such as the server port and persistence configuration to be managed independently.

---

### 📝 Logging

A dedicated logging component is used throughout the server for operational messages and error reporting.

This makes debugging networking and server-side failures easier without mixing logging logic into the core components.

---

## 🧩 Architecture

The project is organized into several layers:

```text
                         ┌───────────────────┐
                         │      Client       │
                         └─────────┬─────────┘
                                   │
                                   │ TCP
                                   ▼
                         ┌───────────────────┐
                         │      Server       │
                         │    Winsock2       │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │  Command Parser   │
                         │   / RESP Parser   │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │ Command Handler   │
                         └─────────┬─────────┘
                                   │
                 ┌─────────────────┼─────────────────┐
                 │                 │                 │
                 ▼                 ▼                 ▼
        ┌────────────────┐ ┌───────────────┐ ┌───────────────┐
        │    Database    │ │  Persistence  │ │    Pub/Sub    │
        └────────────────┘ └───────────────┘ └───────────────┘
                 │
                 ▼
        ┌──────────────────────┐
        │ In-Memory Key-Value  │
        │       Storage        │
        └──────────────────────┘
```

The codebase separates networking, parsing, command execution, storage, persistence, protocol formatting, and supporting services into independent components.

---

## 📁 Project Structure

```text
Redis-Server/
│
├── include/
│   ├── aof.hpp
│   ├── command.hpp
│   ├── command_handler.hpp
│   ├── command_parser.hpp
│   ├── command_result.hpp
│   ├── config.hpp
│   ├── database.hpp
│   ├── logger.hpp
│   ├── persistence.hpp
│   ├── protocol_formatter.hpp
│   ├── pubsub.hpp
│   ├── replica_client.hpp
│   ├── resp_parser.hpp
│   ├── server.hpp
│   ├── server_stats.hpp
│   └── tests_utils.hpp
│
├── src/
│   ├── aof.cpp
│   ├── command_handler.cpp
│   ├── command_parser.cpp
│   ├── config.cpp
│   ├── database.cpp
│   ├── logger.cpp
│   ├── main.cpp
│   ├── persistence.cpp
│   ├── protocol_formatter.cpp
│   ├── pubsub.cpp
│   ├── replica_client.cpp
│   ├── resp_parser.cpp
│   └── server.cpp
│
├── tests/
│   ├── database_tests.cpp
│   ├── formatter_tests.cpp
│   ├── handler_tests.cpp
│   ├── parser_tests.cpp
│   ├── tests_utils.cpp
│   └── main.cpp
│
├── benchmarks/
│
├── docs/
│
├── CMakeLists.txt
├── master.cfg
├── replica.cfg
└── README.md
```

The repository currently separates public interfaces under `include/`, implementations under `src/`, automated tests under `tests/`, and supporting documentation under `docs/`.

---

## 🛠️ Tech Stack

| Technology   | Purpose                                           |
| ------------ | ------------------------------------------------- |
| **C++17**    | Core implementation                               |
| **STL**      | Containers, threading, synchronization, utilities |
| **Winsock2** | TCP networking                                    |
| **CMake**    | Build system                                      |
| **Ninja**    | Fast build generation                             |
| **Threads**  | Concurrent client handling                        |
| **Mutexes**  | Shared-state synchronization                      |

---

## 🚀 Getting Started

### Prerequisites

Make sure you have:

* A C++17-compatible compiler
* CMake 3.16+
* Ninja
* Windows, since the current networking implementation uses Winsock2

---

### Clone the Repository

```bash
git clone https://github.com/NiftyCyberpunk/Redis-Server.git
cd Redis-Server
```

---

### Configure the Project

Using CMake + Ninja:

```bash
cmake -S . -B build -G Ninja
```

---

### Build

```bash
cmake --build build
```

---

### Run

On Windows:

```bash
./build/RedisServer.exe
```

The server will start listening on the configured port.

---

## 💻 Example Commands

Once the server is running, connect to it using a TCP client such as `telnet`, `nc`, or a custom client.

### SET / GET

```text
SET name Aditya
+OK

GET name
$Aditya
```

### EXISTS

```text
EXISTS name
:1
```

### DELETE

```text
DEL name
:1
```

### Increment / Decrement

```text
SET counter 10
+OK

INCR counter
:11

DECR counter
:10
```

### Expiration

```text
SET session abc123
+OK

EXPIRE session 60
:1

TTL session
:59
```

### Ping

```text
PING
+PONG
```

### Database Information

```text
INFO
```

The server returns information about uptime, connected clients, processed commands, and keyspace statistics.

---

## 🧪 Testing

The repository contains tests for several core components:

* Database
* Command parser
* Command handler
* Protocol formatter

Tests are located under:

```text
tests/
```

The project uses CMake to integrate the test targets into the build system.

---

## 🧠 What This Project Teaches

This project is primarily about **systems programming and understanding what happens underneath a database API**.

By working through the implementation, you get hands-on experience with:

* C++17
* Object-oriented design
* RAII
* TCP/IP
* Socket programming
* Winsock
* Client-server architecture
* Multi-threading
* Mutexes and synchronization
* Command parsing
* Protocol design
* RESP
* In-memory data structures
* Persistence
* AOF
* Key expiration
* Pub/Sub
* Replication concepts
* Configuration management
* Logging
* Testing
* Database server architecture

---

## 🗺️ Development Roadmap

The project is being developed incrementally, with the goal of gradually expanding the Redis feature set.

### Completed / Implemented

* [x] In-memory key-value database
* [x] Basic CRUD operations
* [x] Command parsing
* [x] TCP server
* [x] Multiple clients
* [x] Multi-threaded client handling
* [x] Thread synchronization
* [x] Structured responses
* [x] RESP parsing infrastructure
* [x] Persistence
* [x] AOF
* [x] TTL / key expiration
* [x] Server statistics
* [x] Configuration
* [x] Logging
* [x] Pub/Sub infrastructure
* [x] Replication infrastructure
* [x] Unit tests

### Future Improvements

* [ ] More Redis data types
* [ ] More Redis commands
* [ ] More complete RESP compatibility
* [ ] More robust replication
* [ ] Transactions
* [ ] Improved concurrency model
* [ ] Connection/event-loop optimization
* [ ] Better persistence guarantees
* [ ] Performance benchmarking
* [ ] Linux/macOS networking support
* [ ] Production-grade error handling

---

## 🎯 Why Build Redis From Scratch?

Using Redis is easy.

Understanding **how Redis-like systems actually work** is the interesting part.

This project breaks down a database server into smaller problems:

```text
How does a client connect?
        ↓
How do we receive bytes?
        ↓
How do we frame a command?
        ↓
How do we parse it?
        ↓
How do we execute it?
        ↓
How do we store the data?
        ↓
How do we synchronize multiple clients?
        ↓
How do we persist the data?
        ↓
How do keys expire?
        ↓
How can clients communicate?
        ↓
How can data be replicated?
```

Instead of treating a database as a black box, this project explores the machinery behind it.

---

## 📚 Documentation

Additional implementation notes and design documentation can be found in the `docs/` directory.

Topics include networking, threading, protocol design, and architecture.

---

## ⚠️ Disclaimer

This project is intended for **educational purposes**.

It should not be considered a production replacement for Redis. Some Redis features are intentionally simplified or implemented only partially while the project evolves.

For the real Redis project, visit the official [Redis repository](https://github.com/redis/redis?utm_source=chatgpt.com).

---

## 📄 License

This project is currently intended for educational use.

See the repository for the latest licensing information.

---

## ⭐ Contributing

Contributions, suggestions, and improvements are welcome.

If you're interested in learning about databases, networking, distributed systems, or modern C++, feel free to explore the code, open an issue, or submit a pull request.

---

**Built from scratch to understand what happens behind `GET`, `SET`, and everything in between.**
