# Redis-Like Server in Modern C++

A Redis-inspired in-memory key-value database server built from scratch in modern C++.

The purpose of this project is to understand how databases and network servers work internally by implementing them step by step instead of relying on existing frameworks or libraries.

---

## Learning Goals

This project focuses on learning:

- Modern C++ (C++17)
- Object-Oriented Design
- RAII (Resource Acquisition Is Initialization)
- Socket Programming (Winsock)
- TCP/IP Networking
- Multi-threading (`std::thread`)
- Thread Synchronization (`std::mutex`)
- Command Parsing
- Custom Protocol Design
- Software Architecture
- Database Internals

---

## Current Features

### Database

- In-memory key-value storage
- `SET`
- `GET`
- `DEL`
- `EXISTS`

### Networking

- TCP server using Winsock
- Multiple sequential client support
- Multi-threaded client handling
- Thread-safe database using `std::mutex`
- Newline-based command framing

### Command Processing

- Command parser
- Command validation
- Multi-word values
- Graceful error handling
- Structured protocol responses

---

## Example Session

```text
SET name Aryan Verma
+OK

GET name
$Aryan Verma

EXISTS name
:1

DEL name
:1

EXISTS name
:0

GET name
-ERROR Key not found...
```

---

## Architecture

```text
                    +----------------+
                    |   TCP Client   |
                    +-------+--------+
                            |
                            v
                    +----------------+
                    |     Server     |
                    +-------+--------+
                            |
                            v
                    +----------------+
                    | CommandParser  |
                    +-------+--------+
                            |
                            v
                    +----------------+
                    | CommandHandler |
                    +-------+--------+
                            |
                            v
                    +----------------+
                    |    Database    |
                    +-------+--------+
                            |
                            v
               std::unordered_map<std::string, std::string>
```

---

## Project Structure

```text
RedisServer/
│
├── include/
│   ├── database.hpp
│   ├── command_handler.hpp
│   ├── command_parser.hpp
│   └── server.hpp
│
├── src/
│   ├── database.cpp
│   ├── command_handler.cpp
│   ├── command_parser.cpp
│   ├── server.cpp
│   └── main.cpp
│
├── docs/
│   └── networking-notes.md
│
├── tests/
│
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

## Build

### Requirements

- C++17 compatible compiler
- CMake 3.16+
- Ninja (or another supported generator)

### Configure

```bash
cmake -S . -B build -G Ninja
```

### Build

```bash
cmake --build build
```

### Run

```bash
./build/RedisServer.exe
```

---

## Technologies Used

- C++17
- STL
- Winsock2
- CMake
- Ninja

---

## Project Progress

### Phase 1 — Database

- [x] Day 1 — In-memory database
- [x] Day 2 — CRUD operations
- [x] Day 3 — Command handling
- [x] Day 4 — Command parser
- [x] Day 5 — Interactive CLI
- [x] Day 6 — Command execution

### Phase 2 — Networking

- [x] Day 7 — TCP server setup
- [x] Day 8 — TCP message framing
- [x] Day 9 — Network command processing
- [x] Day 10 — Sequential client handling
- [x] Day 11 — Multi-threading & thread safety
- [x] Day 12 — Structured protocol responses

### Phase 3 — Planned Features

- [ ] Protocol abstraction
- [ ] Redis RESP implementation
- [ ] Persistence (save/load database)
- [ ] Key expiration (TTL)
- [ ] Configuration system
- [ ] Logging
- [ ] Transactions
- [ ] Publish / Subscribe
- [ ] Replication

---

## Documentation

Project notes are available in the `docs/` directory.

- `networking-notes.md` — TCP, sockets, Winsock, framing
- `threading-notes.md` — threads, mutexes, RAII
- `protocol-notes.md` — custom protocol design
- `architecture.md` — server architecture

## Why This Project?

Instead of building another CRUD application, this project focuses on understanding how a real database server works internally.

Each feature is implemented from scratch to explore concepts such as:

- networking
- concurrency
- synchronization
- protocol design
- software architecture
- systems programming

The project is developed incrementally, with each milestone building on the previous one.

---

## License

This project is intended for educational purposes.