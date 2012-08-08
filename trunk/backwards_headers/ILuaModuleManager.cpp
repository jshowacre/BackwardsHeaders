#include <stdlib.h>
#define GMMODULE
#include "ILuaModuleManager.h"

void ILuaModuleManager::CreateInterface(lua_State* state)
{
	ILuaInterface* lua = new ILuaInterface(state->luabase);
	//map_States.insert(lua);
}

void ILuaModuleManager::DestroyInterface(lua_State* state)
{
	
}

ILuaInterface* ILuaModuleManager::GetLuaInterface(lua_State* state)
{
	
}