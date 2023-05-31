#!/usr/bin/env bash

if [ ! -d "cmake-build-debug" ];then
  mkdir cmake-build-debug
fi
cd cmake-build-debug
cmake ..
make

cp src/client/bt-client .
cp src/server/bt-server .
