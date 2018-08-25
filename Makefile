OBJS1	= sorted.o test.o
OUT1	= test
CC		= gcc
FLAGS	= -g -Wall #-std=c99

UNAME := $(shell uname -p)
ifeq ($(UNAME),i686)
	LIB = BF_32.a
else
	LIB = BF_64.a
endif

all: $(OUT1)

test: sorted.o test.o
	$(CC) $(FLAGS) $(OBJS1) -o $(OUT1) -L. $(LIB) -lm

sorted.o: sorted.c
	$(CC) $(FLAGS) -c sorted.c
test.o: test.c
	$(CC) $(FLAGS) -c test.c

# clean up
clean:
	rm -f $(OBJS1) $(OUT1)
