#define _DEFAULT_SOURCE
#include "modbus.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

static int modbus_fd = -1;

// Cálculo CRC Modbus RTU
static unsigned short crc16(unsigned char *buffer, unsigned int len) {
    unsigned int crc = 0xFFFF;
    for (unsigned int pos = 0; pos < len; pos++) {
        crc ^= (unsigned int)buffer[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

int modbus_init(const char *device) {
    modbus_fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (modbus_fd < 0) {
        perror("Erro ao abrir porta Modbus");
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    
    if (tcgetattr(modbus_fd, &tty) != 0) {
        perror("Erro tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;   // Sem paridade
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;       // 8 bits
    tty.c_cflag &= ~CRTSCTS;  // Sem controle de fluxo
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag = 0;          // Modo não canônico
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(modbus_fd, TCSANOW, &tty) != 0) {
        perror("Erro tcsetattr");
        return -1;
    }

    return 0;
}

int modbus_send_command(int porta, int estado) {
    unsigned char frame[8];
    
    // Montagem do quadro Modbus
    frame[0] = 0x01;                   // Endereço do dispositivo
    frame[1] = 0x06;                   // Código da função (Write Single Register)
    frame[2] = 0x00;                   // High byte do endereço
    frame[3] = (porta == 1) ? 0x34 : 0x35; // Low byte do endereço
    frame[4] = (estado) ? 0x00 : 0x00; // High byte do valor
    frame[5] = (estado) ? 0xFF : 0x00; // Low byte do valor
    
    // Cálculo CRC
    unsigned short crc = crc16(frame, 6);
    frame[6] = crc & 0xFF;
    frame[7] = (crc >> 8) & 0xFF;

    // Envio do quadro
    int bytes_written = write(modbus_fd, frame, sizeof(frame));
    tcdrain(modbus_fd); 
    
    return bytes_written;
}

void modbus_close() {
    if (modbus_fd >= 0) {
        close(modbus_fd);
        modbus_fd = -1;
    }
}