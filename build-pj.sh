#!/bin/bash

fvad_dir="$(pwd)/fvad"
opus_dir="$(pwd)/opus"
ssl_dir="$(pwd)/openssl"
echo $fvad_dir
echo $opus_dir
echo $ssl_dir
cd pjproject
make clean
./configure CXXFLAGS="-std=c++17 -Wall -fPIC" CFLAGS="-Wall -fPIC" LIBS="-lfvad -lopus -lssl -lcrypto" LDFLAGS="-L$fvad_dir/src/.libs -L$opus_dir/.libs -L$ssl_dir"
make dep
make
