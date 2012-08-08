#ifndef ILUAMODULEMANAGER_H
#define ILUAMODULEMANAGER_H

#include "Interface.h"
#include "ILuaInterface.h"
#include <map>

class ILuaModuleManager
{
public:
	void			CreateInterface( lua_State* state );
	void			DestroyInterface( lua_State* state );
	ILuaInterface*	GetLuaInterface( lua_State* state );

private:
	//std::map<lua_State*, ILuaInterface*> map_States;
};

ILuaModuleManager* modulemanager = new ILuaModuleManager();

#define Lua() modulemanager->GetLuaInterface( L )

#endif