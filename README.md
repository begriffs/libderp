## libderp, yet another C collection library

Why you might use it

* builds with any variant of make, any C99 compiler
* a proper library that you can link against
* no abuse of translation units, no weird macros
* includes proper man pages (TODO)
* shared and static library
* pkg-config interface
* developer friendly ISC license

Why you might avoid it

* containers use void pointers, e.g. no vector of ints
* pedestrian algorithms, not cutting edge
* hard-coded to use malloc/free
* not (yet) thread safe

### Installation

Compile the shared and static libraries:

```sh
# detect proper way to generate shared library
./configure

# create static and shared libs in build/release
make
```

#### Installing development assets (for the linker)

To install libraries that an application would link with, along with headers,
and pkg-config specifications, use the `install-dev.sh` script:

```sh
# (if path is omitted, it defaults to /opt)
./install-dev.sh /path
```

The result will be a folder structure like this:

```
/path/libderp-dev.x.y.z
├── libderp.pc
├── libderp-static.pc
├── include
│   └── derp
│       ├── ...
│       └── ...
├── lib
│   ├── libderp.so (or dylib or dll)
│   └── static
│       └── libderp.a
└── man
    ├── ...
    └── ...
```

The easiest way to build against these files is to use pkg-config, which will
provide the correct compiler/linker flags.

```sh
# make desired libderp version visible to pkg-config
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/path/libderp.x.y.z"

# compile foo.c and link with libderp
cc `pkg-config --cflags --libs-only-L libderp` \
	-o foo foo.c `pkg-config --libs-only-l libderp`
```

To link statically against the library, change the pkg-config name `libderp` to
`libderp-static`. This library uses position-independent code (PIC) for its
static library. Mostly out of laziness in the build process, but it also allows
shared libraries to link a static copy of libderp inside too. The disadvantage
is that PIC causes a performance hit, especially on some older CPU
architectures.

#### Installing runtime assets (for the loader)

To install the shared library for loading, use the `install.sh` script.

```sh
# (if path is omitted, it defaults to /usr/local/lib)
./install.sh /path
```

This copies the shared library, and creates symbolic links to match the soname
that applications are built against.

### Contributing to Libderp

To build in `build/dev` with warnings, profiling, debugging information, and
code coverage data, use the "dev" variant:

```sh
# requires clang specifically
make VARIANT=dev
```

Object files for the dev and release variants can coexist. There is no `make
clean` target because there shouldn't be a need to do that manually. The
Makefile has an accurate dependency graph, so running `make` should always know
what to update.

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
