CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGETS = 
OBJS = 

ifneq (,$(wildcard client.c))
	TARGETS += client
	OBJS += client.o
endif

ifneq (,$(wildcard server.c))
	TARGETS += server
	OBJS += server.o
endif

.PHONY: all clean

all: $(TARGETS)

client: client.o
	$(CC) $(CFLAGS) -o $@ $^

server: server.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)