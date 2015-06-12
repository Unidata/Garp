/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gprint.c
 *
 *	Logging window utility.
 *
 *	History:
 *
 *	 4/97	COMET	Original copy
 *	 5/97	COMET	Print to standard out if GUI has not yet been 
 *			instantiated.
 *	 7/97	COMET	Added X and Xt error handling routines. Also
 *			added write to output file routine.
 *       1/04	Chiz/Unidata	rewrote gprint to use stdarg
 *
 ***********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "gui.h"
#include "winpref.h"
#include "guimacros.h"
#include "underscore.h"

#include "_proto.h"

int x_error();
void xt_error();

static FILE	*fp;
static char	*filename="garp.out";

int
OpenFile ( FILE **fp, char *filename, char *perms )
{
/*
 *	Open a file with the given permissions.
 */
	*fp = fopen ( filename, perms );

	if ( !*fp )
	    return ( -1 );
}

_gprint ( FILE *fp, char *fmt, va_list args )
{
	for (; *fmt; fmt++)
		if (*fmt == '%')
			switch (*++fmt) {
			/*case 'b': {
				
				struct node x = va_arg(args, struct node);
				fprintf(fp, "{%d %d %d %d}", x.a[0], x.a[1], x.a[2], x.a[3]);
				break;
				}*/
			case 'c':
				fprintf(fp, "%c", va_arg(args, int));
				break;
			case 'd':
				fprintf(fp, "%d", va_arg(args, int));
				break;
			case 'w':
				fprintf(fp, "%x", va_arg(args, int));
				break;
			case 's':
				fprintf(fp, "%s", va_arg(args, char *));
				break;
			case 'f':
				fprintf(fp, "%f", va_arg(args, double));
				break;
			default:
				fprintf(fp, "%c", *fmt);
				break;
			}
		 else
			fprintf(fp, "%c", *fmt);
}

void gprint(char *fmt, ...) {
	va_list args;
	extern Widget	log_text;
	extern Widget	main_shell;

	GuiWinPrefDialogType	*wpi;


	wpi = GetGuiWinPrefDialog();

	va_start(args, fmt);
/*
 *	Print to standard out.
 */
	if ( ( GetGuiStdoutW ( wpi ) ) &&
	       XmToggleButtonGetState ( GetGuiStdoutW ( wpi ) ) )
            _gprint ( stdout, fmt, args);
/*
 *	Print to status window when it has been instantiated.
 */
	else if ( ( GetGuiGarpLogW ( wpi ) ) &&
	            XmToggleButtonGetState ( GetGuiGarpLogW ( wpi ) ) ) {
	if ( GetLogInfoW ( wpi ) )
	    wprint ( fmt, args );
	}

/*
 *	Print to a file. Open file if necessary and write to it.
 */
	else if ( ( GetGuiGarpFileW ( wpi ) ) &&
	            XmToggleButtonGetState ( GetGuiGarpFileW ( wpi ) ) ) {
	    if ( !fp )
            	OpenFile ( &fp, filename, "w" );
            if ( fp )
            	_gprint (fp, fmt, args);
	}

/*
 *	Garp interface is not yet initialized so print to standard out.
 */
	else
	    _gprint ( stdout, fmt, args );

	va_end(args);
}


void gfprints ( fmt, str )
        char	*fmt;
	char	*str;
{
/*
 *  Print driver for Fortran strings. This function function parameter list 
 *  comprises a format statement followed by a fixed length (one varible)
 *  argument list. A carriage return is written to the end of the argument
 *  and then passed to gprint.
 */
	char	*arg, *fmt2;
	int	len;

	len = strlen ( str ) + 3;
	arg = ( char * ) malloc ( len * sizeof ( char * ) );
	sprintf ( arg, "%s%s", str, "\n" );

	len = strlen ( fmt ) + 3;
	fmt2 = ( char * ) malloc ( len * sizeof ( char * ) );
	sprintf ( fmt2, "%s%s", fmt, "%s" );

	gprint ( fmt2, arg );

	free ( arg );
	free ( fmt2 );
}


void gfprinti ( fmt, i )
        char	*fmt;
	int	*i;
{
/*
 *  Print driver for Fortran ints. This function function parameter list 
 *  comprises a format statement followed by a fixed length (one varible)
 *  argument list. A carriage return is written to the end of the argument
 *  and then passed to gprint.
 */
	char	*buf, *arg, *fmt2;
	int	len;

	buf = int2str ( *i );
	len = strlen ( buf ) + 3;
	arg = ( char * ) malloc ( len * sizeof ( char * ) );
	sprintf ( arg, "%s%s", buf, "\n" );

	len = strlen ( fmt ) + 3;
	fmt2 = ( char * ) malloc ( len * sizeof ( char * ) );
	sprintf ( fmt2, "%s%s", fmt, "%s" );

	gprint ( fmt2, arg );

	free ( buf );
	free ( arg );
	free ( fmt2 );
}


void gfprintf ( fmt, f )
        char	*fmt;
	float	*f;
{
/*
 *  Print driver for Fortran floats. This function function parameter list 
 *  comprises a format statement followed by a fixed length (one varible)
 *  argument list. A carriage return is written to the end of the argument
 *  and then passed to gprint.
 */
	char	*buf, *arg, *fmt2;
	int	len;

	buf = float2str ( *f );
	len = strlen ( buf ) + 3;
	arg = ( char * ) malloc ( len * sizeof ( char * ) );
	sprintf ( arg, "%s%s", buf, "\n" );

	len = strlen ( fmt ) + 3;
	fmt2 = ( char * ) malloc ( len * sizeof ( char * ) );
	sprintf ( fmt2, "%s%s", fmt, "%s" );

	gprint ( fmt2, arg );

	free ( buf );
	free ( arg );
	free ( fmt2 );
}


int
x_error ( Display *dpy, XErrorEvent *err_event )
{
	char			buf[BUFSIZ];

	XGetErrorText(dpy, err_event->error_code, buf, sizeof buf);

	gprint( "X Error: <%s>\n", buf);
}


void
xt_error ( char *message )
{

	gprint ( "Xt Error: %s\n", message);

}

