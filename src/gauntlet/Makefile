

CC = g++

CXXFLAGS = -std=c++17 # -Wall -Werror -Wextra -pedantic

file_depth = ../..

# for each folder compile the files in the folder, then enter the next folder until no more folders

# SUB_DIRS = $(filter %/, $(shell ls -F ./))


# tile,
# 	dynamic,
# 		entity,
# 			mob, player
# 		projectile

# 	static,
# 		interactionBlock

includes  = ./../tile/
includes += ./../tile/dynamic/
includes += ./../tile/dynamic/entity/
includes += ./../tile/dynamic/entity/player/
includes += ./../tile/dynamic/entity/mob/
includes += ./../tile/dynamic/proj/
includes += ./../tile/static/
includes += ./../tile/static/interactionBlock/
includes += ./../resources/

INC = $(patsubst %, -I%, $(includes))


SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, ./$(file_depth)/bin/%.o, $(SRCS))

# UPPER_INC  = -I../
# UPPER_INC += -I../../

EXT_DIRS = $(patsubst %/, %, $(filter %/, $(shell ls -F ./$(file_depth)/include)))
EXT_INC = $(patsubst %, -I$(file_depth)/include/%, $(EXT_DIRS))




# =======================================


compile: $(OBJS)

./$(file_depth)/bin/%.o: %.cpp %.h#$()
	$(CC) $(CXXFLAGS) $(INC) $(EXT_INC) -c $< -o $@

clean:
	rm -f ./$(file_depth)/bin/*.o
