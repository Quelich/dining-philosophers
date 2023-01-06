all: phsp 

phsp: phsp.o
	gcc phsp.o -o phsp -lm -lpthread

phsp.o: phsp.c
	gcc -c phsp.c -lm -lpthread

clean:
	rm *.o phsp 