CC = gcc
CFLAGS = -Wall -Wextra -g

PROGRAM = escape_abyss

SOURCES = abyss.c
HEADERS = items.h text.h

OBJECTS = $(SOURCES:.c=.o)

$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(PROGRAM)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(PROGRAM)
	./$(PROGRAM)

clean:
	rm -f $(PROGRAM) $(OBJECTS)

.PHONY: all clean run