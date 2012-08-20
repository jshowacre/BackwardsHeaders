#include "ILuaInterface.h"
#include "ILuaUserData.h"

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
	NewTable();
	return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
}

void ILuaInterface::NewTable()
{
	m_pLua->CreateTable();
}

ILuaObject* ILuaInterface::NewTemporaryObject()
{
	return new ILuaObject( m_pLua );
}

ILuaObject* ILuaInterface::NewUserData( ILuaObject* metaT )
{
	ILuaUserData *data = (ILuaUserData*) m_pLua->NewUserdata( sizeof( ILuaUserData ) );
	ILuaObject* obj = new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );

	obj->Push(); // +1
		metaT->Push(); // +1
		m_pLua->SetMetaTable( -2 ); // -1
	m_pLua->Pop(); // -1

	return obj;
}

void ILuaInterface::PushUserData( ILuaObject* metaT, void * v )
{
	if (!metaT)
		Error("CLuaInterface - No Metatable!\n");

	ILuaUserData *data = (ILuaUserData*) m_pLua->NewUserdata( sizeof( ILuaUserData ) );
	data->obj = v;

	ILuaObject* obj = new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );

	obj->Push(); // +1
		metaT->Push(); // +1
		m_pLua->SetMetaTable( -2 ); // -1
}

void ILuaInterface::Error( const char* strError )
{
	m_pLua->ThrowError( strError );
}

ILuaObject* ILuaInterface::GetGlobal( const char* name )
{
	m_pLua->PushSpecial( SPECIAL_GLOB ); // +1
		m_pLua->GetField( -1, name ); // +1
		ILuaObject* o = new ILuaObject( m_pLua, m_pLua->ReferenceCreate() ); // -1
	m_pLua->Pop();
	return o;
}

void ILuaInterface::SetGlobal( const char* name, CFunc f )
{
	m_pLua->PushSpecial( SPECIAL_GLOB ); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushCFunction( f ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaInterface::SetGlobal( const char* name, double d )
{
	m_pLua->PushSpecial( SPECIAL_GLOB ); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushNumber( d ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaInterface::SetGlobal( const char* name, const char* str )
{
	m_pLua->PushSpecial( SPECIAL_GLOB ); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushString( str ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaInterface::SetGlobal( const char* name, bool b )
{
	m_pLua->PushSpecial( SPECIAL_GLOB ); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushBool( b ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaInterface::SetGlobal( const char* name, void* u )
{
	m_pLua->PushSpecial( SPECIAL_GLOB ); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushUserdata( u ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaInterface::SetGlobal( const char* name, ILuaObject* o )
{
	m_pLua->PushSpecial( SPECIAL_GLOB ); // +1
		m_pLua->PushString( name ); // +1
		o->Push(); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

ILuaObject* ILuaInterface::GetObject( int i )
{
	if(i != 0)
		m_pLua->Push( i ); // +??
	return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() ); // -1
}

const char* ILuaInterface::GetString( int i )
{
	return m_pLua->GetString( i );
}

int ILuaInterface::GetInteger( int i )
{
	return (int) GetNumber( i );
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
	return (float) GetNumber( i );
}

bool ILuaInterface::GetBool( int i )
{
	return m_pLua->GetBool( i );
}

void* ILuaInterface::GetUserData( int i )
{
	ILuaUserData* data = (ILuaUserData*) m_pLua->GetUserdata( i );
	return data->obj;
}

int ILuaInterface::GetReference( int i )
{
	if(i != 0)
		m_pLua->Push( i ); // +??
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
	return GetObject( iNum );
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