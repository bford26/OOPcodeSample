
CC = g++

CXXFLAGS = -std=c++17 # -Wall -Werror -Wextra -pedantic
 
LIBS  = -lstdc++fs
LIBS += -luser32
LIBS += -lgdi32
LIBS += -lopengl32
LIBS += -lgdiplus
LIBS += -lShlwapi

# Used only with MINGW if required
# LIBS += -lmingw32 
# LIBS += -lglu32 
# LIBS += -mwindows
# LIBS += -L/mingw64/include

# linux based
LIBS += -lX11 -lGL -lpthread -lpng -lstdc++fs

# gets a list of all subsequent dirs while excluding the object folders
DIRS  = $(patsubst %/, %, $(filter-out obj/, $(filter %/, $(shell ls -F))))

# finds all source files (*.cpp) 
SRCS  = $(wildcard *.cpp)
# SRCS += $(foreach dir, $(DIRS), $(patsubst $(dir)/%.cpp, %.cpp, $(wildcard $(dir)/*.cpp)))

# gets a list of all dependencies in current and subsequent dirs
INC  = $(wildcard *.h) 
# INC += $(foreach dir, $(DIRS), $(wildcard ./$(dir)/*.h))

# makes list of object names & path to obj dir
OBJS = $(patsubst %.cpp, ./../../bin/%.o, $(SRCS))

compile: $(OBJS) $(INC)

./../../bin/%.o: %.cpp $(INC)
	$(CC) $(CXXFLAGS) -c $< -o $@ $(LIBS)

clean:
	rm -f ./obj/*.o 