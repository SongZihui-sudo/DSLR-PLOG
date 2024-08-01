#include <gphoto2/gphoto2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main( int argc, char** argv )
{
    Camera* camera;
    int ret;
    char* owner;
    GPContext* context;
    CameraText text;

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

    gp_camera_exit( camera, context );
    gp_camera_free( camera );
    gp_context_unref( context );
    return 0;
    return 0;
}
