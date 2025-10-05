#!/bin/sh
g++ -std=c++11 -g -Wall -Iinclude -c main.cpp -o main.o
g++ -std=c++11 -g -Wall -Iinclude -c loguru.cpp -o loguru.o
g++ -lsqlite3 -lsodium -o a.out main.o loguru.o