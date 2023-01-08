all: user_lib.o backend.c frontend.c
	gcc backend.c users_lib.o -o backend
	gcc frontend.c -o Frontend

cliente: frontend.c
	gcc frontend.c -o Frontend

servidor: users_lib.o backend.c
	gcc backend.c users_lib.o -o backend

user_lib.o: users_lib.h
	gcc users_lib.h -o user_lib.o
	
backend.c : backend.c

frontend.c : frontend.c

clean:
	rm Frontend
	rm backend
