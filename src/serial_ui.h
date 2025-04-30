#ifndef SERIAL_UI_H
#define SERIAL_UI_H

// Inicializa a COM para I/O do menu
int serial_ui_init(const char *device, int baud);

void serial_ui_close(void);

void serial_ui_write(const char *str);

int serial_ui_readline(char *buffer, int maxlen);

#endif