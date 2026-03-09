# Car — Simple ASCII Animation (C)

### A simple ASCII-art animation of a moving car, written in C.

## Features
- Lightweight, portable C program
- Audio Playback using SDL2 
- Builds with common C compilers (Linux / POSIX systems)
- No Windows support currently

## Supported compilers
- gcc (default)
- clang


## Dependencies

This project requires the following libraries and headers:

- **C Standard Libraries**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
  - `<unistd.h>`
  - `<sys/ioctl.h>`
  - `<termios.h>`
  - `<fcntl.h>`
  - `<errno.h>`
  - `<time.h>`
  - `<signal.h>`
  - `<pthread.h>`

- **SDL2 Library**
  - `<SDL2/SDL.h>`

Make sure to install these dependencies to compile and run the project successfully.


## Building
1. Clone the repository.
2. Default build (uses gcc):
   make

To explicitly use a different compiler:
   make CC=clang
   make CC=tcc

Or use the provided make targets:
   make clang
   make tcc

## Running
After building, run the produced binary (for example ./car) in a POSIX terminal.

## Cleaning
Remove build artifacts:
   make clean

## Notes
- There is no make install rule at this time.
- The program targets POSIX-like terminals; Windows support is not provided.

This project includes various audio files that enhance the user experience. For detailed information about the audio files used in this project, please refer to the `CREDITS.md` file.

## License

This project is distributed under the MIT License. See the LICENSE file for details.
