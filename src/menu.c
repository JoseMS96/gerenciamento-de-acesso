#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "serial_ui.h"
#include "menu.h"
#include "ga_database.h"
#include "modbus.h"
#include <unistd.h>

extern int modbus_fd;

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
        serial_ui_write("9 - Sair\r\n");
        serial_ui_write("Escolha uma opção: ");
        serial_ui_readline(line, sizeof(line));
        option = atoi(line);

        switch(option) {
            case 1: {
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
                } while (admin != 0 && admin != 1);

                int acesso1 = 0, acesso2 = 0;
                if (admin) {
                    acesso1 = acesso2 = 1;
                } else {
                    do {
                        serial_ui_write("Acesso à sala 1? (1=Sim,0=Não): ");
                        serial_ui_readline(line, sizeof(line));
                        acesso1 = atoi(line);
                    } while (acesso1 != 0 && acesso1 != 1);

                    do {
                        serial_ui_write("Acesso à sala 2? (1=Sim,0=Não): ");
                        serial_ui_readline(line, sizeof(line));
                        acesso2 = atoi(line);
                    } while (acesso2 != 0 && acesso2 != 1);
                }

                int rc = database_cadastrar_usuario(nome, senha, admin, acesso1, acesso2);
                if (rc == SQLITE_OK)
                    serial_ui_write("Usuário cadastrado com sucesso!\r\n");
                else
                    serial_ui_write("Erro ao cadastrar usuário!\r\n");
                break;
            }

            case 2:
                serial_ui_write("Listando usuários...\r\n");
                database_listar_usuarios();
                break;

            case 3: {
                char senha[50], nome[50];
                int admin, a1, a2;
                serial_ui_write("\r\nNome de usuário: ");
                serial_ui_readline(nome, sizeof(nome));
                serial_ui_write("Senha admin: ");
                serial_ui_readline(senha, sizeof(senha));
                if (database_verificar_senha(nome, senha, &admin, &a1, &a2) && admin) {
                    database_listar_eventos();
                } else {
                    serial_ui_write("Acesso negado! Requer privilégios de admin.\r\n");
                }
                break;
            }

            case 4:
            case 5: {
                char nome[50], senha[50];
                int admin, acesso1, acesso2;
                int porta = (option == 4) ? 1 : 2;

                serial_ui_write("\r\nNome de usuário: ");
                serial_ui_readline(nome, sizeof(nome));
                serial_ui_write("Senha: ");
                serial_ui_readline(senha, sizeof(senha));

                if (database_verificar_senha(nome, senha, &admin, &acesso1, &acesso2)) {
                    int permitido = (porta == 1) ? acesso1 : acesso2;
                    if (admin || permitido) {
                        if (modbus_send_command(porta, 1) > 0) {
                            serial_ui_write("Porta liberada!\r\n");
                            database_registrar_evento(nome, porta);
                            sleep(5);
                            modbus_send_command(porta, 0);
                        } else {
                            serial_ui_write("Erro ao comunicar com a porta!\r\n");
                        }
                    } else {
                        serial_ui_write("Acesso negado! Sem permissão para esta sala.\r\n");
                    }
                } else {
                    serial_ui_write("Nome ou senha inválidos!\r\n");
                }
                break;
            }

            case 9:
                serial_ui_write("Saindo...\r\n");
                break;

            default:
                serial_ui_write("Opção inválida!\r\n");
                break;
        }
    } while(option != 0);
}
