#! /usr/bin/env bash

g++ src/main.cpp -o main $(pkg-config --cflags --libs gstreamer-1.0) -std=c++11
./main