#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

extern sqlite3 *db;

int database_init();

int database_cadastrar_usuario(const char *nome, const char *senha, int admin, int acesso1, int acesso2);

int database_listar_usuarios();

int database_listar_eventos();

int database_verificar_senha(const char *nome, const char *senha, int *admin, int *acesso1, int *acesso2);

int database_registrar_evento(const char *usuario, int porta);

void database_close();

void database_delete_file();

#endif