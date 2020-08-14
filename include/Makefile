
SUB_DIRS = $(filter %/, $(shell ls -F ./))

all: compile

compile:
	@for dir in $(SUB_DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) compile; fi;); \
	done

clean:
	@for dir in $(SUB_DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) clean; fi;); \
	done
	