all: banker

banker: main.o banker.o
	gcc main.o banker.o -o banker

main.o: main.c banker.h
	gcc -c main.c -Wall -Wextra

banker.o: banker.c banker.h
	gcc -c banker.c -Wall -Wextra

clean:
	rm -f *.o banker result.txt

.PHONY: all clean