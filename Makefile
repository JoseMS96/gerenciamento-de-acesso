CC=gcc
CFLAGS=-Wall -I./src
SRC=src/main.c src/menu.c src/ga_database.c src/serial_ui.c src/modbus.c
LIBS=-lsqlite3
OUT=gerenciamento_de_acesso

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -f $(OUT)