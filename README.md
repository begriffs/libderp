## libderp, yet another C collection library

Why you might use it

* builds with any variant of make, any C99 compiler
* a proper library that you can link against
* no abuse of translation units, no weird macros
* includes proper man pages (TODO)
* shared and static library
* pkg-config interface (TODO)
* available through OS package manager (TODO)
* developer friendly ISC license

Why you might avoid it

* containers use void pointers, e.g. no vector of ints
* pedestrian algorithms, not cutting edge
* hard-coded to use malloc/free
* not (yet) thread safe

### Building

To make optimized object files in `build/release`, simply run:

```sh
# works with any make variant, and any C99 compiler
make
```

To build in `build/dev` with warnings, profiling, debugging information, and
code coverage data, use the "dev" variant:

```sh
# requires clang specifically
make VARIANT=dev
```

Object files for both variants can coexist. There is no `make clean` target
because there shouldn't be a need to do that manually. The Makefile has an
accurate dependency graph, so running `make` should always know what to update.

### Running tests

```sh
make VARIANT=dev tests

./build/dev/test/run
```

On platforms where Clang supports memory leak checks, you can activate them
like this:

```sh
ASAN_OPTIONS=detect_leaks=1 ./build/dev/test/run
```

To see test coverage for a data structure, run the cov script:

```sh
./build/dev/test/cov hashmap
```
