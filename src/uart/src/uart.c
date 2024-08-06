#include "uart.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int uart_open(int serial_port) {
  char serial_port_name[15];
  sprintf(serial_port_name, "/dev/ttyS%d", serial_port);
  int serial_fd;
  serial_fd = open(serial_port_name, O_RDWR | O_NOCTTY);
  if (serial_fd == -1) {
    perror("Failed to open serial port");
    return 1;
  }

  return serial_fd;
}

int uart_init(int serial_fd) {
  struct termios tty;
  memset(&tty, 0, sizeof(tty));

  if (tcgetattr(serial_fd, &tty) != 0) {
    perror("Error from tcgetattr");
    return 1;
  }

  cfsetospeed(&tty, B9600);
  cfsetispeed(&tty, B9600);

  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
    perror("Error from tcsetattr");
    return 1;
  }

  return 0;
}

int uart_send(int serial_fd, char *msg) {
  ssize_t bytes_written = write(serial_fd, msg, strlen(msg));
  if (bytes_written < 0) {
    perror("Error writing to serial port");
    close(serial_fd);
    return 1;
  }
  printf("\rtx_buffer: \n %s ", msg);
  return 0;
}

int uart_receive(int serial_fd, char *buf, int len) {
  int bytes_read = read(serial_fd, buf, len);
  if (bytes_read > 0) {
    buf[bytes_read] = '\0';
    printf("\rrx_buffer: \n %s ", buf);
  } else {
    printf("No data received.\n");
    return 1;
  }
  return 0;
}

void uart_Delay_ms(int xms) {
  int i;
  for (i = 0; i < xms; i++) {
    usleep(1000);
  }
}

void uart_close(int serial_fd) { close(serial_fd); }