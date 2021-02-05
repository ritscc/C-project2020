wc: wc.o
	gcc -o wc wc.c

wc.o: wc.c
	gcc -c wc.c

all: wc

clean:
	\rm *.o
