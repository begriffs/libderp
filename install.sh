#!/bin/sh
set -eu

VER="$(cat VERSION)"
MAJOR_VER="$(cut -d . -f 1 VERSION)"

if [ "$(uname -s)" = Darwin ]
then
	SO=dylib
else
	SO=so
fi

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
PREFIX="$DEST/libderp.$VER"
SYM="$DEST/libderp.$MAJOR_VER"

if [ -d "$PREFIX" ]
then
	printf "ERROR: Directory already exists: %s\n" "$PREFIX"
	echo Remove it and run script again to reinstall.
	exit 1
fi

if [ ! -f "build/release/libderp.$SO" ] || [ ! -f build/release/libderp.a ]
then
	echo ERROR: Run make first to build the shared library
	exit 1
fi

mkdir -p "$PREFIX/include" "$PREFIX/lib/pkgconfig" "$PREFIX/man"
rm -f "$SYM"
ln -s "$PREFIX" "$SYM"

cp -R include/derp "$PREFIX/include"
cp build/release/*."$SO" build/release/*.a "$PREFIX/lib"

# m4 is not universally available...posix scofflaws
sed -e "s%PREFIX%$SYM%" -e "s%VERSION%$VER%" \
	libderp.pc > "$PREFIX/lib/pkgconfig/libderp.pc"

echo "Libderp $VER installed."
echo
printf "To use with pkg-config\n\tadd %s/lib/pkgconfig to PKG_CONFIG_PATH\n" "$SYM"
printf "To view man pages\n\tadd %s/man to MANPATH\n" "$SYM"
