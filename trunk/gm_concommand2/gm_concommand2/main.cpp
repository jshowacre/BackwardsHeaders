#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#ifdef _LINUX
	#define VSTD_LIB "vstdlib.so"
	#define SERVER_LIB "server.so"
#else
	#define VSTD_LIB "vstdlib.dll"
	#define SERVER_LIB "server.dll"
#endif

#include <windows.h>
#include <psapi.h>

#define GAME_DLL 1
#undef _UNICODE

//String stuff
#include <string>
#include <sstream>
#include <iostream>
#include <map>

//Lua module interface
#include "interface.h"
#include "gmod/CStateManager/vfnhook.h"
#include "gmod/CStateManager/GMLuaModule.h"
#include "gmod/CStateManager/CStateManager.h"

// CVar
#include "tier1/iconvar.h"
#include "tier1/tier1.h"
#include "vstdlib/cvar.h"
#include "convar.h"

#include "edict.h"
#include "cbase.h"
#include "util.h"

ICvar *g_ICvar = NULL;
IServerGameClients *g_IServerGameClients = NULL;
int g_nCommandClientIndex = -1;
bool isVFNHooked = false;

GMOD_MODULE( Init, Shutdown );

//-----------------------------------------------------------------------------
// Purpose: 
// Output : CBasePlayer
//-----------------------------------------------------------------------------
int GetCommandClient( void )
{
	// -1 == unknown,dedicated server console
	// 0  == player 1

	// Convert to 1 based offset
	int idx = (g_nCommandClientIndex+1);
	if ( idx > 0 )
	{
		return idx;
	}

	// HLDS console issued command
	return (int) -1;
}

static int OnLuaCmdAutoComplete( char const *partial, char commands[ COMMAND_COMPLETION_MAXITEMS ][ COMMAND_COMPLETION_ITEM_LENGTH ] )
{
	for ( int i=0; i <= 2; i++)
	{
		ILuaInterface* gLua = CStateManager::GetByIndex( i );
		if ( gLua == NULL ){ continue; }

		ILuaObject* concommand = gLua->GetGlobal( "concommand2" );
		ILuaObject* commands = concommand->GetMember( "Commands" );

		ILuaObject* cmdInfo = commands->GetMember( partial );
		if ( cmdInfo != NULL )
		{
			ILuaObject* callback = cmdInfo->GetMember( "autocomplete" );

			gLua->Push( callback );
				gLua->Push( partial );
			gLua->Call(4, 0);

			callback->UnReference();
			cmdInfo->UnReference();
		}

		commands->UnReference();
		concommand->UnReference();

		CUtlLuaVector *members = commands->GetMembers();

		for(int i = 0; i < members->Count(); i++)
		{
			LuaKeyValue &kv = members->Element(i);

			ConCommand* ourCmd = g_ICvar->FindCommand( kv.pKey->GetString() );
			if ( ourCmd )
			{
				g_ICvar->UnregisterConCommand( ourCmd );
				delete ourCmd;
			}
		}

	}
	strcpy( commands[0], "hello" );
	strcpy( commands[1], "goodbye" );
	return 2; // number of entries
}

void OnLuaCmdRan( const CCommand &args )
{
	int plyIndex = GetCommandClient();

	for ( int i=0; i <= 2; i++)
	{
		ILuaInterface* gLua = CStateManager::GetByIndex( i );
		if ( gLua == NULL ){ continue; }

		ILuaObject* largs = gLua->GetNewTable();

		for(int i = 1; i < args.ArgC() ; i++)
			largs->SetMember( i, args[i] );
		
		ILuaObject *retrn = NULL;

		//Msg( "Playerindex %i ran command '%s'\n", plyIndex, args[0] );

		if ( gLua->IsServer() ) // Only the server cares who ran the command
		{
			ILuaObject *globalEntity = gLua->GetGlobal( "Entity" );
		
			if ( globalEntity != NULL )
			{
				if ( !globalEntity->isNil() && globalEntity->isFunction() )
				{
					gLua->Push( globalEntity );
					gLua->Push( (float) plyIndex );
					gLua->Call( 1, 1 );
				}
				globalEntity->UnReference();
				retrn = gLua->GetReturn( 0 );
			}
		}

		ILuaObject* concommand = gLua->GetGlobal( "concommand2" );
		ILuaObject* commands = concommand->GetMember( "Commands" );

		ILuaObject* cmdInfo = commands->GetMember( args[0] );
		if ( cmdInfo != NULL )
		{
			ILuaObject* callback = cmdInfo->GetMember( "func" );

			gLua->Push( callback );
				if ( retrn != NULL )
				{
					gLua->Push( retrn );
					retrn->UnReference();
				}
				else
					gLua->PushNil();
				gLua->Push( args[0] );
				gLua->Push( largs );
				gLua->Push( args.ArgS() );
			gLua->Call(4, 0);

			callback->UnReference();
			cmdInfo->UnReference();
		}

		commands->UnReference();
		concommand->UnReference();
	}

}

LUA_FUNCTION( RemoveEx )
{
	Lua()->CheckType( 1, GLua::TYPE_STRING );

	ILuaObject* concommand = Lua()->GetGlobal( "concommand2" );
	ILuaObject* commands = concommand->GetMember( "Commands" );

	ILuaObject* cmdInfo = commands->GetMember( Lua()->GetString(1) );
	if ( cmdInfo != NULL )
	{
		ILuaObject* nilObj = NULL;
		commands->SetMember( Lua()->GetString(1), nilObj );
		//Msg( "Unregesitered CMD '%s' in Lua table %p\n", Lua()->GetString(1), cmdInfo );

		ConCommand* ourCmd = g_ICvar->FindCommand( Lua()->GetString(1) );
		if ( ourCmd )
		{
			g_ICvar->UnregisterConCommand( ourCmd );
			delete ourCmd;
			//Msg( "Unregesitered CMD '%s'\n", Lua()->GetString(1) );
			Lua()->Push( true );
		} else
			Lua()->Push( false );

		cmdInfo->UnReference();
	} else
		Lua()->Push( false );

	commands->UnReference();
	concommand->UnReference();

	return 1;
}

LUA_FUNCTION( GetTableEx )
{
	ILuaObject* concommand = Lua()->GetGlobal( "concommand2" );
	ILuaObject* commands = concommand->GetMember( "Commands" );
	Lua()->Push( commands );
	commands->UnReference();
	concommand->UnReference();
	return 1;
}

LUA_FUNCTION( AddEx )
{
	Lua()->CheckType( 1, GLua::TYPE_STRING );
	Lua()->CheckType( 2, GLua::TYPE_FUNCTION );

	ConCommand* ourCmd = g_ICvar->FindCommand( Lua()->GetString(1) );
	if ( !ourCmd )
	{
		ILuaObject* concommand = Lua()->GetGlobal( "concommand2" );
		ILuaObject* commands = concommand->GetMember( "Commands" );

		ConCommand* ourNewCmd = new ConCommand( Lua()->GetString(1), OnLuaCmdRan, Lua()->GetString(4), ( Lua()->IsClient() ? FCVAR_CLIENTDLL : FCVAR_GAMEDLL ) | FCVAR_CLIENTCMD_CAN_EXECUTE  );
		g_ICvar->RegisterConCommand( ourNewCmd );

		//Msg( "Regesitered CMD '%s' %p\n", Lua()->GetString(1), ourNewCmd );
	
		ILuaObject* cmdInfo = Lua()->GetNewTable();

		ILuaObject* func = Lua()->GetObject(2);
		ILuaObject* autocomplete = Lua()->GetObject(3);

		cmdInfo->SetMember( "func", func );
		cmdInfo->SetMember( "autocomplete", autocomplete );

		func->UnReference();

		commands->SetMember( Lua()->GetString(1), cmdInfo );

		//Msg( "Regesitered CMD '%s' in Lua table %p\n", Lua()->GetString(1), cmdInfo );

		cmdInfo->UnReference();

		commands->UnReference();
		concommand->UnReference();

		Lua()->Push( true );

	} else {
		Lua()->ErrorNoHalt( "gm_concommand2: Command \"%s\" is already registered\n", Lua()->GetString(1) );
		Lua()->Push( false );
	}

	return 1;
}

DEFVFUNC_( origSetCommandClient, void, ( IServerGameClients *ISrvGmCl, int index ) );

void VFUNC newSetCommandClient( IServerGameClients *ISrvGmCl, int index ) {
	//Msg( "g_nCommandClientIndex = %i", index );
	g_nCommandClientIndex = index; // Simplistic way of getting this value
	return origSetCommandClient( ISrvGmCl, index );
}

int Init( lua_State *L ) {

	ILuaObject* commands = Lua()->GetNewTable();

	Lua()->NewGlobalTable( "concommand2" );

	ILuaObject* concommand = Lua()->GetGlobal( "concommand2" );
		concommand->SetMember( "Add", AddEx );
		concommand->SetMember( "Remove", RemoveEx );
		concommand->SetMember( "GetTable", GetTableEx );
		concommand->SetMember( "Commands", commands );
	concommand->UnReference();

	commands->UnReference();
	
	CreateInterfaceFn VSTDLibFactory = Sys_GetFactory( VSTD_LIB );
	if ( !VSTDLibFactory )
		Lua()->Error( "gm_concommand2: Error getting " VSTD_LIB " factory.\n" );
	
	g_ICvar = ( ICvar* )VSTDLibFactory( CVAR_INTERFACE_VERSION, NULL );
	if ( !g_ICvar )
		Lua()->Error( "gm_concommand2: Error getting ICvar interface.\n" );

	CreateInterfaceFn ServerFactory = Sys_GetFactory( SERVER_LIB );
	if ( !ServerFactory )
		Lua()->Error( "gmcl_extras: Error getting " SERVER_LIB " factory.\n" );

	g_IServerGameClients = ( IServerGameClients* )ServerFactory( INTERFACEVERSION_SERVERGAMECLIENTS, NULL );
	if ( !g_IServerGameClients )
		Lua()->Error( "gmcl_extras: Error getting IServerGameClients interface.\n" );

	if ( !isVFNHooked && Lua()->IsServer() )
	{
		isVFNHooked = true;
		HOOKVFUNC( g_IServerGameClients, 6, origSetCommandClient, newSetCommandClient );
	}

	return 0;
}

int Shutdown(lua_State *L)
{
	if ( isVFNHooked )
	{
		isVFNHooked = false;
		UNHOOKVFUNC( g_IServerGameClients, 6, origSetCommandClient );
	}

	ILuaObject* concommand = Lua()->GetGlobal( "concommand2" );
	ILuaObject* commands = concommand->GetMember( "Commands" );

	if ( commands != NULL && commands->isTable() )
	{
		CUtlLuaVector *members = commands->GetMembers();

		for(int i = 0; i < members->Count(); i++)
		{
			LuaKeyValue &kv = members->Element(i);

			ConCommand* ourCmd = g_ICvar->FindCommand( kv.pKey->GetString() );
			if ( ourCmd )
			{
				g_ICvar->UnregisterConCommand( ourCmd );
				delete ourCmd;
			}
		}

		commands->UnReference();
	}
	concommand->UnReference();

	return 0;
}