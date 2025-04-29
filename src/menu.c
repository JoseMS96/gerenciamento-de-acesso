#include <stdio.h>
#include "menu.h"

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
                break;
            case 2:
                printf("Listagem de usuários...\n");
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