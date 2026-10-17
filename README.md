# Amanet

A small asynchronous HTTP client written in C++20 using Boost.Asio and
Boost.Beast. It is intended as a learning project for DNS resolution, TCP
connections, asynchronous I/O, and HTTP message parsing.

## Current scope

- HTTP over plain TCP
- `GET`, `POST`, and `PUT` methods through the C++ client API
- HTTP/1.0 and HTTP/1.1 requests
- Asynchronous hostname resolution, connection, writing, and response reading

HTTPS/TLS is not supported yet. The command-line program currently sends
requests without a body; request bodies can be supplied through the C++ API.

## Requirements

- C++20-compatible compiler
- CMake 3.20 or newer
- Boost with the `system` and `thread` components

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Usage

```bash
./build/amanet example.com 80 / GET
./build/amanet example.com 80 / GET 1.0
```

The command-line arguments are:

```text
amanet <host> <port> <target> <request_verb> [1.0|1.1]
```

## Sending a body

The `amanet_client` API accepts an already-serialized body. JSON is one
possible format, but the client does not serialize C++ objects automatically:

```cpp
client->run(
    "localhost",
    "8080",
    "/users",
    11,
    http::verb::post,
    R"({"name":"Paun","age":30})",
    "application/json"
);
```

## Project status

This project is under active development. Planned improvements include HTTPS
support, command-line request bodies, and tests.
