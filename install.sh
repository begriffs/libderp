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
	printf "ERROR: Directory already exists: %s\n" "$PREFIX"
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

# m4 is not universally available...posix scofflaws
sed -e "s%PREFIX%$PREFIX%" \
	-e "s%MAJOR%$MAJOR_VER%" -e "s%MINOR%$MINOR_VER%" -e "s%PATCH%$PATCH_VER%" \
	libderp.pc > "$PREFIX/lib/pkgconfig/libderp.pc"

echo "Libderp $MAJOR_VER.$MINOR_VER.$PATCH_VER installed." 
echo
printf "To use with pkg-config\n\tadd %s/libderp/lib/pkgconfig to PKG_CONFIG_PATH\n" "$DEST"
printf "To view man pages\n\tadd %s/libderp/man to MANPATH\n" "$DEST"
