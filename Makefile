build: fractal

fractal: fractal.o bmp.o
	gcc fractal.o bmp.o -o fractal -lm 

fractal.o: fractal.c
	gcc -c fractal.c -o fractal.o 

bmp.o: bmp.c
	gcc -c bmp.c -o bmp.o 

clean:
	rm -f *.o fractal
