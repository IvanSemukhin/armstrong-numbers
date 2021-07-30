CC=g++
CFLAGS=-O3 -pthread

all:
	$(CC) $(CFLAGS) main.cpp -o armstrong_numbers

clean:
	rm -rf *.o armstrong_numbers
