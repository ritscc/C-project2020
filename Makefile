all:
	$(rcsh)

rcsh:
	gcc rcsh.c -o rcsh

clean:
	rm rcsh

