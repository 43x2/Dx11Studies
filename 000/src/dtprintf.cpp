#if defined( _DEBUG )

#include <cstdio>
#include <cstdarg>
#include <windows.h>
#include "dtprintf.h"


int dtprintf( const TCHAR * fmt, ... )
{
	TCHAR   buffer[ DTPRINTF_BUFFER ] = { 0 };
	va_list args;

	va_start( args, fmt );
	int ret = _vstprintf_s( buffer, sizeof( buffer ) / sizeof( TCHAR ), fmt, args );
	OutputDebugString( buffer );

	return ret;
}


#endif // defined( _DEBUG )
