#ifndef UART_H
#define UART_H

int uart_open(int serial_port);

int uart_init(int serial_fd);

int uart_send(int serial_fd, char *msg);

int uart_receive(int serial_fd, char* buf, int len);

void uart_Delay_ms(int xms);

void uart_close(int serial_fd);

#endif
