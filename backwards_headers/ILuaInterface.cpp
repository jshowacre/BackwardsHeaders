#include "ILuaInterface.h"

ILuaInterface::ILuaInterface( ILuaBase* state ) : m_pState(state)
{
}

ILuaBase* ILuaInterface::GetLua()
{
	return m_pState;
}

ILuaObject* ILuaInterface::Global()
{
	m_pState->PushSpecial( SPECIAL_GLOB );
	return new ILuaObject( m_pState, m_pState->ReferenceCreate() );
}

ILuaObject* ILuaInterface::GetNewTable()
{
	m_pState->CreateTable();
	return new ILuaObject(m_pState, m_pState->ReferenceCreate());
}

void ILuaInterface::Error( const char* strError )
{
	m_pState->ThrowError( strError );
}

void ILuaInterface::SetGlobal( const char* name, CFunc f )
{
	m_pState->PushSpecial( SPECIAL_GLOB );
	m_pState->PushString( name );
	m_pState->PushCFunction( f );
	m_pState->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, double d )
{
	m_pState->PushSpecial( SPECIAL_GLOB );
	m_pState->PushString( name );
	m_pState->PushNumber( d );
	m_pState->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, const char* str )
{
	m_pState->PushSpecial( SPECIAL_GLOB );
	m_pState->PushString( name );
	m_pState->PushString( str );
	m_pState->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, bool b )
{
	m_pState->PushSpecial( SPECIAL_GLOB );
	m_pState->PushString( name );
	m_pState->PushBool( b );
	m_pState->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, void* u )
{
	m_pState->PushSpecial( SPECIAL_GLOB );
	m_pState->PushString( name );
	m_pState->PushUserdata( u );
	m_pState->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, ILuaObject* o )
{
	m_pState->PushSpecial( SPECIAL_GLOB );
	m_pState->PushString( name );
	m_pState->ReferencePush( o->GetReference() );
	m_pState->SetTable( -3 );
}

ILuaObject* ILuaInterface::GetObject( int i )
{
	m_pState->ReferencePush( i );
	return new ILuaObject(m_pState, m_pState->ReferenceCreate());
}

const char* ILuaInterface::GetString( int i )
{
	return m_pState->GetString( i );
}

int ILuaInterface::GetInteger( int i )
{
	return m_pState->GetNumber( i );
}

double ILuaInterface::GetNumber( int i )
{
	return m_pState->GetNumber( i );
}

bool ILuaInterface::GetBool( int i )
{
	return (int) m_pState->GetBool( i );
}

void* ILuaInterface::GetUserData( int i )
{
	return m_pState->GetUserdata( i );
}

int ILuaInterface::GetReference( int i )
{
	m_pState->ReferencePush( i );
	return m_pState->ReferenceCreate();
}

void ILuaInterface::FreeReference( int i )
{
	m_pState->ReferenceFree( i );
}

void ILuaInterface::PushReference( int i )
{
	m_pState->ReferencePush( i );
}


void ILuaInterface::Pop( int i )
{
	m_pState->Pop( i );
}

int ILuaInterface::Top()
{
	return m_pState->Top();
}

void ILuaInterface::Push( ILuaObject* o )
{
	m_pState->ReferencePush( o->GetReference() );
}

void ILuaInterface::Push( const char* str )
{
	m_pState->PushString( str );
}

void ILuaInterface::Push( double d )
{
	m_pState->PushNumber( d );
}

void ILuaInterface::Push( bool b )
{
	m_pState->PushBool( b );
}

void ILuaInterface::Push( CFunc f )
{
	m_pState->PushCFunction( f );
}

void ILuaInterface::PushNil()
{
	m_pState->PushNil();
}

void ILuaInterface::PushUserData( ILuaObject* metatable, void * v )
{

}

void ILuaInterface::CheckType( int i, int iType )
{
	if (m_pState->GetType(i) != iType)
	{
		char buff[512];
		sprintf_s(buff, "bad argument #%i (%s expected, got %s)", i, Type::Name[ iType ], m_pState->GetTypeName( i ) );
	}
}

ILuaObject* ILuaInterface::GetMetaTable( const char* strName, int iType )
{
	m_pState->CreateMetaTableType( strName, iType );
	return new ILuaObject(m_pState, m_pState->ReferenceCreate());
}

/*
ILuaObject* ILuaInterface::GetMetaTable( int i );
{
	// Not possible yet..
}
*/