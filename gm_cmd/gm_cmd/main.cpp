#include "ILuaModuleManager.h"

#include <stdlib.h>

GMOD_MODULE( Open, Close );

LUA_FUNCTION( cmd_exec )
{
	ILuaInterface* gLua = Lua();

	gLua->CheckType( 1, GLua::TYPE_STRING );
	gLua->PushLong( system( gLua->GetString( 1 ) ) );
	return 1;
}

int Open( lua_State *L )
{
	ILuaInterface* gLua = Lua();

	gLua->NewGlobalTable( "cmd" );

	ILuaObject *cmdTable = gLua->GetGlobal( "cmd" );
		cmdTable->SetMember( "exec", cmd_exec );
	cmdTable->UnReference();
	return 0;
}

int Close( lua_State *L )
{
	return 0;
}
