Car — Simple ASCII Animation (C)

A simple ASCII-art animation of a moving car, written in C.

Features
- Lightweight, portable C program
- Builds with common C compilers (Linux / POSIX systems)
- No Windows support currently

Supported compilers
- gcc (default)
- clang
- tcc

Building
1. Clone the repository.
2. Default build (uses gcc):
   make

To explicitly use a different compiler:
   make CC=clang
   make CC=tcc

Or use the provided make targets:
   make clang
   make tcc

Running
After building, run the produced binary (for example ./car) in a POSIX terminal.

Cleaning
Remove build artifacts:
   make clean

Notes
- There is no make install rule at this time.
- The program targets POSIX-like terminals; Windows support is not provided.


