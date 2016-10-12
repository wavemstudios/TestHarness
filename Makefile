CC=gcc

all:executable

debug: CC += -g -DDEBUG
debug: executable

executable: apduClient.c
	$(CC) apduClient.c -o TestHarness

.PHONY: clean
clean:
	rm -f apduClient 
