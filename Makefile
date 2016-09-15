all : s.c c.c
	gcc -o s s.c
	gcc -o c c.c


clean : 
	rm s c
