CC = g++
CFLAGS 	= -fPIC -c -Wall -I ./include/robo-eeg -std=c++11
LDFLAGS = -shared

EmotivePath = /home/svetlin/EmotivResearch_2.0.0.20

#Include Libs
#include Emotitve SDK
CFLAGS      += -I $(EmotivePath)/include
LDFLAGS     += -L $(EmotivePath)/lib -ledk

#include OpenCV
CFLAGS 		+= $(shell pkg-config opencv --cflags)
LDFLAGS 	+= $(shell pkg-config opencv --libs)

SOURCES		= $(wildcard ./src/*.cpp)

OBJECTS		= $(SOURCES:.cpp=.o)

EXECUTABLE 	= librobo-eeg.so

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(CURDIR)/$< -o $@

install:
	ln -s `pwd`/librobo-eeg.so /usr/lib/librobo-eeg.so
clean:
	rm -rf ./src/*.o $(EXECUTABLE)

















