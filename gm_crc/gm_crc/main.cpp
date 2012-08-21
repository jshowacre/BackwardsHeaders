#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#include <string.h>

// Lua module interface
#include "ILuaModuleManager.h"

// File system
#include "checksum_crc.h"

GMOD_MODULE( Open, Close );

LUA_FUNCTION( CRC32 )
{
	ILuaInterface* gLua = Lua();

	gLua->CheckType(1, Type::STRING);
	const char* crcTxt = gLua->GetString(1);

	CRC32_t crc;
	CRC32_Init( &crc );
	CRC32_ProcessBuffer( &crc, crcTxt, strlen(crcTxt) );
	CRC32_Final( &crc );

	char buffer[11];
	sprintf(buffer, "%lu", crc);

	gLua->Push( buffer );
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