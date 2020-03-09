#!/bin/bash

fvad_dir="$(pwd)/fvad"
opus_dir="$(pwd)/opus"
ssl_dir="$(pwd)/openssl"
echo $fvad_dir
echo $opus_dir
echo $ssl_dir

cd pjproject
./configure --disable-video CXXFLAGS="-std=c++17 -Wall -g -DDEBUG -fPIC" CFLAGS="-g -DDEBUG -fPIC" LDFLAGS="-lfvad -lopus"
make dep
make
