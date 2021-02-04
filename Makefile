subdirs := $(shell find . -maxdepth 2 -path "./commands/*")

.PHONY: all $(subdirs) clean
all: $(subdirs) rcsh

$(subdirs):
		$(MAKE) -C $@ $(MAKECMDGOALS)

rcsh: rcsh.c
		gcc rcsh.c -Wall -Wextra -g -o rcsh

clean: $(subdirs)
		rm rcsh

