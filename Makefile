all:
	gcc user_lib.h -o user_lib.o
	gcc backend.c users_lib.o -o backend.o

cliente: backend.c
	gcc backend.c -o backend.o

clean:
	rm *.o
	rm proxe.txt
