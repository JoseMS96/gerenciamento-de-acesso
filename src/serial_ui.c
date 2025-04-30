#define _DEFAULT_SOURCE
#include "serial_ui.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

static int ser_fd = -1;

int serial_ui_init(const char *device, int baud) {
    ser_fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (ser_fd < 0) return -1;

    struct termios tty;
    if (tcgetattr(ser_fd, &tty) != 0) { close(ser_fd); return -1; }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);

    tty.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    tty.c_iflag |= IGNPAR;
    
    tty.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1;

    tcflush(ser_fd, TCIFLUSH);
    if (tcsetattr(ser_fd, TCSANOW, &tty) != 0) { close(ser_fd); return -1; }

    return 0;
}

void serial_ui_close(void) {
    if (ser_fd >= 0) close(ser_fd);
}

void serial_ui_write(const char *str) {
    if (ser_fd >= 0) write(ser_fd, str, strlen(str));
}

int serial_ui_readline(char *buffer, int maxlen) {
    int idx = 0; 
    char c;
    while (idx < maxlen - 1) {
        if (read(ser_fd, &c, 1) <= 0) continue;
        
        // Verifica se é backspace (ASCII 8) ou delete (ASCII 127)
        if (c == '\b' || c == 127) {
            if (idx > 0) {
                idx--; // Remove o último caractere do buffer
                // Envia sequência para apagar visualmente na tela
                write(ser_fd, "\b \b", 3);
            }
        }
        
        else if (c == '\r' || c == '\n') {
            write(ser_fd, "\r\n", 2); 
            break;
        }
        
        else {
            buffer[idx++] = c;
            write(ser_fd, &c, 1); 
        }
    }
    buffer[idx] = '\0';
    return idx;
}