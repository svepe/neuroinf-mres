CC = g++
CFLAGS 	= -c -Wall -I ./include/ -std=c++11
LDFLAGS =

#Include Libs
#include RoboEEG
RoboEEGPath = ./../robo-eeg
CFLAGS      += -I $(RoboEEGPath)/include
LDFLAGS     += -L $(RoboEEGPath)/ -lrobo-eeg

#include Emotiv
EmotivePath = /home/$(USER)/EmotivResearch_2.0.0.20
CFLAGS      += -I $(EmotivePath)/include
LDFLAGS     += -L $(EmotivePath)/lib -ledk

#include OpenCV
CFLAGS 		+= $(shell pkg-config opencv --cflags)
LDFLAGS 	+= $(shell pkg-config opencv --libs)

SOURCES		= $(wildcard ./src/*.cpp)

OBJECTS		= $(SOURCES:.cpp=.o)

EXECUTABLE 	= flanker

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(CURDIR)/$< -o $@

clean:
	rm -rf ./src/*.o $(EXECUTABLE)

















