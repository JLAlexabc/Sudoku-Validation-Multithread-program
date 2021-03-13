all:project1

project1: project1.c
	gcc -pthread -o project1 project1.c

clean:
	rm -f project1
