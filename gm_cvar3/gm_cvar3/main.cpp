#undef _UNICODE

#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#ifdef _LINUX
	#define VSTD_LIB "vstdlib.so"
	#define ENGINE_LIB "engine.so"
#else
	#define VSTD_LIB "vstdlib.dll"
	#define ENGINE_LIB "engine.dll"
#endif

//#define CVAR3_NAME "CVar3"
//#define CVAR3_ID 9001

#include "sigscan.h"

//Lua module interface
#include "interface.h"
#include "gmod/GMLuaModule.h"

// CVar
#include "tier1/iconvar.h"
#include "tier1/tier1.h"
#include "vstdlib/cvar.h"
#include "iconvar.h"
#include "convar.h"

#include "eiface.h"
#include "netadr.h"
#include "inetmsghandler.h"

#include "tmpserver.h"
#include "tmpclient.h"
//#include "iserver.h"
//#include "iclient.h"

#include "igameevents.h"
#include "inetchannel.h"

GMOD_MODULE( Open, Close );

ICvar *g_ICvar = NULL;
CBaseServer *pServer = NULL;
IVEngineServer *g_EngineServer = NULL;

int GetPlayerMethodInt( lua_State* L, int i, char *methodName ) {
	
	ILuaObject *ply = Lua()->GetObject( i );
    ILuaObject *plyMeta = Lua()->GetMetaTable( "Player", GLua::TYPE_ENTITY );
    ILuaObject *method = plyMeta->GetMember( methodName );
     
    Lua()->Push( method );
    Lua()->Push( ply );
    Lua()->Call( 1, 1 );
     
    ILuaObject *retrn = Lua()->GetReturn( 0 );

    int retrnNum = retrn->GetInt();
     
    ply->UnReference();
    plyMeta->UnReference();
    method->UnReference();
    retrn->UnReference();

	return retrnNum;
}

inline ILuaObject* NewConVarObject( lua_State* L, const char *name ) {
	ILuaObject* func = Lua()->GetGlobal( "GetConVar" );
	Lua()->Push( func );
	Lua()->Push( name );
	if( !Lua()->Call( 1, 1 ) ) {
		func->UnReference();
		return NULL;
	}
	return Lua()->GetReturn( 0 );
}

LUA_FUNCTION( GetAllCvars )
{
	ConCommandBase *conCmds = g_ICvar->GetCommands();

	ILuaObject* conVarTable = Lua()->GetNewTable();

	int i = 1;
	while( conCmds )
	{
		if ( !conCmds->IsCommand() ) {

			/*
			ConVar *cvar = ( ConVar* ) g_ICvar->FindVar( conCmds->GetName() );

			ILuaObject *metaT = Lua()->GetMetaTable( "ConVar", GLua::TYPE_CONVAR );
				Lua()->PushUserData( metaT, cvar );
			metaT->UnReference();
			*/

			/*ILuaObject* luaCvar = NewConVarObject( L, conCmds->GetName() );
				conVarTable->SetMember( i, luaCvar ); // attempt to index a userdata value?
			luaCvar->UnReference();*/

			conVarTable->SetMember( i, conCmds->GetName() );
			i++;
		}
		conCmds = conCmds->GetNext();
	}

	Lua()->Push( conVarTable );

	conVarTable->UnReference();

	return 1;
}

LUA_FUNCTION( GetAllCmds )
{
	ConCommandBase *conCmds = g_ICvar->GetCommands();

	ILuaObject* conVarTable = Lua()->GetNewTable();

	int i = 1;
	while( conCmds )
	{
		if ( conCmds->IsCommand() ) {
			conVarTable->SetMember( i, conCmds->GetName() );
			i++;
		}
		conCmds = conCmds->GetNext();
	}

	Lua()->Push( conVarTable );

	conVarTable->UnReference();

	return 1;
}

LUA_FUNCTION( SetConVarValue )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	int argType = Lua()->GetType(2);

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	switch( argType ) // From haza's cvar2..
	{
		case GLua::TYPE_NUMBER:
			cvar->SetValue( (float) Lua()->GetDouble( 2 ) );
			break;
		case GLua::TYPE_BOOL:
			cvar->SetValue( Lua()->GetBool( 2 ) ? 1 : 0 );
			break;
		case GLua::TYPE_STRING:
			cvar->SetValue( Lua()->GetString( 2 ) );
			break;
		default:
			Lua()->Error("Argument #2 invalid!\n");
			break;
	}

	return 0;
}

LUA_FUNCTION( GetConVarBool )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( (bool) cvar->GetBool() );

	return 1;
}

LUA_FUNCTION( GetConVarDefault )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( cvar->GetDefault() );

	return 1;
}

LUA_FUNCTION( GetConVarFloat )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( (float) cvar->GetFloat() );

	return 1;
}

LUA_FUNCTION( GetConVarInt )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( (float) cvar->GetInt() );

	return 1;
}

LUA_FUNCTION( GetConVarName )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( cvar->GetName() );

	return 1;
}

LUA_FUNCTION( SetConVarName )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);
	Lua()->CheckType(2, GLua::TYPE_STRING);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	cvar->m_pszName = new char[50];
	strcpy( (char*)cvar->m_pszName, Lua()->GetString(2) );

	return 0;
}

LUA_FUNCTION( GetConVarString )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( cvar->GetString() );

	return 1;
}

LUA_FUNCTION( SetConVarFlags )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);
	Lua()->CheckType(2, GLua::TYPE_NUMBER);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	cvar->m_nFlags = Lua()->GetInteger(2);

	return 0;
}

LUA_FUNCTION( GetConVarFlags )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push((float)cvar->m_nFlags);

	return 1;
}

LUA_FUNCTION( ConVarHasFlag )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);
	Lua()->CheckType(2, GLua::TYPE_NUMBER);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( (bool) cvar->IsFlagSet( Lua()->GetInteger( 2 ) ) );

	return 1;
}

LUA_FUNCTION( SetConVarHelpText )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);
	Lua()->CheckType(2, GLua::TYPE_STRING);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	cvar->m_pszHelpString = new char[100];
	strcpy( (char*)cvar->m_pszHelpString, Lua()->GetString(2) );

	return 0;
}

LUA_FUNCTION( GetConVarHelpText )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	Lua()->Push( cvar->m_pszHelpString );

	return 1;
}

LUA_FUNCTION( ResetConVarValue )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	cvar->Revert();

	return 0;
}

LUA_FUNCTION( GetConVarMin )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	float min;
	bool hasMin = cvar->GetMin( min );

	if ( hasMin ) {
		Lua()->Push( min );
		return 1;
	}

	return 0;
}

LUA_FUNCTION( GetConVarMax )
{
	Lua()->CheckType(1, GLua::TYPE_CONVAR);

	ConVar *cvar = (ConVar*) Lua()->GetUserData(1);

	if (!cvar)
		Lua()->Error("Invalid ConVar handle!\n");

	float max;
	bool hasMax = cvar->GetMax( max );

	if ( hasMax ) {
		Lua()->Push(max);
		return 1;
	}

	return 0;
}

LUA_FUNCTION( RemoveConVar )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = (ConVar*) Lua()->GetUserData( 1 );

	if (!cvar)
		Lua()->Error("Invalid ConVar!\n");

	g_ICvar->UnregisterConCommand( cvar );
	delete cvar;
	return 0;
}

LUA_FUNCTION( ExecuteCommandOnClient )
{
	if ( !pServer )
		Lua()->Error("CVar3: pServer is NULL!");
	
	Lua()->CheckType(1, GLua::TYPE_ENTITY);
	Lua()->CheckType(2, GLua::TYPE_STRING);
	
	int clientID = GetPlayerMethodInt(L,1,"UserID");
	
	for (int i=0; i < pServer->GetClientCount(); i++) {

		IClient* client = pServer->GetClient(i);

		if( client->GetUserID() == clientID ) {
			client->ExecuteStringCommand( Lua()->GetString(2) );
			break;
		}
	}

	return 0;
}

LUA_FUNCTION( GetUserSetting )
{
	if ( !pServer )
		Lua()->Error("CVar3: pServer is NULL!");

	Lua()->CheckType(1, GLua::TYPE_ENTITY);
	Lua()->CheckType(2, GLua::TYPE_STRING);

	int clientID = GetPlayerMethodInt(L,1,"UserID");
	
	for (int i=0; i < pServer->GetClientCount(); i++) {
		IClient* client = pServer->GetClient(i);

		if( client->GetUserID() == clientID ) {
			Lua()->Push( client->GetUserSetting( Lua()->GetString(2) ) );
			return 1;
		}
	}

	return 0;
}

LUA_FUNCTION( ReplicateData )
{
	if ( !pServer )
		Lua()->Error("CVar3: pServer is NULL!");

	Lua()->CheckType(1, GLua::TYPE_ENTITY);
	Lua()->CheckType(2, GLua::TYPE_STRING);
	Lua()->CheckType(3, GLua::TYPE_STRING);

	int clientID = GetPlayerMethodInt(L,1,"UserID");
	
	for (int i=0; i < pServer->GetClientCount(); i++) {

		IClient* client = pServer->GetClient(i);

		if( client->GetUserID() == clientID ) {

			INetChannel *netChan = client->GetNetChannel();

			char pckBuf[256];
			bf_write pck( pckBuf, sizeof( pckBuf ) );
			
			pck.WriteUBitLong( 5, 5 );
			pck.WriteByte( 0x01 );
			pck.WriteString( Lua()->GetString(2) );
			pck.WriteString( Lua()->GetString(3) );

			netChan->SendData (pck );
			break;
		}		
	}

	return 0;
}

LUA_FUNCTION( __eq )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );
	Lua()->CheckType( 2, GLua::TYPE_CONVAR );

	ConVar *cvar1 = ( ConVar* ) Lua()->GetUserData(1);
	ConVar *cvar2 = ( ConVar* ) Lua()->GetUserData(2);
	
	if ( cvar1 == cvar2 )
		Lua()->Push( (bool) true );
	else
		Lua()->Push( (bool) false );

	return 1;
}

LUA_FUNCTION( __tostring )
{
	Lua()->CheckType( 1, GLua::TYPE_CONVAR );

	ConVar *cvar = ( ConVar* ) Lua()->GetUserData(1);

	if ( !cvar )
		Lua()->Error("Invalid ConVar!\n");
	
	Lua()->PushVA( "ConVar: %p", cvar );

	return 1;
}

LUA_FUNCTION( GetConVar )
{
	Lua()->CheckType(1, GLua::TYPE_STRING);

	ConVar *ConVar = g_ICvar->FindVar( Lua()->GetString( 1 ) );

	ILuaObject *metaT = Lua()->GetMetaTable( "ConVar", GLua::TYPE_CONVAR );
		Lua()->PushUserData( metaT, cvar );
	metaT->UnReference();

	return 1;
}

LUA_FUNCTION( GetCommand )
{
	Lua()->CheckType(1, GLua::TYPE_STRING);

	ConCommand *cvar = g_ICvar->FindCommand( Lua()->GetString( 1 ) );

	ILuaObject *metaT = Lua()->GetMetaTable( "ConVar", GLua::TYPE_CONVAR );
		Lua()->PushUserData( metaT, cvar );
	metaT->UnReference();

	return 1;
}

int Open( lua_State *L ) {
	
	CreateInterfaceFn VSTDLibFactory = Sys_GetFactory( VSTD_LIB );
	if ( !VSTDLibFactory )
		Lua()->Error( "gm_cvar3: Error getting " VSTD_LIB " factory.\n" );

	g_ICvar = ( ICvar* )VSTDLibFactory( CVAR_INTERFACE_VERSION, NULL );
	if ( !g_ICvar )
		Lua()->Error( "gm_cvar3: Error getting ICvar interface.\n" );

	if ( Lua()->IsServer() ) {
		// rip from gatekeeper
		CSigScan::sigscan_dllfunc = Sys_GetFactory( ENGINE_LIB );
	
		if ( !CSigScan::GetDllMemInfo() )
			Lua()->Error("gm_cvar3: CSigScan::GetDllMemInfo failed!");

		CSigScan sigBaseServer;
		
		sigBaseServer.Init( (unsigned char *)
		"\x00\x00\x00\x00" "\x88\x1D\x00\x00\x00\x00" "\xE8\x00\x00\x00\x00" "\xD9\x1D",
		"????xx????x????xx",
		17 );

		if ( !sigBaseServer.is_set )
			Lua()->Error("gm_cvar3: CBaseServer signature failed!");
		else {
			pServer = *(CBaseServer**) sigBaseServer.sig_addr;

			ILuaObject* playerMeta = Lua()->GetMetaTable( "Player", GLua::TYPE_ENTITY );
			if( playerMeta ) {
				playerMeta->SetMember( "ExecuteCommandOnClient", ExecuteCommandOnClient );
				playerMeta->SetMember( "GetUserSetting", GetUserSetting );
				playerMeta->SetMember( "ReplicateData", ReplicateData );
			}
			playerMeta->UnReference();
		}
	}

	Lua()->SetGlobal( "FCVAR_DEVELOPMENTONLY", (float) FCVAR_DEVELOPMENTONLY );
	Lua()->SetGlobal( "FCVAR_HIDDEN", (float) FCVAR_HIDDEN );

	Lua()->NewGlobalTable("cvars");
	ILuaObject* cvars = Lua()->GetGlobal( "cvars" );
		cvars->SetMember( "GetAllConVars", GetAllCvars );
		cvars->SetMember( "GetAllCommands", GetAllCmds );
		cvars->SetMember( "GetCommand", GetCommand );
	cvars->UnReference();
	
	ILuaObject* conVarMeta = Lua()->GetMetaTable( "ConVar", GLua::TYPE_CONVAR );
	if( conVarMeta ) {
		conVarMeta->SetMember( "SetValue", SetConVarValue );
		conVarMeta->SetMember( "GetBool", GetConVarBool );
		conVarMeta->SetMember( "GetDefault", GetConVarDefault );
		conVarMeta->SetMember( "GetFloat", GetConVarFloat );
		conVarMeta->SetMember( "GetInt", GetConVarInt );
		conVarMeta->SetMember( "GetName", GetConVarName );
		conVarMeta->SetMember( "SetName", SetConVarName );
		conVarMeta->SetMember( "GetString", GetConVarString );
		conVarMeta->SetMember( "SetFlags", SetConVarFlags );
		conVarMeta->SetMember( "GetFlags", GetConVarFlags );
		conVarMeta->SetMember( "HasFlag", ConVarHasFlag );
		conVarMeta->SetMember( "SetHelpText", SetConVarHelpText );
		conVarMeta->SetMember( "GetHelpText", GetConVarHelpText );
		conVarMeta->SetMember( "ResetValue", ResetConVarValue );
		conVarMeta->SetMember( "Remove", RemoveConVar );
		conVarMeta->SetMember( "__tostring", __tostring );
		conVarMeta->SetMember( "__eq", __eq );
		conVarMeta->SetMember( "__index", conVarMeta );
	}
	conVarMeta->UnReference();

	// Was making my own convar class before I realized I could just extend the already existing one..

	/*
	Lua()->SetGlobal( "ConVar3", __new );

	ILuaObject *metaT = Lua()->GetMetaTable( CVAR3_NAME, CVAR3_ID );
		metaT->SetMember( "SetValue", SetConVarValue );
		metaT->SetMember( "GetBool", GetConVarBool );
		metaT->SetMember( "GetDefault", GetConVarDefault );
		metaT->SetMember( "GetFloat", GetConVarFloat );
		metaT->SetMember( "GetInt", GetConVarInt );
		metaT->SetMember( "GetName", GetConVarName );
		metaT->SetMember( "GetString", GetConVarString );
		metaT->SetMember( "SetFlags", SetConVarFlags );
		metaT->SetMember( "GetFlags", GetConVarFlags );
		metaT->SetMember( "HasFlag", ConVarHasFlag );
		metaT->SetMember( "SetHelpText", SetConVarHelpText );
		metaT->SetMember( "GetHelpText", GetConVarHelpText );
		metaT->SetMember( "ResetValue", ResetConVarValue );
		metaT->SetMember( "__tostring", __tostring );
		metaT->SetMember( "__eq", __eq );
		metaT->SetMember( "__index", metaT );
	metaT->UnReference();
	*/

	return 0;
}

int Close(lua_State *L)
{
	return 0;
}