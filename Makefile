INCLUDE = -I/usr/include/
LIBDIR  = -L/usr/X11R6/lib 

COMPILERFLAGS = -Wall
CC = g++
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lX11 -lXi -lglut -lGL -lGLU -lm -lpthread
  

all: clean main


main : 
	$(CC) $(CFLAGS) main.cpp imageloader.cpp  -o main   $(LIBDIR) $< $(LIBRARIES)  

clean:
	rm -rf *.o main

