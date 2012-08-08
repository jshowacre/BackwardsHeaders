#ifndef ILUAOBJECT_H
#define ILUAOBJECT_H

#include "gmod13/include/GarrysMod/Lua/Interface.h"

using namespace GarrysMod::Lua;

class ILuaObject
{
public:
	ILuaObject( ILuaBase* state, int iRef );
	ILuaObject( ILuaBase* state, ILuaObject* obj );
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

	void			Push();

private:
	ILuaBase* m_pState;
	int m_iRef;
};

#endif