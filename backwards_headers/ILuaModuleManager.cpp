#include <stdlib.h>
#define GMMODULE
#include "ILuaModuleManager.h"
#include "tier0/dbg.h"

#define DEBUG

#ifdef DEBUG
ILuaInterface* lua = NULL; // Testing
#endif

void ILuaModuleManager::CreateInterface(lua_State* state)
{
#ifndef DEBUG
	ILuaInterface* lua = new ILuaInterface(state);
	map_States.insert(lua);
#else
	lua = new ILuaInterface(state);
	Msg( "%p\n", lua );
#endif
}

void ILuaModuleManager::DestroyInterface(lua_State* state)
{
#ifndef DEBUG
	delete GetLuaInterface(state);
#else
	delete lua;
#endif
}

ILuaInterface* ILuaModuleManager::GetLuaInterface(lua_State* state)
{
#ifndef DEBUG
	
#else
	return lua;
#endif
}

ILuaModuleManager* modulemanager = new ILuaModuleManager();