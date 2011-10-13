#if !defined( DTPRINTF_H )
#define DTPRINTF_H

#include <tchar.h>


#if defined( _DEBUG )

// バッファ長 (スタックに確保します)
#define DTPRINTF_BUFFER ( 256 )

// OutputDebugString＋書式指定機能
int dtprintf( const TCHAR * fmt, ... );

#else // defined( _DEBUG )

// リリースビルド時は何もしない
#define dtprintf( ... ) ( 0 )

#endif // defined( _DEBUG )


#endif // !defined( DTPRINTF_H )
