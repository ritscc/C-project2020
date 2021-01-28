all: clean rcsh

rcsh:
	gcc rcsh.c -Wall -Wextra -g -o rcsh

clean:
	rm rcsh

