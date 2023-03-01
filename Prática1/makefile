CC = gcc
CFLAGS = -O2 -Wall
HEADERS = includes/person.h
DEPS = programs/person.o

.PHONY: all clean
all: pessoas

pessoas: $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f pessoas programs/*.o