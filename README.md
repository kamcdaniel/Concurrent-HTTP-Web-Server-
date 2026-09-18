# Concurrent HTTP Web Server

## Overview

A simple HTTP web server in C based on an echo server. The server supports multiple simultaneous clients by using `fork()` to create a child process for each client request.

## Features

* Accepts multiple client connections concurrently using `fork()`
* Uses `SIGCHLD` signal handling to clean up terminated child processes
* Uses `waitpid()` with `WNOHANG` to prevent the parent process from blocking
* Implements a subset of HTTP supporting `GET` requests
* Reads and returns the contents of requested files
* Includes a corresponding HTTP client
* Uses a Makefile for compilation
* Designed to produce a clean Valgrind report

## HTTP Functionality

The server accepts requests in the following format:

```text
GET filename
```

For example:

```text
GET source.txt
```

If the requested file exists, the server sends its contents back to the client.

## Technologies

* C
* UNIX/Linux system calls
* TCP sockets
* `fork()`
* Signals (`SIGCHLD`)
* `waitpid()`
* HTTP
* Valgrind
* Make
