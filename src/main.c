#include <stdio.h>
#include <termios.h>
#include "serial_ui.h"
#include "menu.h"
#include "ga_database.h"


int main() {
    if (database_init() != 0) {
        fprintf(stderr, "Erro ao inicializar o banco de dados!\n");
        return -1;
    }
    if (serial_ui_init("/dev/pts/25", B9600) != 0) { 
        fprintf(stderr, "Erro ao abrir a porta serial!\n");
        return -1;
    }
    fprintf(stderr, "Serial inicializada com sucesso. Iniciando menu...\n"); 
    menu_show();
    database_close();
    database_delete_file();
    return 0;
}