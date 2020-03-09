#!/usr/bin/env bash

export CFLAGS="-fPIC -Wall"
export CPPFLAGS="-fPIC -Wall"
fvad_dir="$(pwd)/fvad"
opus_dir="$(pwd)/opus"
ssl_dir="$(pwd)/openssl"
opencore_amr_dir="$(pwd)/opencore-amr"
cd opencore-amr || exit
autoreconf -i
./configure --enable-static --disable-shared CXXFLAGS="-Wall -fPIC" CFLAGS="-Wall -fPIC"
make
cd ../fvad || exit
autoreconf -i
./configure --enable-static --disable-shared CXXFLAGS="-Wall -fPIC" CFLAGS="-Wall -fPIC"
make
cd ../opus || exit
./configure --enable-static --disable-shared CXXFLAGS="-Wall -fPIC" CFLAGS="-Wall -fPIC"
make
cd ../openssl || exit
export CFLAGS="-fPIC -Wall"
./config
make
cd ../pjproject || exit
./configure --enable-epoll --with-opencore-amr=$opencore_amr_dir --with-opus=$opus_dir --with-ssl=$ssl_dir CXXFLAGS="-Wall -fPIC" CFLAGS="-Wall -fPIC"
make clean
make dep && make