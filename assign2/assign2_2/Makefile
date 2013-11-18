PROGNAME = assign2_1_2

IMAGEVIEW = display
CC = gcc

CFLAGS =  -fopenmp -std=c99 -ggdb -O2 $(WARNFLAGS) -D_POSIX_C_SOURCE=200112
LFLAGS = -lm -lrt -lpthread

# Do some substitution to get a list of .o files from the given .c files.

all: $(PROGNAME)

$(PROGNAME): $(PROGNAME).c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

run: $(PROGNAME)
	./$(PROGNAME)

clean:
	rm -fv $(PROGNAME)
