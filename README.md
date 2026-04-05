# Hexa.NET.Utilities.Native

The native C/C++ side of [Hexa.NET.Utilities](https://github.com/HexaEngine/Hexa.NET.Utilities), providing low-level synchronization primitives with a C-compatible API for P/Invoke interop from .NET.

## Features

- **ReaderWriterLock** — A high-performance, lock-free reader-writer lock built on C++20 atomics with:
  - Multiple concurrent readers
  - Exclusive writer access
  - Writer fairness (writers are not starved by incoming readers)
  - Try-lock variants for non-blocking acquisition
  - C-compatible ABI for use from C or via P/Invoke

## Requirements

- CMake 3.20 or later
- A C++20-capable compiler (GCC, Clang, or MSVC)
- C99 compiler support

## Building

```bash
cmake -B build -DBUILD_SHARED_LIBS=ON
cmake --build build
```

To build as a static library:

```bash
cmake -B build -DBUILD_SHARED_LIBS=OFF
cmake --build build
```

## Installation

```bash
cmake --install build
```

This installs the library to `lib/` and headers to `include/`.

## Project Structure

```
include/
  common.h           — Export macros and calling convention definitions
  ReaderWriterLock.h — ReaderWriterLock C API
  utils.h            — Umbrella header (includes all public headers)
src/
  ReaderWriterLock.cpp — C++20 atomic-based implementation
CMakeLists.txt
```

## License

MIT License — Copyright (c) 2024 Juna Meinhold. See [LICENSE.txt](LICENSE.txt) for details.
