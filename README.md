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

int
main(int argc, char *argv[])
{
        frog_da_str src = { 0 };
        frog_rebuild_itself(argc, argv);

        frog_filter_files(&src, "./src", ".*.c");
        frog_cmd_foreach(src, "gcc", "-c", NULL);
        frog_delete_filter(&src);

        frog_makedir("objs");
        frog_shell_cmd("mv *.o objs");
        frog_shell_cmd("gcc objs/*.o -o executable");

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
As Im not going to have this section updated, I prefer to have this empty. Read the source

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


