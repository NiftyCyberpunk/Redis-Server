# Protocol Notes

The server uses a simple text-based protocol inspired by Redis RESP.

## Success

```text
+OK
```

---

## Error

```text
-ERROR Key not found...
```

---

## Integer

```text
:1
```

```text
:0
```

---

## String

```text
$Aryan Verma
```

---

## Command Delimiter

Commands are separated using:

```text
\n
```

Example:

```text
SET name Aryan
GET name
```

Each command is extracted after a newline is received.

---

## Why Protocols?

Without response types:

```text
1
```

is ambiguous.

With prefixes:

```text
:1
```

means integer.

```text
$1
```

means string.

The client can distinguish responses without guessing.