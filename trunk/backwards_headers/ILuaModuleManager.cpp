#include <stdlib.h>
#define GMMODULE
#include "ILuaModuleManager.h"
#include "tier0/dbg.h"

void ILuaModuleManager::CreateInterface(lua_State* state)
{
	ILuaInterface* lua = new ILuaInterface(state);
	map_States[state] = lua;
#ifdef DEBUG
	Msg( "CreateInterface -> %p\t%u\n", lua, state->_ignore_this_common_lua_header_ );
#endif
}

void ILuaModuleManager::DestroyInterface(lua_State* state)
{
	ILuaInterface* lua = GetLuaInterface(state);
#ifdef DEBUG
	Msg( "DestroyInterface -> %p\t%u\n", lua, state->_ignore_this_common_lua_header_ );
#endif
	delete lua;
}

ILuaInterface* ILuaModuleManager::GetLuaInterface(lua_State* state)
{
	ILuaInterface* lua = map_States[state];
	#ifdef DEBUG
		Msg( "GetLuaInterface -> %p\t%u\n", lua, state->_ignore_this_common_lua_header_ );
	#endif
	return lua;
}

ILuaModuleManager* modulemanager = new ILuaModuleManager();