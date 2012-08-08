#ifndef ILUAINTERFACE_H
#define ILUAINTERFACE_H

#define GMMODULE
#include <stdio.h>
#include <stdarg.h>

#include "ILuaObject.h"
#include "Interface.h"

using namespace GarrysMod::Lua;

class ILuaInterface
{
public:
	ILuaInterface( lua_State* state );
	~ILuaInterface( void );

	lua_State*		GetLuaState();
	ILuaObject*		Global();
	ILuaObject*		GetNewTable();
	void			NewTable();

	void			Error( const char* strError );

	void			SetGlobal( const char* name, CFunc f );
	void			SetGlobal( const char* name, double d );
	void			SetGlobal( const char* name, const char* s );
	void			SetGlobal( const char* name, bool b );
	void			SetGlobal( const char* name, void* u );
	void			SetGlobal( const char* name, ILuaObject* o );

	ILuaObject*		GetObject( int i = -1 );
	const char*		GetString( int i = -1 );
	int				GetInteger( int i = -1 );
	double			GetNumber( int i = -1 );
	double			GetDouble( int i = -1 );
	float			GetFloat( int i = -1 );
	bool			GetBool( int i = -1 );
	void*			GetUserData( int i = -1 );

	int				GetReference( int i = -1 );
	void			FreeReference( int i );
	void			PushReference( int i );

	void			Pop( int i=1 );
	int				Top();

	void			Push( ILuaObject* o );
	void			Push( const char* s );
	void			PushVA( const char* str, ... );
	void			Push( double d );
	void			Push( bool b );
	void			Push( CFunc f );
	void			PushNil();

	void			PushUserData( ILuaObject* metatable, void * v );

	void			CheckType( int i, int iType );
	int				GetType( int iStackPos );
	const char*		GetTypeName( int iType );

	ILuaObject*		GetReturn( int iNum );

	void			Call( int args, int returns = 0 );

	ILuaObject*		GetMetaTable( const char* strName, int iType );
	ILuaObject*		GetMetaTable( int i );

private:
	lua_State*		m_pState;
	ILuaBase*		m_pLua;
	ILuaObject*		m_pG;
};

#endif