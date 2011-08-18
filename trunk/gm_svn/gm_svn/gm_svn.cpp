//=============================================================================//
// 		gm_svn.dll
//	luasvn gmod port: Tad2020 (http://www.wiremod.com/)
//	luasvn: Sérgio Medeiros (http://www.lua.inf.puc-rio.br/~sergio/luasvn/index.html)
//	subversion: CollabNet (http://subversion.tigris.org/)
//=============================================================================//

#define NO_SDK
#include <windows.h>
#include "gmod/GMLuaModule.h"
#include "include/luasvn.h"

/*
// CVar
#include "tier1/iconvar.h"
#include "tier1/tier1.h"
#include "vstdlib/cvar.h"
#include "convar.h"

static ConVar svn_cache_passwords( "svn_cache_passwords", "0", 0, "Cache SVN usernames and passwords." );
*/

// This is a macro to make life easier.
// When the dll is loaded it will call the function (Init)
GMOD_MODULE( Init, Shutdown );

int Init( lua_State *L )
{

	Lua()->NewGlobalTable( "svn" );
	
	ILuaObject *_G_svn = Lua()->GetGlobal( "svn" );

	_G_svn->SetMember( "update", svn_update );
	_G_svn->SetMember( "cleanup", svn_cleanup );
	_G_svn->SetMember( "checkout", svn_checkout );
	_G_svn->SetMember( "commit", svn_commit );
	
	_G_svn->UnReference();

	return 0;
}

// When the DLL is shutdown it calls this function.
// Lua() should be assumed to have been released here
int Shutdown( lua_State *L )
{
	return 0;
}

