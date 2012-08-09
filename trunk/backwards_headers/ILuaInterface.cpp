#include "ILuaInterface.h"

ILuaInterface::ILuaInterface( lua_State* state ) : m_pState(state), m_pLua(state->luabase)
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pG = new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );

	m_pLua->PushSpecial( SPECIAL_REG );
	m_pR = new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );

	m_pLua->PushSpecial( SPECIAL_ENV );
	m_pE = new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
}

ILuaInterface::~ILuaInterface()
{
	m_pG->UnReference();
	m_pR->UnReference();
	m_pE->UnReference();
}

lua_State* ILuaInterface::GetLuaState()
{
	return m_pState;
}

ILuaObject* ILuaInterface::Global()
{
	return m_pG;
}

ILuaObject* ILuaInterface::Registry()
{
	return m_pR;
}

ILuaObject* ILuaInterface::Environment()
{
	return m_pE;
}

ILuaObject* ILuaInterface::GetNewTable()
{
	m_pLua->CreateTable();
	return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
}

void ILuaInterface::NewTable()
{
	m_pLua->CreateTable();
}

void ILuaInterface::Error( const char* strError )
{
	m_pLua->ThrowError( strError );
}

ILuaObject* ILuaInterface::GetGlobal( const char* name )
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pLua->GetField( -1, name );
	return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
}

void ILuaInterface::SetGlobal( const char* name, CFunc f )
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pLua->PushString( name );
	m_pLua->PushCFunction( f );
	m_pLua->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, double d )
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pLua->PushString( name );
	m_pLua->PushNumber( d );
	m_pLua->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, const char* str )
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pLua->PushString( name );
	m_pLua->PushString( str );
	m_pLua->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, bool b )
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pLua->PushString( name );
	m_pLua->PushBool( b );
	m_pLua->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, void* u )
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pLua->PushString( name );
	m_pLua->PushUserdata( u );
	m_pLua->SetTable( -3 );
}

void ILuaInterface::SetGlobal( const char* name, ILuaObject* o )
{
	m_pLua->PushSpecial( SPECIAL_GLOB );
	m_pLua->PushString( name );
	o->Push();
	m_pLua->SetTable( -3 );
}

ILuaObject* ILuaInterface::GetObject( int i )
{
	m_pLua->ReferencePush( i );
	return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
}

const char* ILuaInterface::GetString( int i )
{
	return m_pLua->GetString( i );
}

int ILuaInterface::GetInteger( int i )
{
	return (int) m_pLua->GetNumber( i );
}

double ILuaInterface::GetNumber( int i )
{
	return m_pLua->GetNumber( i );
}

double ILuaInterface::GetDouble( int i )
{
	return GetNumber( i );
}

float ILuaInterface::GetFloat( int i )
{
	return (float) m_pLua->GetNumber( i );
}

bool ILuaInterface::GetBool( int i )
{
	return m_pLua->GetBool( i );
}

void* ILuaInterface::GetUserData( int i )
{
	return m_pLua->GetUserdata( i );
}

int ILuaInterface::GetReference( int i )
{
	m_pLua->ReferencePush( i );
	return m_pLua->ReferenceCreate();
}

void ILuaInterface::FreeReference( int i )
{
	m_pLua->ReferenceFree( i );
}

void ILuaInterface::PushReference( int i )
{
	m_pLua->ReferencePush( i );
}


void ILuaInterface::Pop( int i )
{
	m_pLua->Pop( i );
}

int ILuaInterface::Top()
{
	return m_pLua->Top();
}

void ILuaInterface::Push( ILuaObject* o )
{
	o->Push();
}

void ILuaInterface::Push( const char* str )
{
	m_pLua->PushString( str );
}

void ILuaInterface::PushVA(const char* str, ...)
{
	char buff[ 1024 ];
	va_list argptr;
	va_start( argptr, str );
	vsprintf_s( buff, str, argptr );
	va_end( argptr );
	m_pLua->PushString( buff );
}

void ILuaInterface::Push( double d )
{
	m_pLua->PushNumber( d );
}

void ILuaInterface::Push( bool b )
{
	m_pLua->PushBool( b );
}

void ILuaInterface::Push( CFunc f )
{
	m_pLua->PushCFunction( f );
}

void ILuaInterface::PushNil()
{
	m_pLua->PushNil();
}

void ILuaInterface::PushUserData( ILuaObject* metatable, void * v )
{
	m_pLua->NewUserdata( sizeof( v ) );
		metatable->Push();
		m_pLua->GetMetaTable( -1 );
	m_pLua->SetMetaTable( -2 );
}

void ILuaInterface::CheckType( int i, int iType )
{
	m_pLua->CheckType( i, iType );
}

int ILuaInterface::GetType( int iStackPos )
{
	return m_pLua->GetType( iStackPos );
}

const char* ILuaInterface::GetTypeName( int iType )
{
	return m_pLua->GetTypeName( iType );
}

ILuaObject* ILuaInterface::GetReturn( int iNum )
{
	return new ILuaObject(m_pLua, m_pLua->ReferenceCreate());
}

void ILuaInterface::Call( int args, int returns )
{
	m_pLua->Call( args, returns );
}

ILuaObject* ILuaInterface::GetMetaTable( const char* strName, int iType )
{
	m_pLua->CreateMetaTableType( strName, iType );
	return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
}

ILuaObject* ILuaInterface::GetMetaTable( int i )
{
	if( m_pLua->GetMetaTable( i ) )
		return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
	else
		return NULL;
}