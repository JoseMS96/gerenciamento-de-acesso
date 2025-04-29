#ifndef DATABASE_H
#define DATABASE_H

int database_init();

int database_cadastrar_usuario(const char *nome, const char *senha, int admin);

int database_listar_usuarios();

void database_delete_file();

void database_close();

#endif