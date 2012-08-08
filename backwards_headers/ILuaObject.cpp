#include "ILuaObject.h"

ILuaObject::ILuaObject( ILuaBase* lua, int iRef ) : m_pLua(lua), m_iRef(iRef)
{
}

ILuaObject::ILuaObject( ILuaBase* lua, ILuaObject* obj ) : m_pLua(lua), m_iRef(obj->m_iRef)
{
	
}

ILuaObject::~ILuaObject()
{

}

void ILuaObject::UnReference()
{
	m_pLua->ReferenceFree( m_iRef );
	delete this;
}

int ILuaObject::GetReference()
{
	return m_iRef;
}

int ILuaObject::GetType()
{
	m_pLua->ReferencePush( m_iRef );
	return m_pLua->GetType( -1 );
}

const char* ILuaObject::GetTypeName()
{
	m_pLua->ReferencePush( m_iRef );
	return m_pLua->GetTypeName( m_pLua->GetType( -1 ) );
}

void ILuaObject::SetMember( const char* name, ILuaObject* obj )
{
	m_pLua->ReferencePush( m_iRef );
	m_pLua->PushString( name );
	m_pLua->ReferencePush( obj->GetReference() );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, double d )
{
	m_pLua->ReferencePush( m_iRef );
	m_pLua->PushString( name );
	m_pLua->PushNumber( d );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, bool b )
{
	m_pLua->ReferencePush( m_iRef );
	m_pLua->PushString( name );
	m_pLua->PushBool( b );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, const char* s )
{
	m_pLua->ReferencePush( m_iRef );
	m_pLua->PushString( name );
	m_pLua->PushString( s );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, CFunc f )
{
	m_pLua->ReferencePush( m_iRef );
	m_pLua->PushString( name );
	m_pLua->PushCFunction( f );
	m_pLua->SetTable( -3 );
}


void ILuaObject::Push()
{
	m_pLua->ReferencePush( m_iRef );
}

