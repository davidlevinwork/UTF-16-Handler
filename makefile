a.out: ex2.o
	gcc -Werror -o a.out ex2.o

ex2.o: ex2.c
	gcc -Werror -c -o ex2.o ex2.c

clean:
	rm -f *.o a.out
