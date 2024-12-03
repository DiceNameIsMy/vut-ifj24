#
#  Project: IFJ24
#
#  Authors:
#      - Nurdaulet Turar (nurdaulet.turar.cz@gmail.com)
#

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(wildcard *.h)

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) main

.PHONY: all clean