# Server Architecture

```text
                    TCP Client
                        |
                        v
                  +-------------+
                  |   Server    |
                  +------+------+ 
                         |
                         v
                CommandParser
                         |
                         v
                CommandHandler
                         |
                         v
                   Database
                         |
                         v
               unordered_map
```

## Responsibilities

### Server

- TCP networking
- Accept connections
- Receive data
- Send responses
- Client lifecycle

### CommandParser

Converts raw input into structured commands.

### CommandHandler

Executes commands.

### Database

Stores key-value pairs.

---

## Threading

```text
Main Thread

accept()
      |
      +----------- Thread A
      |
      +----------- Thread B
      |
      +----------- Thread C
```

Each client executes independently.

The shared database is protected using a mutex.