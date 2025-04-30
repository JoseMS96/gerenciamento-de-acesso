CC=gcc
CFLAGS=-Wall -I./src
SRC=src/main.c src/menu.c src/ga_database.c src/serial_ui.c src/modbus.c src/webserver.c
LIBS=-lsqlite3 -lpthread
OUT=gerenciamento_de_acesso

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -f $(OUT)