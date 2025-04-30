#include "serial_ui.h"
#include <sqlite3.h>
#include <stdio.h>
#include "ga_database.h"
#include <unistd.h>
#include <string.h>

sqlite3 *db;

int database_init() {
    int rc = sqlite3_open("acesso.db", &db);
    if (rc) {
        fprintf(stderr, "Não foi possível abrir banco de dados: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *sql_users =
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome TEXT NOT NULL,"
        "senha TEXT NOT NULL,"
        "admin INTEGER NOT NULL CHECK (admin IN (0,1)),"
        "acesso_sala1 INTEGER NOT NULL CHECK (acesso_sala1 IN (0,1)),"
        "acesso_sala2 INTEGER NOT NULL CHECK (acesso_sala2 IN (0,1))"
        ");";

    const char *sql_events =
        "CREATE TABLE IF NOT EXISTS eventos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario TEXT NOT NULL,"
        "porta INTEGER NOT NULL,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    char *err = NULL;
    sqlite3_exec(db, sql_users, 0, 0, &err);
    if (err) { fprintf(stderr, "SQL error: %s\n", err); sqlite3_free(err); }
    sqlite3_exec(db, sql_events, 0, 0, &err);
    if (err) { fprintf(stderr, "SQL error: %s\n", err); sqlite3_free(err); }

    return 0;
}

int database_cadastrar_usuario(const char *nome,
                               const char *senha,
                               int admin,
                               int acesso1,
                               int acesso2) {
    char sql[512];
    char *errmsg = 0;
    snprintf(sql, sizeof(sql),
             "INSERT INTO usuarios "
             "(nome, senha, admin, acesso_sala1, acesso_sala2) "
             "VALUES ('%s','%s',%d,%d,%d);",
             nome, senha, admin, acesso1, acesso2);

    int rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQLite erro: %s\n", errmsg);
        sqlite3_free(errmsg);
    }
    return rc;
}

int database_listar_usuarios() {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT nome FROM usuarios;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    int found = 0;
    char buffer[256]; 

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (!found) {
            serial_ui_write("\r\nUsuários Cadastrados:\r\n");
            found = 1;
        }
        snprintf(buffer, sizeof(buffer), "- %s\r\n", sqlite3_column_text(stmt, 0));
        serial_ui_write(buffer);
    }

    if (!found) {
        serial_ui_write("Banco de dados vazio.\r\n");
    }

    sqlite3_finalize(stmt);
    return 0;
}

int database_listar_eventos() {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT usuario, porta, datetime(timestamp, 'localtime') "
                      "FROM eventos ORDER BY timestamp DESC;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar consulta: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    char buffer[256];
    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (!found) {
            serial_ui_write("\r\n=== Últimos Eventos ===\r\n");
            found = 1;
        }
        const char *usuario = (const char *)sqlite3_column_text(stmt, 0);
        int porta = sqlite3_column_int(stmt, 1);
        const char *timestamp = (const char *)sqlite3_column_text(stmt, 2);
        
        snprintf(buffer, sizeof(buffer), "%-15s | Porta %d | %s\r\n",
                 usuario, porta, timestamp);
        serial_ui_write(buffer);
    }

    if (!found) {
        serial_ui_write("Nenhum evento registrado.\r\n");
    }

    sqlite3_finalize(stmt);
    return 0;
}

int database_verificar_senha(const char *nome,
                             const char *senha,
                             int *admin,
                             int *acesso1,
                             int *acesso2) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT admin, acesso_sala1, acesso_sala2 "
                      "FROM usuarios WHERE nome = ? AND senha = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar consulta: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_STATIC);
    
    int result = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *admin   = sqlite3_column_int(stmt, 0);
        *acesso1 = sqlite3_column_int(stmt, 1);
        *acesso2 = sqlite3_column_int(stmt, 2);
        result = 1;
    }

    sqlite3_finalize(stmt);
    return result;
}

int database_registrar_evento(const char *usuario, int porta) {
    char sql[256];
    snprintf(sql, sizeof(sql),
             "INSERT INTO eventos (usuario, porta) VALUES ('%s', %d);",
             usuario, porta);
    
    char *errmsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao registrar evento: %s\n", errmsg);
        sqlite3_free(errmsg);
        return rc;
    }
    return SQLITE_OK;
}

void database_close() {
    sqlite3_close(db);
}

void database_delete_file() {
    const char *filename = "acesso.db";
    if (unlink(filename) == 0) {
        printf("Banco de dados removido com sucesso.\n");
    } else {
        perror("Erro ao remover o banco de dados");
    }
}