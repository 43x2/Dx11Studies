#if !defined( DebugAssert_h )
#define DebugAssert_h

#include <cstdlib>
#include <dtprintf.h>


#if defined( _DEBUG )

// メッセージなしアサート
#define ASSERT( cond ) \
	do { \
		if ( !( cond ) ) { \
			dtprintf( _T( "Assertion failed in %s() (%s:%u)\n" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ ); \
			abort(); \
		} \
	} while( 0 )

// メッセージ付きアサート
#define ASSERT_MESSAGE( cond, ... ) \
	do { \
		if ( !( cond ) ) { \
			dtprintf( __VA_ARGS__ ); \
			dtprintf( _T( " [in %s() (%s:%u)]\n" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ ); \
			abort(); \
		} \
	} while( 0 )

// メッセージなしベリファイ
#define VERIFY( cond )	ASSERT( cond )

// メッセージ付きベリファイ
#define VERIFY_MESSAGE( cond, ... )	ASSERT_MESSAGE( cond, __VA_ARGS__ )

#else // defined( _DEBUG )

#define ASSERT( cond )
#define ASSERT_MESSAGE( cond, ... )
// リリースビルド時でも VERIFY は cond を評価する
#define VERIFY( cond )	( cond )
#define VERIFY_MESSAGE( cond, ... )	( cond )

#endif // defined( _DEBUG )


#endif // !defined( DebugAssert_h )
