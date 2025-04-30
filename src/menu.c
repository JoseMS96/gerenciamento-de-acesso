#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "serial_ui.h"
#include "menu.h"
#include "ga_database.h"

void menu_show() {
    char line[64];
    int option;
    do {
        serial_ui_write("\r\n=== MENU PRINCIPAL ===\r\n");
        serial_ui_write("1 - Cadastrar Usuário\r\n");
        serial_ui_write("2 - Listar Usuários\r\n");
        serial_ui_write("3 - Listar Eventos\r\n");
        serial_ui_write("4 - Liberar Porta 1\r\n");
        serial_ui_write("5 - Liberar Porta 2\r\n");
        serial_ui_write("0 - Sair\r\n");
        serial_ui_write("Escolha uma opção: ");
        serial_ui_readline(line, sizeof(line));
        option = atoi(line);

        switch(option) {
            case 1:
                serial_ui_write("Cadastro de usuário...\r\n");
                char nome[50], senha[50];
                int admin;
                serial_ui_write("Nome: ");
                serial_ui_readline(nome, sizeof(nome));
                serial_ui_write("Senha: ");
                serial_ui_readline(senha, sizeof(senha));
                do {
                    serial_ui_write("É admin? (1=Sim,0=Não): ");
                    serial_ui_readline(line, sizeof(line));
                    admin = atoi(line);
                } while (admin!=0 && admin!=1);
                int rc = database_cadastrar_usuario(nome, senha, admin);
                if (rc == SQLITE_OK)
                    serial_ui_write("Usuário cadastrado com sucesso!\r\n");
                else
                    serial_ui_write("Erro ao cadastrar usuário!\r\n");
                break;
            case 2:
                serial_ui_write("Listando usuários...\r\n");
                database_listar_usuarios();
                break;
            case 3:
                serial_ui_write("Listagem de eventos...\r\n");
                break;
            case 4:
                serial_ui_write("Liberação Porta 1...\r\n");
                break;
            case 5:
                serial_ui_write("Liberação Porta 2...\r\n");
                break;
            case 0:
                serial_ui_write("Saindo...\r\n");
                break;
            default:
                serial_ui_write("Opção inválida!\r\n");
                break;
        }
    } while(option != 0);
}