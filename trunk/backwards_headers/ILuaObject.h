#ifndef ILUAOBJECT_H
#define ILUAOBJECT_H

#include "Interface.h"

using namespace GarrysMod::Lua;

class ILuaObject
{
public:
	ILuaObject( ILuaBase* lua, int iRef );
	ILuaObject( ILuaBase* lua, ILuaObject* obj );
	~ILuaObject( void );
	
	void			UnReference();
	int				GetReference();
	int				GetType();
	const char*		GetTypeName();
	
	void			SetMember( const char* name, ILuaObject* obj );
	void			SetMember( const char* name, double d );
	void			SetMember( const char* name, bool b );
	void			SetMember( const char* name, const char* s );
	void			SetMember( const char* name, CFunc f );

	void			SetUserData( void* obj );
	
	bool			isType( int iType );
	bool			isNil();
	bool			isTable();
	bool			isString();
	bool			isNumber();
	bool			isFunction();
	bool			isUserData();

	void			Push();

private:
	ILuaBase* m_pLua;
	int m_iRef;
};

#endif