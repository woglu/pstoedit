#!/bin/sh
set -e
if [ ! -f /usr/share/aclocal/pkg.m4 ]; then
	echo "Your system is missing /usr/share/aclocal/pkg.m4"
	echo "Please install the pkg-config package"
	exit 1
fi

if [ ! -f /usr/share/aclocal/ax_cxx_compile_stdcxx_0x.m4 ]; then
	echo "Your system is missing /usr/share/aclocal/ax_cxx_compile_stdcxx_0x.m4"
	echo "Please install the autoconf-archive package"
	exit 1
fi

LANG=C
export LANG
autoreconf --force --install --warnings=all $*
