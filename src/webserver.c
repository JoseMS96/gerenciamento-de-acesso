#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "ga_database.h"

#define PORT 8080

void send_html(int socket, const char *html) {
    char header[512];
    snprintf(header, sizeof(header), 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Connection: close\r\n"
        "Content-Length: %ld\r\n\r\n", 
        strlen(html)
    );
    
    write(socket, header, strlen(header));
    write(socket, html, strlen(html));
}

char* generate_events_html() {
    sqlite3 *db;
    sqlite3_open("acesso.db", &db);
    
    sqlite3_stmt *stmt;
    const char *sql = "SELECT usuario, porta, datetime(timestamp, 'localtime') "
                      "FROM eventos ORDER BY timestamp DESC;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    char *html = malloc(4096);

    strcpy(html, "<html><head><title>Eventos de Acesso</title>"
        "<meta charset='UTF-8'>"
        "</head><body>"
        "<h1>Eventos Registrados</h1><table border='1'>"
        "<tr><th>Usuário</th><th>Porta</th><th>Data/Hora</th></tr>");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *usuario = (const char*)sqlite3_column_text(stmt, 0);
        int porta = sqlite3_column_int(stmt, 1);
        const char *timestamp = (const char*)sqlite3_column_text(stmt, 2);
        
        char row[256];
        snprintf(row, sizeof(row), 
            "<tr><td>%s</td><td>Porta %d</td><td>%s</td></tr>", 
            usuario, porta, timestamp
        );
        strcat(html, row);
    }
    
    strcat(html, "</table></body></html>");
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return html;
}

void* webserver_thread(void *arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        char *html = generate_events_html();
        send_html(new_socket, html);
        free(html);
        
        close(new_socket);
    }

    return NULL;
}