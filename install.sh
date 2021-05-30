#!/bin/sh
set -eu

MAJOR_VER=0
MINOR_VER=0
PATCH_VER=0

if [ $# -lt 1 ]
then
	if [ -d /opt ]
	then
		DEST=/opt
	else
		DEST=/usr/local
	fi
else
	DEST=$1
fi

if [ ! -d "$DEST" ]
then
	printf "ERROR: Destination directory '%s' does not exist\n" "$DEST"
	exit 1
fi
PREFIX="$DEST/libderp.$MAJOR_VER.$MINOR_VER.$PATCH_VER"

if [ -d "$PREFIX" ]
then
	printf "ERROR: Directory already exists: %s\n" $PREFIX
	echo Remove it and run script again to reinstall.
	exit 1
fi

if [ ! -f build/release/libderp.so ] || [ ! -f build/release/libderp.a ]
then
	echo ERROR: Run make first to build the shared library
	exit 1
fi

mkdir -p "$PREFIX/include" "$PREFIX/lib/pkgconfig" "$PREFIX/man"
rm -f "$DEST/libderp"
ln -s "$PREFIX" "$DEST/libderp"

cp -R include/derp "$PREFIX/include"
cp build/release/*.so build/release/*.a "$PREFIX/lib"

m4 libderp.pc -D PREFIX="$PREFIX" \
	-D MAJOR="$MAJOR_VER" -D MINOR="$MINOR_VER" -D PATCH="$PATCH_VER" \
	> "$PREFIX/lib/pkgconfig/libderp.pc"

echo "Libderp $MAJOR_VER.$MINOR_VER.$PATCH_VER installed."
echo
echo "To use with pkg-config\n\tadd $DEST/libderp/lib/pkgconfig to PKG_CONFIG_PATH"
echo "To view man pages\n\tadd $DEST/libderp/man to MANPATH"
