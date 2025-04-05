# 🐸 FROG — Fast Recompilation and Object Generation

- **Author:** Hugo Coto Florez
- **Standard:** C11
- **License:** Licenseless
- **Inspired by:** [`nob.h`](https://github.com/tsoding/nob.h) by [@tsoding](https://github.com/tsoding)



## Overview

**FROG** is a lightweight, header-only C library designed to streamline the build process of small to medium C projects. It provides tools for:

- Filtering source files using regular expressions
- Executing batch compilation commands
- Asynchronous command execution
- Simple self-recompilation logic

FROG is not a compiler nor a full build system — it's a **minimalist C-native build assistant** that integrates easily into your codebase.



## Installation

Simply include the `frog.h` file in your project. FROG is a single-header library with an optional implementation block.

```c
#define FROG_IMPLEMENTATION
#include "frog.h"
```

You also need [`da.h`](https://github.com/tsoding/nob.h/blob/master/da.h), a simple dynamic array utility header.



## Quickstart

Here’s a minimal example of how to use FROG to compile all `.c` files in a `src/` directory and link them into an executable:

```c
#define FROG_IMPLEMENTATION
#include "frog.h"

int main(int argc, char *argv[]) {
    frog_da_str src = {0};

    frog_rebuild_itself(argc, argv);                          // Optional: Self-rebuild if needed
    frog_filter_files(&src, "./src", ".*\\.c");               // Filter C source files
    frog_cmd_foreach(src, "gcc", "-c", NULL);                 // Compile each file with gcc -c
    frog_cmd_wait("gcc", "main.o", "-o", "program", NULL);    // Link main.o into executable

    return 0;
}
```

Build and run:

```sh
$ gcc build.c -o build
$ ./build
```

FROG will recompile itself if its source has changed since the last build.



## API Reference

```c
int frog_filter_files(frog_da_str *out, const char *path, const char *pattern);
```

Filters files in a directory `path` matching `pattern` (POSIX regex), storing the full paths in the dynamic array `out`.



```c
void frog_cmd_foreach(frog_da_str files, const char *command, ...);
```

Executes the given `command` on each file in `files`, forking one process per call. Waits for all processes to complete.



```c
int frog_cmd_wait(const char *command, ...);
```

Executes a command synchronously and waits for its termination. Returns the exit status.



```c
int frog_cmd_async(const char *command, ...);
```

Executes a command asynchronously and returns its PID. Does not wait for termination.



```c
int frog_cmd_asyncv(const char *command, va_list fargs);
```
```c
int frog_cmd_asyncl(const char *command, char *args[]);
```

Lower-level interfaces for asynchronous command execution using `va_list` or a `char*` argument list.



```c
int frog_is_newer(const char *file1, const char *file2);
```

Returns true if `file1` is newer than `file2`, based on file modification timestamps.



```c
frog_rebuild_itself(argc, argv);
```

Macro that checks if the source file is newer than the current executable. If so, it recompiles and relaunches the program.



## Example Output

```text
[CMD] gcc -c ./src/foo.c
[CMD] gcc -c ./src/bar.c
[CMD] gcc main.o -o program
```



## Notes

- FROG is designed for simplicity and portability — it only uses POSIX APIs.
- The library **does not handle dependency graphs** or complex build rules. It's best suited for projects with flat or simple structures.
- You may customize the logging by changing the `LOG_PRINT` macro.



## License

This project is **licenseless**. Do whatever you want, no strings attached.


