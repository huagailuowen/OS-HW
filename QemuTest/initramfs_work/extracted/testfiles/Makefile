CC = gcc
CFLAGS = -static

all: test

test: test.c impl.c
    $(CC) $(CFLAGS) -o test test.c impl.c

clean:
    rm -f test test_mmap.txt