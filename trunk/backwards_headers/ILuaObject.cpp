#include "ILuaObject.h"

ILuaObject::ILuaObject( ILuaBase* lua, int iRef ) : m_pLua(lua), m_iRef(iRef)
{
}

ILuaObject::ILuaObject( ILuaBase* lua, ILuaObject* obj ) : m_pLua(lua), m_iRef(obj->m_iRef)
{
	
}

ILuaObject::~ILuaObject()
{
	m_pLua->ReferenceFree( m_iRef );
}

void ILuaObject::UnReference()
{
	delete this;
}

int ILuaObject::GetReference()
{
	return m_iRef;
}

int ILuaObject::GetType()
{
	Push();
	int ret = m_pLua->GetType( -1 );
	m_pLua->Pop();
	return ret;
}

const char* ILuaObject::GetTypeName()
{
	Push();
	const char* ret = m_pLua->GetTypeName( m_pLua->GetType( -1 ) );
	m_pLua->Pop();
	return ret;
}

void ILuaObject::SetMember( const char* name, ILuaObject* obj )
{
	Push();
	m_pLua->PushString( name );
	obj->Push();
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, double d )
{
	Push();
	m_pLua->PushString( name );
	m_pLua->PushNumber( d );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, bool b )
{
	Push();
	m_pLua->PushString( name );
	m_pLua->PushBool( b );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, const char* s )
{
	Push();
	m_pLua->PushString( name );
	m_pLua->PushString( s );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, CFunc f )
{
	Push();
	m_pLua->PushString( name );
	m_pLua->PushCFunction( f );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetUserData( void* obj )
{
	Push();
	m_pLua->NewUserdata( sizeof( obj ) );
	m_pLua->PushUserdata( obj );
	m_pLua->GetUserdata( -3 );
}

bool ILuaObject::isType( int iType )
{
	Push();
	bool ret = m_pLua->GetType(-1) == iType;
	m_pLua->Pop();
	return ret;
}

bool ILuaObject::isNil()
{
	return isType( Type::NIL );
}

bool ILuaObject::isTable()
{
	return isType( Type::TABLE );
}

bool ILuaObject::isString()
{
	return isType( Type::STRING );
}

bool ILuaObject::isNumber()
{
	return isType( Type::NUMBER );
}

bool ILuaObject::isFunction()
{
	return isType( Type::FUNCTION );
}

bool ILuaObject::isUserData()
{
	return isType( Type::USERDATA );
}

void ILuaObject::Push()
{
	m_pLua->ReferencePush( m_iRef );
}

