# compiler option
CC = g++

# c++ version and error checking
CXXFLAGS = -std=c++17 # -Wall -Werror -Wextra -pedantic
 
#  required LIBS for olcPGEX
LIBS  = -lstdc++fs
LIBS += -luser32
LIBS += -lgdi32
LIBS += -lopengl32
LIBS += -lgdiplus
LIBS += -lShlwapi

# Used only with MINGW if required
# LIBS += -lmingw32  
# LIBS += -mwindows
# LIBS += -L/mingw64/lib

# gets a list of all subsequent dirs while excluding the object folders
DIRS  = $(patsubst %/, %, $(filter-out obj/, $(filter %/, $(shell ls -F))))

# finds all source files (*.cpp) 
SRCS  = $(wildcard *.cpp)
SRCS += $(foreach dir, $(DIRS), $(patsubst $(dir)/%.cpp, %.cpp, $(wildcard $(dir)/*.cpp)))

# gets a list of all dependencies in current and subsequent dirs
INC  = $(wildcard *.h) 
INC += $(foreach dir, $(DIRS), $(wildcard ./$(dir)/*.h))

# makes list of object names & path to obj dir
OBJS = $(patsubst %.cpp, ./obj/%.o, $(SRCS))

LOCAL_DIRS = $(patsubst %/, %, $(filter-out main/, $(filter %/, $(shell ls -F ..))))
LOCAL_INC = $(patsubst %, -I../%, $(LOCAL_DIRS))

# Gets the external dependencies dirs, this case olcPGEX 
EXT_DIRS = $(patsubst %/, %, $(filter-out obj/, $(filter %/, $(shell ls -F ../../ext/)))) 

# includes the path to the external dependencies
EXT_INC = $(patsubst %, -I../../ext/%, $(EXT_DIRS))


compile: $(OBJS) $(INC)

./obj/%.o: %.cpp $(INC)
	$(CC) $(CXXFLAGS) $(LOCAL_INC) $(EXT_INC) -c $< -o $@ $(LIBS)

clean:
	rm -f ./obj/*.o 
