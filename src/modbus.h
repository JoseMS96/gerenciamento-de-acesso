#ifndef MODBUS_H
#define MODBUS_H

int modbus_init(const char *device);

int modbus_send_command(int porta, int estado);

void modbus_close();

#endif