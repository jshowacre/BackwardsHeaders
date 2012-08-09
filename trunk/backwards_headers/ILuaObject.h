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

	const char*		GetString();
	int				GetInt();
	double			GetDouble();
	float			GetFloat();
	bool			GetBool();
	void*			GetUserData();
	
	void			SetMember( const char* name );
	void			SetMember( const char* name, ILuaObject* obj );
	void			SetMember( const char* name, double d );
	void			SetMember( const char* name, bool b );
	void			SetMember( const char* name, const char* s );
	void			SetMember( const char* name, CFunc f );
	
	void			SetMember( double fKey );
	void			SetMember( double fKey, ILuaObject* obj );
	void			SetMember( double fKey, double d );
	void			SetMember( double fKey, bool b );
	void			SetMember( double fKey, const char* s );
	void			SetMember( double fKey, CFunc f );

	ILuaObject*		GetMember( const char* name );

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