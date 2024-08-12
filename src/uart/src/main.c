#include "uart.h"
#include <stdio.h>

int main( int argc, char** argv )
{
    int serial_fd = uart_open( 4 );
    uart_init( serial_fd );
    char rx_buffer[256];
    uart_send( serial_fd, "AT+NAME\r\n" );
    uart_Delay_ms( 200 );
    uart_receive( serial_fd, rx_buffer, 256 );
    while ( 1 )
    {
        uart_receive( serial_fd, rx_buffer, sizeof( rx_buffer ) );
        // 接受蓝牙消息
        uart_receive( serial_fd, rx_buffer, 256 );
        // 传 iso
        if ( !strcmp( rx_buffer, "iso" ) || !strcmp( rx_buffer, "ISO" ) )
        {
            uart_send( serial_fd, "100" );
        }
        // 传 光圈
        else if ( !strcmp( rx_buffer, "F" ) || !strcmp( rx_buffer, "f" ) )
        {
            uart_send( serial_fd, "f/2.8" );
        }
        // 传 快门速度
        else if ( !strcmp( rx_buffer, "S" ) || !strcmp( rx_buffer, "s" ) )
        {
            uart_send( serial_fd, "1/200" );
        }
        else
        {
            uart_send( serial_fd, "bluetooth unknown cmd!" );
            printf( "bluetooth unknown cmd: %s!", rx_buffer );
        }
        uart_Delay_ms( 200 );
    }
    uart_close( serial_fd );
    return 0;
}
