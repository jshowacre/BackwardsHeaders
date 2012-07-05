#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#ifdef _LINUX
	#define FILESYSTEM_LIB "filesystem_steam.so"
#else
	#define FILESYSTEM_LIB "filesystem_steam.dll"
#endif

//Lua module interface
#include "gmod/GMLuaModule.h"

// File system
#include "filesystem.h"

IFileSystem *g_FileSystem = NULL;

GMOD_MODULE( Open, Close );

LUA_FUNCTION( FILE_CRC32 )
{
	Lua()->CheckType(1, GLua::TYPE_STRING);

	const char* mode = Lua()->GetString(2);
	const char* dir = Lua()->GetString(3);

	FileHandle_t fh = g_FileSystem->Open( Lua()->GetString(1), mode != NULL ? mode : "r", dir != NULL ? dir : "GAME" );
 
	if( fh )
	{
		int file_len = g_FileSystem->Size(fh);

		CRC32_t crc;
		CRC32_Init( &crc );
		CRC32_ProcessBuffer( &crc, fh, file_len );
		CRC32_Final( &crc );

		char buffer[10];
		sprintf(buffer, "%lu", crc);
		
		g_FileSystem->Close(fh);

		Lua()->Push( buffer );

		return 1;
	} else
		return 0;
}

LUA_FUNCTION( CRC32 )
{
	Lua()->CheckType(1, GLua::TYPE_STRING);
	const char* crcTxt = Lua()->GetString(1);

	CRC32_t crc;
	CRC32_Init( &crc );
	CRC32_ProcessBuffer( &crc, crcTxt, Lua()->StringLength(1) );
	CRC32_Final( &crc );

	char buffer[10];
	sprintf(buffer, "%lu", crc);

	Lua()->Push( buffer );
	return 1;
}

int Open( lua_State *L )
{
	CreateInterfaceFn FileSystemFactory = Sys_GetFactory( FILESYSTEM_LIB );
	if ( !FileSystemFactory )
		Lua()->Error( "gm_crc: Error getting " FILESYSTEM_LIB " factory.\n" );

	g_FileSystem = ( IFileSystem* )FileSystemFactory( FILESYSTEM_INTERFACE_VERSION, NULL );
	if ( !g_FileSystem )
		Lua()->Error( "gm_crc: Error getting IFileSystem interface.\n" );

	Lua()->SetGlobal("CRC32", CRC32);

	ILuaObject *file = Lua()->GetGlobal("file");
		file->SetMember( "CRC32", FILE_CRC32 );
	file->UnReference();
	
	return 0;
}

int Close( lua_State *L )
{
	return 0;
}