#!/usr/bin/env bash

fvad_dir="$(pwd)/fvad"
opus_dir="$(pwd)/opus"
ssl_dir="$(pwd)/openssl"
opencore_amr_dir="$(pwd)/opencore-amr"
cd pjproject
make clean
./configure --enable-shared --with-opencore-amr=$opencore_amr_dir --with-opus=$opus_dir --with-ssl=$ssl_dir CXXFLAGS="-std=c++17 -Wall -fPIC" CFLAGS="-Wall -fPIC"
make dep
make