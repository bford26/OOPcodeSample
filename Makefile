# DIRS for code that potentially needs compiling
DIRS  = ./ext
DIRS += ./src


# HERE WE TAKE ALL *.o file to create the executable!

EXT_DIRS = $(patsubst %/, %, $(filter-out obj/, $(filter %/, $(shell ls -F ./ext/))))
SRC_DIRS = $(patsubst %/, %, $(filter-out obj/, $(filter %/, $(shell ls -F ./src/))))

OBJS  = $(foreach dir, $(EXT_DIRS), $(shell cd ./ext/$(dir)/obj/ && ls *.o))
OBJS += $(foreach dir, $(SRC_DIRS), $(shell cd ./src/$(dir)/obj/ && ls *.o))


EXT_INC = $(foreach dir, $(EXT_DIRS), $(wildcard ./ext/$(dir)/obj/*.o))
SRC_INC = $(foreach dir, $(SRC_DIRS), $(wildcard ./src/$(dir)/obj/*.o))
INC = $(EXT_INC) $(SRC_INC)

CC = g++

CXXFLAGS = -std=c++17

LIBS  = -lstdc++fs
LIBS += -luser32
LIBS += -lgdi32
LIBS += -lopengl32
LIBS += -lgdiplus
LIBS += -lShlwapi


# Targets

all: compile

compile:
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) compile; fi;); \
	done	

	$(CC) $(INC) -o demo $(LIBS)


clean:
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) clean; fi;); \
	done

	rm -f demo

# docs:
	# doxygen
