#!/bin/sh
set -eu

VER="$(cat VERSION)"
MAJOR="$(cut -d . -f 1 VERSION)"
MINOR="$(cut -d . -f 2 VERSION)"

if [ "$(uname -s)" = Darwin ]
then
	SO=dylib
else
	SO=so
fi

if [ $# -lt 1 ]
then
	DEST=/usr/local/lib
else
	DEST=$1
fi

if [ ! -d "$DEST" ]
then
	printf "ERROR: Destination directory '%s' does not exist\n" "$DEST"
	exit 1
fi

if [ ! -f "build/release/libderp.$SO" ]
then
	echo ERROR: Run make first to build the shared library
	exit 1
fi

if [ "$SO" = dylib ]
then
	cp build/release/libderp.dylib "$DEST/libderp.$MAJOR.dylib"
else
	cp build/release/libderp.so "$DEST/libderp.so.$VER"

	# soname symlinks
	i=0
	while [ $i -le "$MINOR" ]; do
		ln -fs "libderp.so.$VER" "$DEST/libderp.so.$MAJOR.$i"
		i=$((i+1))
	done
fi

echo "Libderp $VER installed to $DEST."
