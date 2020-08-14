CC = g++

CXXFLAGS = -std=c++17 # -Wall -Werror -Wextra -pedantic

file_depth = ../../../..

# for each folder compile the files in the folder, then enter the next folder until no more folders

SUB_DIRS = $(filter %/, $(shell ls -F ./))

SRCS = $(wildcard *.cpp)

OBJS = $(patsubst %.cpp, ./$(file_depth)/bin/%.o, $(SRCS))

UPPER_INC  = -I../
UPPER_INC += -I../../

EXT_DIRS = $(patsubst %/, %, $(filter %/, $(shell ls -F ./$(file_depth)/include)))
EXT_INC = $(patsubst %, -I$(file_depth)/include/%, $(EXT_DIRS))



# ======================================


compile: $(OBJS)

	@for dir in $(SUB_DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) compile; fi;); \
	done

./$(file_depth)/bin/%.o: %.cpp %.h#$()
	$(CC) $(CXXFLAGS) $(UPPER_INC) $(EXT_INC) -c $< -o $@


clean:
	rm -f ./$(file_depth)/bin/*.o


test:
	@echo $()