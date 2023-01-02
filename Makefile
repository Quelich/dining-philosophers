all: phsp

phsp: phsp.o
	gcc phsp.o -o phsp

phsp.o: phsp.c
	gcc -c phsp.c

clean:
	rm *.o phsp