	# Variables #
CC = gcc		#gcc/g++
CFLAGS = -Wall
INCLUDE =
LIBDIR =
LIBS =
allo = main.o threads.o 

	# OSX compatibility #
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  CC =
  CFLAGS = -D__MAC__
  LIBDIR =
  INCLUDE =
  LIBS =
endif

Threads: $(allo)
	${CC} ${CFLAGS} $(INCLUDE) -o Threads $(allo) ${LIBDIR} ${LIBS}
main.o: main.c threads.o threads.h
	${CC} -c ${CFLAGS} $(INCLUDE) main.c
threads.o: threads.c threads.h
	${CC} -c ${CFLAGS} $(INCLUDE) threads.c
clean: 
	rm $(allo)
	rm Threads
