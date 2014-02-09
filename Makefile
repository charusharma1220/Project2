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
    #openGL
    #	-L/user/X11/lib
  INCLUDE =
  LIBS =
    #openGL:
    #	-framework OpenGL -framework GLUT
endif

threads.o: threads.c threads.h
	${CC} -c ${CFLAGS} $(INCLUDE) threads.c
main.o: main.c threads.o threads.h
	${CC} -c ${CFLAGS} $(INCLUDE) main.c
Threads: $(allo)
	${CC} ${CFLAGS} $(INCLUDE) -o Threads $(allo) ${LIBDIR} ${LIBS}
clean: 
	rm $(allo)
