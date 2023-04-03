#!/bin/bash

set -e

if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make -j32
	
cd ..


