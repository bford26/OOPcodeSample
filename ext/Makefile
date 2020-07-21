
# gets a list of all subsequent dirs while excluding the object folders
# object folders shouldnt be at this level, but it is just simple error handling
DIRS  = $(patsubst %/, %, $(filter-out obj/, $(filter %/, $(shell ls -F))))

all: compile

compile:
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) compile; fi;); \
	done

clean:
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) clean; fi;); \
	done