# Libderp changelog

## 1.1.0

### Added

* Support for alternate memory allocation functions
* Memory leak checking in tests with the Boehm GC

## 1.0.0

### Added

* `EXTRA_CFLAGS` for cross-compilation
* Include debug symbols in the release variant too

### Changed

* Don't explicitly build the static library with
  position-independent code (-fPIC)

## 0.1.0

### Added

* `derp_strcmp` and `derp_free` convenience wrappers which
  ignore their second argument

### Fixed

* Conform with C99 7.1.3 for internal identifier names
  (don't use leading underscores)

## 0.0.0

### Added

* List, vector, balanced tree, and hash map
