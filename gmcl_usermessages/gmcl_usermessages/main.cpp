#undef _UNICODE

#pragma comment (lib, "tier0.lib")
#pragma comment (lib, "tier1.lib")
#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#ifdef _LINUX
	#define ENGINE_LIB "engine.so"
	#define CLIENT_LIB "client.so"
#else
	#define ENGINE_LIB "engine.dll"
	#define CLIENT_LIB "client.dll"
#endif

#define CBITREAD_NAME "CBitRead"
#define CBITREAD_ID 5844

#define CLIENT_DLL

#include "gmod/CStateManager/vfnhook.h"

//Lua module interface
#include "interface.h"
#include "gmod/GMLuaModule.h"
#include "gmod/CStateManager/CStateManager.h"

#include "eiface.h"
#include "cdll_client_int.h"

#include "usermessages.h"

IBaseClientDLL *g_BaseClientDLL = NULL;

GMOD_MODULE( Init, Shutdown );

/*extern CUserMessages *usermessages;
LUA_FUNCTION( LookupUserMessage )
{
	Lua()->CheckType( 1, GLua::TYPE_STRING );
	Lua()->Push( (float) usermessages->LookupUserMessage( Lua()->GetString(1) ) );
	return 1;
}

LUA_FUNCTION( GetUserMessageSize )
{
	Lua()->CheckType( 1, GLua::TYPE_NUMBER );
	Lua()->Push( (float) usermessages->GetUserMessageSize( Lua()->GetInteger(1) ) );
	return 1;
}

LUA_FUNCTION( GetUserMessageName )
{
	Lua()->CheckType( 1, GLua::TYPE_NUMBER );
	Lua()->Push( usermessages->GetUserMessageName( Lua()->GetInteger(1) ) );
	return 1;
}

LUA_FUNCTION( IsValidIndex )
{
	Lua()->CheckType( 1, GLua::TYPE_NUMBER );
	Lua()->Push( usermessages->IsValidIndex( Lua()->GetInteger(1) ) );
	return 1;
}*/

/*
LUA_FUNCTION( HookMessage )
{
	Lua()->CheckType( 1, GLua::TYPE_STRING );
	Lua()->Push( usermessages->HookMessage( Lua()->GetInteger(1) ) )
	return 1;
}
*/

LUA_FUNCTION( GetSize )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	
	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->m_nDataBytes );
	return 1;
}

LUA_FUNCTION( GetDebugName )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	
	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( msg_data->m_pDebugName );
	return 1;
}

LUA_FUNCTION( GetNumBitsLeft )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Lua()->Push( (float) msg_data->GetNumBitsLeft() );
	return 1;
}

LUA_FUNCTION( GetNumBitsRead )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Lua()->Push( (float) msg_data->GetNumBitsRead() );
	return 1;
}

LUA_FUNCTION( GetNumBytesLeft )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Lua()->Push( (float) msg_data->GetNumBytesLeft() );
	return 1;
}

LUA_FUNCTION( IsOverflowed )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Lua()->Push( (bool) msg_data->IsOverflowed() );
	return 1;
}

LUA_FUNCTION( PeekUBitLong )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Lua()->Push( (float) msg_data->PeekUBitLong( Lua()->GetInteger(2) ) );
	return 1;
}

LUA_FUNCTION( ReadAndAllocateString )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	//Lua()->CheckType( 2, GLua::TYPE_BOOL );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Lua()->Push( msg_data->ReadAndAllocateString( false ) ); // True = Crash
	return 1;
}

LUA_FUNCTION( ReadBitAngle )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Lua()->Push( msg_data->ReadBitAngle( Lua()->GetInteger(2) ) );
	return 1;
}

LUA_FUNCTION( ReadBitAngles )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	QAngle readAng;
	msg_data->ReadBitAngles( readAng );

	ILuaObject *metaT = Lua()->GetMetaTable( "Angle", GLua::TYPE_ANGLE );
		Lua()->PushUserData( metaT, &readAng );
	metaT->UnReference();
	
	return 1;
}

LUA_FUNCTION( ReadBitCoord )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( msg_data->ReadBitCoord() );
	return 1;
}


LUA_FUNCTION( ReadBitCoordMP )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_BOOL );
	Lua()->CheckType( 3, GLua::TYPE_BOOL );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( msg_data->ReadBitCoordMP( Lua()->GetBool(1), Lua()->GetBool(2) ) );
	return 1;
}


LUA_FUNCTION( ReadBitFloat )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( msg_data->ReadBitFloat() );
	return 1;
}

LUA_FUNCTION( ReadBitNormal )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( msg_data->ReadBitNormal() );
	return 1;
}

LUA_FUNCTION( ReadBitVec3Coord )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Vector readVec; 
	msg_data->ReadBitVec3Coord( readVec );

	ILuaObject *metaT = Lua()->GetMetaTable( "Vector", GLua::TYPE_VECTOR );
		Lua()->PushUserData( metaT, &readVec );
	metaT->UnReference();
	return 1;
}

LUA_FUNCTION( ReadBitVec3Normal )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	Vector readVec; 
	msg_data->ReadBitVec3Normal( readVec );

	ILuaObject *metaT = Lua()->GetMetaTable( "Vector", GLua::TYPE_VECTOR );
		Lua()->PushUserData( metaT, &readVec );
	metaT->UnReference();
	return 1;
}

LUA_FUNCTION( ReadByte )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadByte() );
	return 1;
}

LUA_FUNCTION( ReadChar )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadChar() );
	return 1;
}

LUA_FUNCTION( ReadFloat )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( msg_data->ReadFloat() );
	return 1;
}

LUA_FUNCTION( ReadLong )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadLong() );
	return 1;
}

LUA_FUNCTION( ReadOneBit )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadOneBit() );
	return 1;
}

LUA_FUNCTION( ReadSBitLong )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadSBitLong( Lua()->GetInteger(2) ) );
	return 1;
}

LUA_FUNCTION( ReadShort )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadShort() );
	return 1;
}

LUA_FUNCTION( ReadString )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	char szString[ 2048 ];
	
	int optionalSize = Lua()->GetInteger(2);
	int buffReadSize = optionalSize != NULL ? clamp( optionalSize, 0, 2048 ) : sizeof(szString);
	
	msg_data->ReadString( szString, buffReadSize );

	Lua()->Push( szString );
	return 1;
}

LUA_FUNCTION( ReadUBitLong )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadUBitLong( Lua()->GetInteger(2) ) );
	return 1;
}

LUA_FUNCTION( ReadUBitVar )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadUBitVar() );
	return 1;
}

LUA_FUNCTION( ReadWord )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->ReadWord() );
	return 1;
}

LUA_FUNCTION( Seek )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (bool) msg_data->Seek( Lua()->GetInteger(2) ) );
	return 1;
}

LUA_FUNCTION( SeekRelative )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (bool) msg_data->SeekRelative( Lua()->GetInteger(2) ) );
	return 1;
}

LUA_FUNCTION( SetDebugName )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	msg_data->SetDebugName( Lua()->GetString(2) );
	return 0;
}

LUA_FUNCTION( SetOverflowFlag )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	msg_data->SetOverflowFlag();
	return 0;
}

LUA_FUNCTION( Tell )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->Tell() );
	return 1;
}

LUA_FUNCTION( TotalBytesAvailable )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);
	
	Lua()->Push( (float) msg_data->TotalBytesAvailable() );
	return 1;
}

LUA_FUNCTION( __eq )
{
	Lua()->CheckType( 1, CBITREAD_ID );
	Lua()->CheckType( 2, CBITREAD_ID );

	bf_read *msg_data1 = ( bf_read* ) Lua()->GetUserData(1);
	bf_read *msg_data2 = ( bf_read* ) Lua()->GetUserData(2);

	if ( msg_data1 == msg_data2 )
		Lua()->Push( (bool) true );
	else
		Lua()->Push( (bool) false );

	return 1;
}


LUA_FUNCTION( __tostring )
{
	Lua()->CheckType( 1, CBITREAD_ID );

	bf_read *msg_data = ( bf_read* ) Lua()->GetUserData(1);

	if ( !msg_data )
		Lua()->Error( "gmcl_usermessages: Invalid " CBITREAD_NAME "!\n");
	
	Lua()->PushVA( "%s: %p", CBITREAD_NAME, msg_data );

	return 1;
}

DEFVFUNC_( origDispatchUserMessage, void, ( IBaseClientDLL *baseCLDLL, int peebis, int msg_type, bf_read &msg_data ) );

void VFUNC newDispatchUserMessage( IBaseClientDLL *baseCLDLL, int peebis, int msg_type, bf_read &msg_data ) {

	for ( int i=0; i <= 1; i++) {
		ILuaInterface* gLua = CStateManager::GetByIndex( i );
		if ( gLua == NULL ){ continue; }

		gLua->Push( gLua->GetGlobal("hook")->GetMember("Call") );
			gLua->Push("DispatchUserMessage");
			gLua->PushNil();

			gLua->Push( (float) peebis );
			gLua->Push( (float) msg_type );

			ILuaObject *metaT = gLua->GetMetaTable( CBITREAD_NAME, CBITREAD_ID ); //Push our custom stringtable object
				gLua->PushUserData( metaT, &msg_data );
			metaT->UnReference();

		gLua->Call(4, 0);
	}

	return origDispatchUserMessage( baseCLDLL, peebis, msg_type, msg_data );
}

int Init(lua_State *L) {

	if ( !Lua()->IsClient() )
		Lua()->Error( "gmcl_usermessages can only be loaded on the client!" );

	CreateInterfaceFn ClientFactory = Sys_GetFactory( CLIENT_LIB );
	if ( !ClientFactory )
		Lua()->Error( "gmcl_usermessages: Error getting " CLIENT_LIB " factory." );

	g_BaseClientDLL = ( IBaseClientDLL* )ClientFactory( CLIENT_DLL_INTERFACE_VERSION, NULL );
	if ( !g_BaseClientDLL )
		Lua()->Error( "gmcl_usermessages: Error getting IBaseClientDLL interface.\n" );
	
	HOOKVFUNC( g_BaseClientDLL, 36, origDispatchUserMessage, newDispatchUserMessage );
	
	Color Blue( 0, 162, 232, 255 );
	Color White( 255, 255, 255, 255 );

	ConColorMsg( Blue, "gmcl_usermessages" );
	ConColorMsg( White, ": Loaded!\n" );

	/*
	Lua()->NewGlobalTable("usermessages");
		ILuaObject *usermessages = Lua()->GetGlobal("usermessages");
		usermessages->SetMember( "Lookup", LookupUserMessage );
		usermessages->SetMember( "GetSize", GetUserMessageSize );
		usermessages->SetMember( "GetName", GetUserMessageName );
		usermessages->SetMember( "IsValidIndex", IsValidIndex );
	usermessages->UnReference();
	*/

	ILuaObject *metaT = Lua()->GetMetaTable( CBITREAD_NAME, CBITREAD_ID );
		metaT->SetMember( "GetSize", GetSize );
		metaT->SetMember( "GetDebugName", GetDebugName );
		metaT->SetMember( "GetNumBitsLeft", GetNumBitsLeft );
		metaT->SetMember( "GetNumBitsRead", GetNumBitsRead );
		metaT->SetMember( "GetNumBytesLeft", GetNumBytesLeft );
		metaT->SetMember( "IsOverflowed", IsOverflowed );
		metaT->SetMember( "PeekUBitLong", PeekUBitLong );
		metaT->SetMember( "ReadAndAllocateString", ReadAndAllocateString );
		metaT->SetMember( "ReadBitAngle", ReadBitAngle );
		metaT->SetMember( "ReadBitAngles", ReadBitAngles );
		metaT->SetMember( "ReadBitCoord", ReadBitCoord );
		metaT->SetMember( "ReadBitCoordMP", ReadBitCoordMP );
		metaT->SetMember( "ReadBitFloat", ReadBitFloat );
		metaT->SetMember( "ReadBitNormal", ReadBitNormal );
		metaT->SetMember( "ReadBitVec3Coord", ReadBitVec3Coord );
		metaT->SetMember( "ReadBitVec3Normal", ReadBitVec3Normal );
		metaT->SetMember( "ReadByte", ReadByte );
		metaT->SetMember( "ReadChar", ReadChar );
		metaT->SetMember( "ReadFloat", ReadFloat );
		metaT->SetMember( "ReadLong", ReadLong );
		metaT->SetMember( "ReadOneBit", ReadOneBit );
		metaT->SetMember( "ReadSBitLong", ReadSBitLong );
		metaT->SetMember( "ReadShort", ReadShort );
		metaT->SetMember( "ReadString", ReadString );
		metaT->SetMember( "ReadUBitLong", ReadUBitLong );
		metaT->SetMember( "ReadUBitVar", ReadUBitVar );
		metaT->SetMember( "ReadWord", ReadWord );
		metaT->SetMember( "Seek", Seek );
		metaT->SetMember( "SeekRelative", SeekRelative );
		metaT->SetMember( "SetDebugName", SetDebugName );
		metaT->SetMember( "SetOverflowFlag", SetOverflowFlag );
		metaT->SetMember( "Tell", Tell );
		metaT->SetMember( "TotalBytesAvailable", TotalBytesAvailable );

		metaT->SetMember( "__tostring", __tostring );
		metaT->SetMember( "__eq", __eq );
		metaT->SetMember( "__index", metaT );
	metaT->UnReference();

	return 0;
}

int Shutdown(lua_State *L) {
	UNHOOKVFUNC( g_BaseClientDLL, 36, origDispatchUserMessage );
	return 0;
}