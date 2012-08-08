#include "ILuaObject.h"

ILuaObject::ILuaObject( ILuaBase* state, int iRef ) : m_pState(state), m_iRef(iRef)
{
}

ILuaObject::ILuaObject( ILuaBase* state, ILuaObject* obj ) : m_pState(state), m_iRef(obj->m_iRef)
{
	
}

void ILuaObject::UnReference()
{
	m_pState->ReferenceFree( m_iRef );
	delete this;
}

int ILuaObject::GetReference()
{
	return m_iRef;
}

int ILuaObject::GetType()
{
	m_pState->ReferencePush( m_iRef );
	return m_pState->GetType( -1 );
}

const char* ILuaObject::GetTypeName()
{
	m_pState->ReferencePush( m_iRef );
	return m_pState->GetTypeName( -1 );
}

void ILuaObject::SetMember( const char* name, ILuaObject* obj )
{
	m_pState->ReferencePush( m_iRef );
	m_pState->PushString( name );
	m_pState->ReferencePush( obj->GetReference() );
	m_pState->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, double d )
{
	m_pState->ReferencePush( m_iRef );
	m_pState->PushString( name );
	m_pState->PushNumber( d );
	m_pState->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, bool b )
{
	m_pState->ReferencePush( m_iRef );
	m_pState->PushString( name );
	m_pState->PushBool( b );
	m_pState->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, const char* s )
{
	m_pState->ReferencePush( m_iRef );
	m_pState->PushString( name );
	m_pState->PushString( s );
	m_pState->SetTable( -3 );
}

void ILuaObject::SetMember( const char* name, CFunc f )
{
	m_pState->ReferencePush( m_iRef );
	m_pState->PushString( name );
	m_pState->PushCFunction( f );
	m_pState->SetTable( -3 );
}


void ILuaObject::Push()
{
	m_pState->ReferencePush( m_iRef );
}

