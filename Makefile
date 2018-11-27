all: omp serial pthreads

serial: fractal
omp: fractal_omp
pthreads: fractal_pthreads

fractal_pthreads: fractal_pthreads.o bmp.o
	gcc fractal_pthreads.o bmp.o -o fractal_pthreads -lm -pthread -fopenmp
	
fractal_omp: fractal_omp.o bmp.o
	gcc fractal_omp.o bmp.o -o fractal_omp -lm -fopenmp
 
fractal: fractal.o bmp.o
	gcc fractal.o bmp.o -o fractal -lm 
	
fractal_pthreads.o: fractal_pthreads.c
	gcc -c fractal_pthreads.c -o fractal_pthreads.o -pthread -fopenmp

fractal_omp.o: fractal_omp.c
	gcc -c fractal_omp.c -o fractal_omp.o -fopenmp

fractal.o: fractal.c
	gcc -c fractal.c -o fractal.o 

bmp.o: bmp.c
	gcc -c bmp.c -o bmp.o 

clean:
	rm -f *.o fractal fractal_omp fractal_pthreads
