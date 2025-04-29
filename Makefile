CC=gcc
CFLAGS=-Wall
SRC=src/main.c src/menu.c src/ga_database.c
LIBS=-lsqlite3
OUT=gerenciamento_de_acesso

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -f gerenciamento_de_acesso