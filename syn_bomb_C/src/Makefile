
.PHONY:syn_bomb.o client_bomb

all: client_bomb

client_bomb: syn_bomb.o client_bomb.c
	$(CC) client_bomb.c syn_bomb.o -o ../client_bomb

syn_bomb.o: syn_bomb.c syn_bomb.h
	$(CC) -c syn_bomb.c -o syn_bomb.o

clean_obj:
	-@rm *.o
