#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "serial_ui.h"
#include "menu.h"
#include "ga_database.h"
#include "modbus.h"
#include <pthread.h>
#include "webserver.h"


int main() {
    if (database_init() != 0) {
        fprintf(stderr, "Erro ao inicializar o banco de dados!\n");
        return EXIT_FAILURE;
    }

    if (serial_ui_init("/dev/pts/70", B9600) != 0) {
        fprintf(stderr, "Erro ao abrir a porta serial da UI!\n");
        return EXIT_FAILURE;
    }

    if (modbus_init("/dev/pts/72") != 0) { // Porta Modbus simulada
        fprintf(stderr, "Erro ao inicializar Modbus!\n");
        return EXIT_FAILURE;
    }

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, webserver_thread, NULL) != 0) {
        fprintf(stderr, "Erro ao iniciar servidor web!\n");
        return EXIT_FAILURE;
    }

    menu_show();

    modbus_close();
    database_close();
    database_delete_file();
    
    return EXIT_SUCCESS;
}