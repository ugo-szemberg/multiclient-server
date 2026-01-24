# Multiclient server
A Linux multi-client TCP server designed to explore low-level network programming and I/O multiplexing.

## Overview
This project is a learning-focused implementation of a network messaging server.  
It uses a non-blocking event loop to handle multiple simultaneous client connections efficiently within a single process.

## Learning Goals
- **Socket:** Master the lifecycle of TCP sockets (socket, bind, listen, accept).
- **I/O Multiplexing:** Implement and understand `poll` and `epoll` API.
- **Concurrency:** Handle multiple clients without multithreading.
- **Linux:** Gain practical experience with low-level POSIX system programming.

## Tech Stack
- Language: C
- API: POSIX Sockets
- Protocol: TCP
- Multiplexing: poll & epoll
- Build System: CMake

## Installation & Build
Prerequisites
- Linux environment (required for epoll)
- GCC or Clang
- CMake

### Author
[Ugo Szemberg](https://github.com/ugo-szemberg)