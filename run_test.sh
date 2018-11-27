#!/bin/bash

make all

./fractal 500 500 200 test1.bmp
./fractal 800 800 400 test2.bmp

./fractal_omp 500 500 200 test3.bmp
./fractal_omp 800 800 400 test4.bmp

./fractal_pthreads 500 500 200 test5.bmp 10
./fractal_pthreads 800 800 400 test6.bmp 10

make clean
