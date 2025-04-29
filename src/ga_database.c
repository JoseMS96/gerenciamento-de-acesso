#include <sqlite3.h>
#include <stdio.h>
#include "ga_database.h"

sqlite3 *db;

int database_init() {
    int rc = sqlite3_open("acesso.db", &db);
    if (rc) {
        fprintf(stderr, "Não foi possível abrir banco de dados: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *sql_users = "CREATE TABLE IF NOT EXISTS usuarios ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "nome TEXT NOT NULL,"
                            "senha TEXT NOT NULL,"
                            "admin INTEGER NOT NULL);";

    const char *sql_events = "CREATE TABLE IF NOT EXISTS eventos ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "usuario TEXT NOT NULL,"
                             "porta INTEGER NOT NULL,"
                             "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    char *err = NULL;
    sqlite3_exec(db, sql_users, 0, 0, &err);
    if (err) { fprintf(stderr, "SQL error: %s\n", err); sqlite3_free(err); }
    sqlite3_exec(db, sql_events, 0, 0, &err);
    if (err) { fprintf(stderr, "SQL error: %s\n", err); sqlite3_free(err); }

    return 0;
}

void database_close() {
    sqlite3_close(db);
}