all: phsp 

phsp: phsp.o
	gcc phsp.o -o phsp -lm

phsp.o: phsp.c
	gcc -c phsp.c -lm

clean:
	rm *.o phsp 