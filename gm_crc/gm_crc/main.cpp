#pragma comment (linker, "/NODEFAULTLIB:libcmt")

// Lua module interface
#include "ILuaModuleManager.h"

// File system
#include "filesystem.h"

GMOD_MODULE( Open, Close );

LUA_FUNCTION( CRC32 )
{
	Lua()->CheckType(1, Type::STRING);
	const char* crcTxt = Lua()->GetString(1);

	CRC32_t crc;
	CRC32_Init( &crc );
	CRC32_ProcessBuffer( &crc, crcTxt, strlen(crcTxt) );
	CRC32_Final( &crc );

	char buffer[10];
	sprintf(buffer, "%lu", crc);

	Lua()->Push( buffer );
	return 1;
}

int Open( lua_State *L )
{
	Lua()->SetGlobal("CRC32", CRC32);
	return 0;
}

int Close( lua_State *L )
{
	return 0;
}