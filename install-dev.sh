#!/bin/sh
set -eu

VER="$(cat VERSION)"

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
PREFIX="$DEST/libderp-dev-$VER"

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

mkdir -p "$PREFIX/include" "$PREFIX/lib/static" "$PREFIX/man"

cp -R include/derp "$PREFIX/include"
cp build/release/*."$SO" "$PREFIX/lib"
cp build/release/*.a  "$PREFIX/lib/static"

# m4 is not universally available...posix scofflaws
sed -e "s%PREFIX%$PREFIX%" -e "s%VERSION%$VER%" -e "s%LIBDIR%lib%" \
	libderp.pc > "$PREFIX/libderp.pc"
sed -e "s%PREFIX%$PREFIX%" -e "s%VERSION%$VER%" -e "s%LIBDIR%lib/static%" \
	libderp.pc > "$PREFIX/libderp-static.pc"

echo "Libderp $VER installed."
echo
printf "To use with pkg-config\n\tadd %s to PKG_CONFIG_PATH\n" "$PREFIX"
printf "To view man pages\n\tadd %s/man to MANPATH\n" "$PREFIX"
