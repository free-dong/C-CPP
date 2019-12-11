#!/bin/sh

rm -rf ./bin/*
cd ./build/
cmake ..
make -j8


cd ../bin/
./detectHuman 

cd ..

