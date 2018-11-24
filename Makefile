all: omp serial

serial: fractal
omp: fractal_omp

fractal_omp: fractal_omp.o bmp.o
	gcc fractal_omp.o bmp.o -o fractal_omp -lm -fopenmp
 
fractal: fractal.o bmp.o
	gcc fractal.o bmp.o -o fractal -lm 

fractal_omp.o: fractal_omp.c
	gcc -c fractal_omp.c -o fractal_omp.o -fopenmp

fractal.o: fractal.c
	gcc -c fractal.c -o fractal.o 

bmp.o: bmp.c
	gcc -c bmp.c -o bmp.o 

clean:
	rm -f *.o fractal
