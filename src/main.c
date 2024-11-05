#include "oled/oled_0_in_96.h"
#include "uart/src/uart.h"
#include <assert.h>
#include <gphoto2/gphoto2.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GPPortInfoList* portinfolist
= NULL;
static CameraAbilitiesList* abilities = NULL;
char shutterspeed_val[16]; // 快门速度
char iso_val[16];          // 感光度
char aperture_val[16];     // 光圈
int serial_fd;             // 串口
Camera* camera;
GPContext* context;

/*
 * This detects all currently attached cameras and returns
 * them in a list. It avoids the generic usb: entry.
 *
 * This function does not open nor initialize the cameras yet.
 */
int sample_autodetect( CameraList* list, GPContext* context )
{
    gp_list_reset( list );
    return gp_camera_autodetect( list, context );
}

static void ctx_error_func( GPContext* context, const char* str, void* data )
{
    fprintf( stderr, "\n*** Contexterror ***              \n%s\n", str );
    fflush( stderr );
}

static void ctx_status_func( GPContext* context, const char* str, void* data )
{
    fprintf( stderr, "%s\n", str );
    fflush( stderr );
}

static GPContext* sample_create_context( )
{
    GPContext* context;

    /* This is the mandatory part */
    context = gp_context_new( );

    /* All the parts below are optional! */
    gp_context_set_error_func( context, ctx_error_func, NULL );
    gp_context_set_status_func( context, ctx_status_func, NULL );

    /* also:
    gp_context_set_cancel_func    (p->context, ctx_cancel_func,  p);
        gp_context_set_message_func   (p->context, ctx_message_func, p);
        if (isatty (STDOUT_FILENO))
                gp_context_set_progress_funcs (p->context,
                        ctx_progress_start_func, ctx_progress_update_func,
                        ctx_progress_stop_func, p);
     */
    return context;
}

/*
 * This function looks up a label or key entry of
 * a configuration widget.
 * The functions descend recursively, so you can just
 * specify the last component.
 */

static int _lookup_widget( CameraWidget* widget, const char* key, CameraWidget** child )
{
    int ret;
    ret = gp_widget_get_child_by_name( widget, key, child );
    if ( ret < GP_OK )
        ret = gp_widget_get_child_by_label( widget, key, child );
    return ret;
}

/* Gets a string configuration value.
 * This can be:
 *  - A Text widget
 *  - The current selection of a Radio Button choice
 *  - The current selection of a Menu choice
 *
 * Sample (for Canons eg):
 *   get_config_value_string (camera, "owner", &ownerstr, context);
 */
static int get_config_value_string( Camera* camera, const char* key, char** str, GPContext* context )
{
    CameraWidget *widget = NULL, *child = NULL;
    CameraWidgetType type;
    int ret;
    char* val;

    ret = gp_camera_get_config( camera, &widget, context );
    if ( ret < GP_OK )
    {
        fprintf( stderr, "camera_get_config failed: %d\n", ret );
        return ret;
    }
    ret = _lookup_widget( widget, key, &child );
    if ( ret < GP_OK )
    {
        fprintf( stderr, "lookup widget failed: %d\n", ret );
        goto out;
    }

    /* This type check is optional, if you know what type the label
     * has already. If you are not sure, better check. */
    ret = gp_widget_get_type( child, &type );
    if ( ret < GP_OK )
    {
        fprintf( stderr, "widget get type failed: %d\n", ret );
        goto out;
    }
    switch ( type )
    {
        case GP_WIDGET_MENU:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_TEXT:
            break;
        default:
            fprintf( stderr, "widget has bad type %d\n", type );
            ret = GP_ERROR_BAD_PARAMETERS;
            goto out;
    }

    /* This is the actual query call. Note that we just
     * a pointer reference to the string, not a copy... */
    ret = gp_widget_get_value( child, &val );
    if ( ret < GP_OK )
    {
        fprintf( stderr, "could not query widget value: %d\n", ret );
        goto out;
    }
    /* Create a new copy for our caller. */
    *str = strdup( val );
out:
    gp_widget_free( widget );
    return ret;
}

/*
    蓝牙串口
*/
void* repl_booltooth( )
{
    char rx_buffer[256];
    printf( "Open Bluetooth OK!\n" );
    int iso_count   = 0;
    int f_count     = 0;
    int s_count     = 0;
    int take_count  = 0;
    int clear_count = 0;
    uart_send(serial_fd, "Open Bluetooth OK!");
    uart_receive( serial_fd, rx_buffer, 256 );
    while ( 1 )
    {
        // 接受蓝牙消息
        uart_receive( serial_fd, rx_buffer, 256 );
        // 传 iso
        if ( ( !strcmp( rx_buffer, "iso" ) || !strcmp( rx_buffer, "ISO" ) ) && !iso_count )
        {
            uart_send( serial_fd, iso_val );
            iso_count++;
            f_count     = 0;
            s_count     = 0;
            take_count  = 0;
            clear_count = 0;
        }
        // 传 光圈
        else if ( ( !strcmp( rx_buffer, "F" ) || !strcmp( rx_buffer, "f" ) ) && !f_count )
        {
            uart_send( serial_fd, aperture_val );
            iso_count = 0;
            s_count   = 0;
            f_count++;
            take_count  = 0;
            clear_count = 0;
        }
        // 传 快门速度
        else if ( ( !strcmp( rx_buffer, "S" ) || !strcmp( rx_buffer, "s" ) ) && !s_count )
        {
            uart_send( serial_fd, shutterspeed_val );
            iso_count = 0;
            f_count   = 0;
            s_count++;
            take_count  = 0;
            clear_count = 0;
        }
        // 拍摄
        else if ( ( !strcmp( rx_buffer, "take" ) ) && !take_count )
        {
            printf( "Capturing.\n" );
            CameraFilePath camera_file_path;
            int retval = gp_camera_trigger_capture( camera, context );
            if ( retval != GP_OK )
            {
                uart_send( serial_fd, "capture failed!" );
                printf( "  capture failed: %d\n", retval );
            }
            uart_send( serial_fd, "Capturing OK!" );
            take_count++;
            f_count     = 0;
            s_count     = 0;
            iso_count   = 0;
            clear_count = 0;
        }
        // /n
        else if ( ( !strcmp( rx_buffer, "###" ) && !clear_count ) )
        {
            clear_count++;
            take_count = 0;
            f_count    = 0;
            s_count    = 0;
            iso_count  = 0;
        }
        uart_Delay_ms( 10 );
    }
}

// oled 显示信息
void* r_oled( )
{
    while ( 1 )
    {
        // oled 显示
        OLED_ShowString( 0, 0, "DSLR-PLOG-IN ", 12 );
        OLED_ShowString( 0, 2, "ISO: ", 16 );
        OLED_ShowString( 0, 4, "F: ", 16 );
        OLED_ShowString( 0, 6, "S: ", 16 );

        // get shutter speed
        char* val;
        float shutterspeed;
        int ret = get_config_value_string( camera, "shutterspeed", &val, context );
        if ( ret == GP_OK )
        {
            if ( strchr( val, '/' ) )
            {
                int zaehler, nenner;
                sscanf( val, "%d/%d", &zaehler, &nenner );
                shutterspeed = 1.0 * zaehler / nenner;
            }
            else
            {
                if ( !sscanf( val, "%g", &shutterspeed ) )
                    shutterspeed = 0.0;
            }
            if ( shutterspeed < 1 )
            {
                int res_shutterspeed = 1 / shutterspeed;
                sprintf( shutterspeed_val, "1 / %d", res_shutterspeed );
            }
            else
            {
                sprintf( shutterspeed_val, "%g", shutterspeed );
            }
        }

        // get iso
        int iso = 0;
        ret     = get_config_value_string( camera, "iso", &val, context );
        if ( ret == GP_OK )
        {
            sscanf( val, "%d", &iso );
            sprintf( iso_val, "%d", iso );
        }

        // get aperture
        float aperture = 0;
        ret            = get_config_value_string( camera, "f-number", &val, context );
        if ( ret == GP_OK )
        {
            sscanf( val, "f/%g", &aperture );
            sprintf( aperture_val, "F/%g", aperture );
        }

        OLED_ShowString( 50, 2, iso_val, 16 );
        OLED_ShowString( 50, 4, aperture_val, 16 );
        OLED_ShowString( 50, 6, shutterspeed_val, 16 );
        OLED_Clear( );
    }
}

int main( int argc, char** argv )
{
    int ret;
    char* owner;
    CameraText text;
    void* evtdata;

    context = sample_create_context( ); /* see context.c */
    gp_camera_new( &camera );

    ret = gp_camera_init( camera, context );
    if ( ret < GP_OK )
    {
        printf( "No camera auto detected.\n" );
        gp_camera_free( camera );
        return 0;
    }

    // 打开串口
    serial_fd = uart_open( 4 );
    uart_init( serial_fd );

    // 创建蓝牙轮询子线程
    pthread_t tid[2];
    int t_ok = pthread_create( &tid[0], NULL, repl_booltooth, NULL );
    assert( t_ok );

    // oled 初始化
    if ( DEV_ModuleInit( ) != 0 )
    {
        return -1;
    }
    printf( "OLED Init...\r\n" );
    OLED_Init( );
    OLED_Clear( );

    //  子线程在 oled 屏幕上显示信息
    t_ok = pthread_create( &tid[1], NULL, r_oled, NULL );
    assert( t_ok );

    // 等这两个进程
    for ( int i = 0; i < 2; i++ )
    {
        int ret = pthread_join( tid[i], NULL );
    }

    // 释放资源
    pthread_exit( NULL );
    uart_close( serial_fd );
    gp_camera_exit( camera, context );
    gp_camera_free( camera );
    gp_context_unref( context );

    return 0;
}
