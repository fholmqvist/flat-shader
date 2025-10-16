#!/bin/sh

set -e

cd ./build/
ninja
cd ..
./flat_shader
