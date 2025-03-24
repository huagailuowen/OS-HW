CC = gcc
CFLAGS = -static
LDFLAGS = -lpthread

all: test_basic test_concurrent

test_basic: 
	$(CC) $(CFLAGS) -o test_kv test_kv.c 

clean:
	rm -f test_basic test_concurrent

