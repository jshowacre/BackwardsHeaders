#ifndef ILUAINTERFACE_H
#define ILUAINTERFACE_H

#include "ILuaObject.h"
#include "Interface.h"

#define GMOD_MODULE( _startfunction_, _closefunction_ ) \
	int _startfunction_( lua_State* L );\
	int _closefunction_( lua_State* L );\
	DLL_EXPORT int gmod13_open( lua_State* L ) \
	{ \
		modulemanager->CreateInterface(L);\
		return _startfunction_(L);\
	} \
	DLL_EXPORT int gmod13_close( lua_State* L ) \
	{ \
		modulemanager->DestroyInterface(L);\
		return _closefunction_(L);\
	} \


#define LUA_FUNCTION( _function_ ) int _function_( lua_State* L ) // Compatablity, also I find it neater

using namespace GarrysMod::Lua;

class ILuaInterface
{
public:
	ILuaInterface( ILuaBase* state );
	~ILuaInterface( void );

	ILuaBase*		GetLua();
	ILuaObject*		Global();
	ILuaObject*		GetNewTable();

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
	bool			GetBool( int i = -1 );
	void*			GetUserData( int i = -1 );

	int				GetReference( int i = -1 );
	void			FreeReference( int i );
	void			PushReference( int i );

	void			Pop( int i=1 );
	int				Top();

	void			Push( ILuaObject* o );
	void			Push( const char* s );
	void			Push( double d );
	void			Push( bool b );
	void			Push( CFunc f );
	void			PushNil();

	void			PushUserData( ILuaObject* metatable, void * v );

	void			CheckType( int i, int iType );

	ILuaObject*		GetMetaTable( const char* strName, int iType );
	//ILuaObject*		GetMetaTable( int i );

private:
	ILuaBase* m_pState;
};

#endif