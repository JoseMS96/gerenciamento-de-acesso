#include <stdio.h>
#include <sqlite3.h>
#include "menu.h"
#include "ga_database.h"

void menu_show() {
    int option;
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1 - Cadastrar Usuário\n");
        printf("2 - Listar Usuários\n");
        printf("3 - Listar Eventos\n");
        printf("4 - Liberar Porta 1\n");
        printf("5 - Liberar Porta 2\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &option);

        switch(option) {
            case 1:
                printf("Cadastro de usuário...\n");
                char nome[50], senha[50];
                int admin;
                printf("Nome: ");
                scanf("%s", nome);
                printf("Senha: ");
                scanf("%s", senha);
                printf("É administrador? (1=Sim, 0=Não): ");
                scanf("%d", &admin);
                int result = database_cadastrar_usuario(nome, senha, admin);
                if (result == SQLITE_OK) {
                    printf("Usuário cadastrado com sucesso!\n");
                } else {
                    printf("Erro ao cadastrar usuário! Código SQLite: %d\n", result);
                }
                break;
            case 2:
                printf("Listando usuários...\n");
                database_listar_usuarios();
                break;
            case 3:
                printf("Listagem de eventos...\n");
                break;
            case 4:
                printf("Liberação Porta 1...\n");
                break;
            case 5:
                printf("Liberação Porta 2...\n");
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }
    } while(option != 0);
}