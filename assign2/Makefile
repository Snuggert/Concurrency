PROGNAME = assign2_1_2

IMAGEVIEW = display
CC = gcc

CFLAGS =  -fopenmp -std=c99 -ggdb -O2 $(WARNFLAGS) -D_POSIX_C_SOURCE=200112
LFLAGS = -lm -lpthread

# Do some substitution to get a list of .o files from the given .c files.

.PHONY: all run runlocal plot clean dist todo

all: $(PROGNAME)

$(PROGNAME): $(PROGNAME).c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(PROGNAME)
	./$(PROGNAME)

clean:
	rm -fv $(PROGNAME)
