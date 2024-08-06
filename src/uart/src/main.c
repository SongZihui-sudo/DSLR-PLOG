#include "uart.h"
#include <stdio.h>

int main(int argc, char **argv) {
  int serial_fd = uart_open(4);
  uart_init(serial_fd);
  char rx_buffer[256];
  uart_send(serial_fd, "AT+NAME\r\n");
  uart_Delay_ms(200);
  uart_receive(serial_fd, rx_buffer, 256);
  while (1) {
    uart_receive(serial_fd, rx_buffer, 256);
    uart_Delay_ms(200);
  }
  uart_close(serial_fd);
  return 0;
}
