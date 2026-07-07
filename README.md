# Redis-Like Server in C++

A Redis-like in-memory key-value database built from scratch in C++.

The goal of this project is to learn systems programming concepts by building a small database server step by step instead of relying on a complete framework.

## Documentation

- Networking fundamentals: `docs/networking-notes.md`

## Current Features

- In-memory key-value storage
- `SET` command
- `GET` command
- `DEL` command
- `EXISTS` command
- Command-line interface
- Command parsing
- Command validation
- Multi-word values
- Missing-key handling

## Example Usage

```text
> SET name Aryan Verma
OK

> GET name
Aryan Verma

> EXISTS name
1

> DEL name
1

> GET name
ERROR --> Key not found
```

## Architecture

```text
User Input
    |
    v
CommandParser
    |
    v
Command
    |
    v
CommandHandler
    |
    v
Database
    |
    v
std::unordered_map
```

## Project Structure

```text
Redis Server/
├── include/
│   ├── database.hpp
│   ├── command_parser.hpp
│   └── command_handler.hpp
├── src/
│   ├── main.cpp
│   ├── database.cpp
│   ├── command_parser.cpp
│   └── command_handler.cpp
├── docs/
├── tests/
├── CMakeLists.txt
├── .gitignore
└── README.md
