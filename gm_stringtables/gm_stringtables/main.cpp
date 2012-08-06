#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#ifdef _LINUX
	#define ENGINE_LIB "engine.so"
	#define CLIENT_LIB "client.so"
#else
	#define ENGINE_LIB "engine.dll"
	#define CLIENT_LIB "client.dll"
#endif

#define STRINGTABLE_NAME "StringTable"
#define STRINGTABLE_ID 5843

#undef _UNICODE

#include <windows.h>

#define GMOD_BETA
#define CLIENT_DLL

//String stuff
#include <string>
#include <sstream>

#include "gmod/CStateManager/vfnhook.h"

//Lua module interface
#include "interface.h"
#include "gmod/GMLuaModule.h"
#include "gmod/CStateManager/CEasyState.h"

#include "filesystem.h"
#include "cdll_client_int.h"
#include "networkstringtabledefs.h"

IBaseClientDLL *g_BaseClientDLL = NULL;
INetworkStringTableContainer *g_NetworkStringTable = NULL;

GMOD_MODULE( Init, Shutdown );

LUA_FUNCTION(DumpInfo)
{
	Lua()->CheckType( 1, STRINGTABLE_ID );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl )
		Lua()->Msg("%d %s %d %d %d\n", netTbl->a, netTbl->b, netTbl->c, netTbl->d);
	else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION(SetTableSize)
{
	Lua()->CheckType(1, GLua::TYPE_STRING);
	Lua()->CheckType(2, GLua::TYPE_NUMBER);

	INetworkStringTable *netTbl = g_NetworkStringTable->FindTable(Lua()->GetString(1));

	if(netTbl) {
		netTbl->c = Lua()->GetInteger(2);
		netTbl->d = Q_log2(netTbl->c);
	}

	return 0;
}

LUA_FUNCTION(GetTableNames)
{
	ILuaObject *tbl = Lua()->GetNewTable();

	for(int i = 0; i < g_NetworkStringTable->GetNumTables(); i++)
		tbl->SetMember(i, g_NetworkStringTable->GetTable(i)->GetTableName());

	Lua()->Push(tbl);

	tbl->UnReference();

	return 1;
}

LUA_FUNCTION( GetTable )
{
	Lua()->CheckType( 1, STRINGTABLE_ID );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		ILuaObject *tbl = Lua()->GetNewTable();

		for(int i = 0; i < netTbl->GetNumStrings(); i++) {
			int b;
			tbl->SetMember(netTbl->GetString(i), (const char*)netTbl->GetStringUserData(i, &b));
		}

		Lua()->Push(tbl);

		tbl->UnReference();
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( GetName ) {
	Lua()->CheckType( 1, STRINGTABLE_ID );
	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		Lua()->Push( netTbl->GetTableName() );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( GetTableId ) {
	Lua()->CheckType( 1, STRINGTABLE_ID );
	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		Lua()->Push( (float) netTbl->GetTableId() );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( GetNumStrings ) {
	Lua()->CheckType( 1, STRINGTABLE_ID );
	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		Lua()->Push( (float) netTbl->GetNumStrings() );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");
	
	return 0;
}

LUA_FUNCTION( GetMaxStrings ) {
	Lua()->CheckType( 1, STRINGTABLE_ID );
	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		Lua()->Push( (float) netTbl->GetMaxStrings() );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( GetEntryBits ) {
	Lua()->CheckType( 1, STRINGTABLE_ID );
	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		Lua()->Push( (float) netTbl->GetEntryBits() );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( SetTick ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl )
		netTbl->SetTick( Lua()->GetInteger(2) );
	else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( ChangedSinceTick ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		Lua()->Push( netTbl->ChangedSinceTick( Lua()->GetInteger(2) ) );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");
	
	return 0;
}

LUA_FUNCTION( AddString ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_BOOL );
	Lua()->CheckType( 3, GLua::TYPE_STRING );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl )
		netTbl->AddString( Lua()->GetBool(2), Lua()->GetString(3) );
	else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( SetString ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_STRING );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		netTbl->SetStringUserData( Lua()->GetInteger(2), strlen( Lua()->GetString(3) ) + 1, Lua()->GetString(3) );
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( GetString ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		int b;
		Lua()->Push( (const char*) netTbl->GetStringUserData( Lua()->GetInteger(2), &b ) );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( GetBool ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		int b;
		Lua()->Push( ((int) netTbl->GetStringUserData( Lua()->GetInteger(2), &b ) == 1 ) ? true : false );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( GetNumber ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		int b;
		const int thing = (int) netTbl->GetStringUserData( Lua()->GetInteger(2), &b );
		Lua()->Push( (float) thing );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

/*LUA_FUNCTION( SetBool ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_BOOL );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		netTbl->SetStringUserData( Lua()->GetInteger(2), strlen( Lua()->GetString(3) ) + 1, (bool*) Lua()->GetBool(3) );
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}*/

LUA_FUNCTION( FindStringIndex ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl ) {
		Lua()->Push( (float) netTbl->FindStringIndex( Lua()->GetString(2) ) );
		return 1;
	} else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

LUA_FUNCTION( SetAllowClientSideAddString ) {

	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, GLua::TYPE_BOOL );

	INetworkStringTable *netTbl = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( netTbl )
		g_NetworkStringTable->SetAllowClientSideAddString( netTbl, Lua()->GetBool(2) );
	else
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	return 0;
}

void OnTableChanged( void *object, INetworkStringTable *stringTable, int stringNumber, const char *newString, void const *newData ) {

	for ( int i=0; i <= 1; i++) {
		ILuaInterface* gLua = CStateManager::GetByIndex( i );
		if ( gLua == NULL ){ continue; }

		ILuaObject* _G = gLua->Global();

		ILuaObject* hook = _G->GetMember("hook");
		ILuaObject* hookCall = hook->GetMember("Call");

		gLua->Push( hookCall );
			gLua->Push("OnStringTableChanged");
			gLua->PushNil();

			ILuaObject *metaT = gLua->GetMetaTable( STRINGTABLE_NAME, STRINGTABLE_ID ); //Push our custom stringtable object
				gLua->PushUserData( metaT, stringTable );
			metaT->UnReference();

			gLua->Push( (float) stringNumber );
			gLua->Push( newString );
			gLua->Push( (const char*) newData );

		gLua->Call(6, 0);

		hookCall->UnReference();
		hook->UnReference();

		_G->UnReference();
	}

}

DEFVFUNC_( origInstallStringTableCallback, void, ( IBaseClientDLL *baseCLDLL, char const *tableName ) );

void VFUNC newInstallStringTableCallback( IBaseClientDLL *baseCLDLL, char const *tableName ) {

	for ( int i=0; i <= 1; i++ ) {

		ILuaInterface* gLua = CStateManager::GetByIndex( i );
		if ( gLua == NULL ){ continue; }

#ifdef GMOD_BETA
		ILuaObject* _G = gLua->Global();
#else
		ILuaObject* _G = gLua->GetGlobal("_G");
#endif

		ILuaObject* hook = _G->GetMember("hook");
		ILuaObject* hookCall = hook->GetMember("Call");

		gLua->Push( hookCall );
			gLua->Push("InstallStringTableCallback");
			gLua->PushNil();
			gLua->Push( tableName );
		gLua->Call(3, 1);

		ILuaObject* retrn = gLua->GetReturn(0);

		if ( !retrn->isNil() && retrn->GetType() == GLua::TYPE_BOOL && retrn->GetBool() == true ) {
			INetworkStringTable *netTbl = g_NetworkStringTable->FindTable( tableName );
			netTbl->SetStringChangedCallback( NULL, OnTableChanged );
		}
		
		retrn->UnReference();
		hookCall->UnReference();
		hook->UnReference();

#ifndef GMOD_BETA
		_G->UnReference();
#endif
	}

	return origInstallStringTableCallback( baseCLDLL, tableName );
}

LUA_FUNCTION( __eq )
{
	Lua()->CheckType( 1, STRINGTABLE_ID );
	Lua()->CheckType( 2, STRINGTABLE_ID );

	INetworkStringTable *stringTable1 = ( INetworkStringTable* ) Lua()->GetUserData(1);
	INetworkStringTable *stringTable2 = ( INetworkStringTable* ) Lua()->GetUserData(2);
	
	if ( stringTable1 == stringTable2 )
		Lua()->Push( (bool) true );
	else
		Lua()->Push( (bool) false );

	return 1;
}


LUA_FUNCTION( __tostring )
{
	Lua()->CheckType( 1, STRINGTABLE_ID );

	INetworkStringTable *stringTable = ( INetworkStringTable* ) Lua()->GetUserData(1);

	if ( !stringTable )
		Lua()->Error( "gm_stringtables: Invalid StringTable!\n");
	
	Lua()->PushVA( "%s: %p", STRINGTABLE_NAME, stringTable );

	return 1;
}

LUA_FUNCTION( __new )
{
	Lua()->CheckType( 1, GLua::TYPE_STRING );

	INetworkStringTable *stringTable = g_NetworkStringTable->FindTable( Lua()->GetString(1) );

	if ( !stringTable )
		Lua()->Error("gm_stringtables: Invalid StringTable!\n");

	ILuaObject *metaT = Lua()->GetMetaTable( STRINGTABLE_NAME, STRINGTABLE_ID );
		Lua()->PushUserData( metaT, stringTable );
	metaT->UnReference();

	return 1;
}

int Init(lua_State *L) {

	ILuaInterface* gLua = Lua();

	CreateInterfaceFn EngineFactory = Sys_GetFactory( ENGINE_LIB );
	if ( !EngineFactory )
		gLua->Error( "gm_stringtables: Error getting " ENGINE_LIB " factory." );
	
	g_NetworkStringTable = ( INetworkStringTableContainer* )EngineFactory( INTERFACENAME_NETWORKSTRINGTABLECLIENT, NULL );
	if ( !g_NetworkStringTable )
		gLua->Error( "gm_stringtables: Error getting INetworkStringTableContainer interface." );

	if ( gLua->IsClient() ) {

		CreateInterfaceFn ClientFactory = Sys_GetFactory( CLIENT_LIB );
		if ( !ClientFactory )
			gLua->Error( "gm_stringtables: Error getting " CLIENT_LIB " factory." );

		g_BaseClientDLL = ( IBaseClientDLL* )ClientFactory( CLIENT_DLL_INTERFACE_VERSION, NULL );
		if ( !g_BaseClientDLL )
			gLua->Error( "gm_stringtables: Error getting IBaseClientDLL interface.\n" );
	
		HOOKVFUNC( g_BaseClientDLL, 34, origInstallStringTableCallback, newInstallStringTableCallback );
		
		Color Blue( 0, 162, 232, 255 );
		Color White( 255, 255, 255, 255 );

		ConColorMsg( Blue, "gm_stringtables" );
		ConColorMsg( White, ": Loaded!\n" );

	} else
		gLua->Msg( "gm_stringtables: Loaded!\n" );

#ifdef GMOD_BETA
		ILuaObject* _G = gLua->Global();
#else
		ILuaObject* _G = gLua->GetGlobal("_G");
#endif

	_G->SetMember( "GetAllStringTables", GetTableNames );
	_G->SetMember( "StringTable", __new );

	ILuaObject *metaT = gLua->GetMetaTable( STRINGTABLE_NAME, STRINGTABLE_ID );
		metaT->SetMember( "GetTable", GetTable );
		metaT->SetMember( "GetName", GetName );
		metaT->SetMember( "GetTableID", GetTableId );
		metaT->SetMember( "GetNumStrings", GetNumStrings );
		metaT->SetMember( "GetMaxStrings", GetMaxStrings );
		metaT->SetMember( "GetEntryBits", GetEntryBits );
		metaT->SetMember( "SetTick", SetTick );
		metaT->SetMember( "ChangedSinceTick", ChangedSinceTick );
		metaT->SetMember( "AddString", AddString );
		metaT->SetMember( "SetString", SetString );
		metaT->SetMember( "GetString", GetString );
		metaT->SetMember( "GetBool", GetBool );
		metaT->SetMember( "GetNumber", GetNumber );
		//metaT->SetMember( "SetBool", SetBool );
		metaT->SetMember( "FindStringIndex", FindStringIndex );
		metaT->SetMember( "SetAllowClientSideAddString", SetAllowClientSideAddString );
		metaT->SetMember( "DumpInfo", DumpInfo );
		metaT->SetMember( "__tostring", __tostring );
		metaT->SetMember( "__eq", __eq );
		metaT->SetMember( "__index", metaT );
	metaT->UnReference();

#ifndef GMOD_BETA
	_G->UnReference();
#endif

	return 0;
}

int Shutdown(lua_State *L) {
	if ( Lua()->IsClient() )
		UNHOOKVFUNC( g_BaseClientDLL, 34, origInstallStringTableCallback );
	return 0;
}