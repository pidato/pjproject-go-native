#!/usr/bin/env bash

fvad_dir="$(pwd)/fvad"
opus_dir="$(pwd)/opus"
ssl_dir="$(pwd)/openssl"
opencore_amr_dir="$(pwd)/opencore-amr"
cd opencore-amr || exit
autoreconf -i
./configure --enable-static --enable-shared CXXFLAGS="-Wall -fPIC" CFLAGS="-Wall -fPIC"
make clean
make
cd ../
cd fvad || exit
autoreconf -i
./configure --enable-static --enable-shared CXXFLAGS="-Wall -fPIC" CFLAGS="-Wall -fPIC"
make clean
make
cd ../
cd opus || exit
autoreconf -i
./configure --enable-static --enable-shared CXXFLAGS="-Wall -fPIC" CFLAGS="-Wall -fPIC"
make clean
make
cd ../
cd openssl || exit
export CFLAGS="-fPIC -Wall"
./config
make clean
make
cd ../
cd pjproject
./configure --enable-shared --with-opencore-amr=$opencore_amr_dir --with-opus=$opus_dir --with-ssl=$ssl_dir CXXFLAGS="-std=c++17 -Wall -fPIC" CFLAGS="-Wall -fPIC"
make clean
make dep && make
cd ../
cmake .
make
sh assemble-mac.sh