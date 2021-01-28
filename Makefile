all: rcsh

rcsh:
	gcc rcsh.c -g -o rcsh

clean:
	rm rcsh

