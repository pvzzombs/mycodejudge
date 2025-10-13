#!/bin/sh
g++ -std=c++11 -g -Wall -Iinclude -c main.cpp -o main.o
g++ -std=c++11 -g -Wall -Iinclude -c loguru.cpp -o loguru.o
g++ -std=c++11 -g -Wall -Iinclude -c rootrunner.cpp -o rootrunner.o
g++ -lsqlite3 -lsodium -o a.out main.o loguru.o
g++ -o rootrunner.out rootrunner.o loguru.o