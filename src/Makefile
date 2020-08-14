
# build order
# tile, resources, gauntlet, main
DIRS  = tile
DIRS += resources
DIRS += gauntlet
DIRS += main



# Targets

all: compile

compile:
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) compile; fi;); \
	done

clean:
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) clean; fi;); \
	done
	
# docs:
	# doxygen


test:
	@echo $(INC)
