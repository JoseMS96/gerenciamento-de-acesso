CC=gcc
CFLAGS=-Wall
SRC=src/main.c src/menu.c
OUT=gerenciamento_de_acesso

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)