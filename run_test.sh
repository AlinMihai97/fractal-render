#!/bin/bash

make all &> /dev/null

echo "Serial implementation------"
./fractal 400 400 200 test1.bmp
./fractal 400 400 400 test2.bmp
./fractal 800 800 400 test3.bmp
echo ""
echo "OMP implementation---------"
./fractal_omp 400 400 200 test4.bmp
./fractal_omp 400 400 400 test5.bmp
./fractal_omp 800 800 400 test6.bmp
echo ""
echo "Pthreads implementation----"
./fractal_pthreads 400 400 200 test7.bmp 8
./fractal_pthreads 400 400 400 test8.bmp 8
./fractal_pthreads 800 800 400 test9.bmp 8

make clean &> /dev/null
