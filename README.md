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

Compile and install the library:

```sh
# detect proper way to generate shared library
./configure

# create static and shared libs in build/release
make

# create versioned directory for headers and libs
# (if path is omitted, it tries /opt and /usr/local in that order)
./install.sh /path
```

The result will be a new folder and symbolic link like this:

```
/path/libderp.x -> /path/libderp.x.y.z
/path/libderp.x.y.z
├── include
│   └── derp
│       ├── ...
│       └── ...
├── lib
│   ├── libderp.a
│   ├── libderp.so (or dylib or dll)
│   └── pkgconfig
│       └── libderp.pc
└── man
    ├── ...
    └── ...
```

### Usage

Multiple versionf of libderp may be installed at once on the system in their
own dedicated folders. This allows everything to be versioned, including
headers and man pages.

When linking to libderp, set an rpath inside your binary to point to the
library of the appropriate major version. The easiest way is to use pkg-config
which will provide you the correct compiler/linker flags.

```sh
# make desired libderp version visible to pkg-config
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/path/libderp.x/lib/pkgconfig"

# compile foo.c and link with libderp
cc `pkg-config --cflags --libs-only-L --libs-only-other libderp` \
	-o foo foo.c `pkg-config --libs-only-l libderp`
```

The `--libs-only-other` options set the rpath for the resulting binary.

### Libderp development

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
