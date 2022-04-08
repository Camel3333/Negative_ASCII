CC = gcc -Wall

all: clean build test

clean:
	rm -f *.o

build:
	$(CC) -o main main.c -lpthread -lm

test:
	./main 1 numbers surf.ascii.pgm negative_surf.ascii.pgm
	./main 2 numbers surf.ascii.pgm negative_surf.ascii.pgm
	./main 4 numbers surf.ascii.pgm negative_surf.ascii.pgm
	./main 8 numbers surf.ascii.pgm negative_surf.ascii.pgm
	./main 16 numbers surf.ascii.pgm negative_surf.ascii.pgm
	./main 1 block surf.ascii.pgm negative_surf.ascii.pgm
	./main 2 block surf.ascii.pgm negative_surf.ascii.pgm
	./main 4 block surf.ascii.pgm negative_surf.ascii.pgm
	./main 8 block surf.ascii.pgm negative_surf.ascii.pgm
	./main 16 block surf.ascii.pgm negative_surf.ascii.pgm