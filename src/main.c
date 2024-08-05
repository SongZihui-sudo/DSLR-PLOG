#include <gphoto2/gphoto2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oled/Pico_OLED_0in96.h"

static GPPortInfoList* portinfolist   = NULL;
static CameraAbilitiesList* abilities = NULL;

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

int init_0in96_olcd( ) 
{
    if ( DEV_ModuleInit( ) != 0 )
    {
        fprintf( stderr, "0.96 OLED init fail: %d\n", -1 );
        return -1;
    }

    fprintf( stdout, "OLED Init...\r\n" );
    OLED_Init( );
    OLED_Clear( );

    while (1) 
    {
        OLED_ShowString( 0, 0, "luckfox", 16 );
        DEV_Delay_ms( 1000 );
        OLED_Clear( );
    }

    return 0;
}

int main( int argc, char** argv )
{
    Camera* camera;
    int ret;
    char* owner;
    GPContext* context;
    CameraText text;
    void* evtdata;

    init_0in96_olcd( );

    context = sample_create_context( ); /* see context.c */
    gp_camera_new( &camera );

    /* This call will autodetect cameras, take the
     * first one from the list and use it. It will ignore
     * any others... See the *multi* examples on how to
     * detect and use more than the first one.
     */
    ret = gp_camera_init( camera, context );
    if ( ret < GP_OK )
    {
        printf( "No camera auto detected.\n" );
        gp_camera_free( camera );
        return 0;
    }

    /* Simple query the camera summary text */
    ret = gp_camera_get_summary( camera, &text, context );
    if ( ret < GP_OK )
    {
        printf( "Camera failed retrieving summary.\n" );
        gp_camera_free( camera );
        return 0;
    }
    printf( "Summary:\n%s\n", text.text );

    // get shutter speed
    char* val;
    float shutterspeed;
    ret = get_config_value_string( camera, "shutterspeed", &val, context );
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
        int res_shutterspeed = 1 / shutterspeed;
        printf( "Shutterspeed is 1 / %d (%g) \n", res_shutterspeed, shutterspeed );
        free( val );
    }

    // get iso
    float iso = 0.0;
    ret = get_config_value_string( camera, "iso", &val, context );
    if ( ret == GP_OK )
    {
        sscanf( val, "%g", &iso );
        printf( "ISO is %g\n", iso );
        free( val );
    }

    // get aperture
    float aperture = 0;
    ret = get_config_value_string( camera, "f-number", &val, context );
    if ( ret == GP_OK )
    {
        sscanf( val, "f/%g", &aperture );
        printf( "Aperture is %s (%g)\n", val, aperture );
        free( val );
    }

    gp_camera_exit( camera, context );
    gp_camera_free( camera );
    gp_context_unref( context );

    return 0;
}
