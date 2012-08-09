#include "ILuaObject.h"

ILuaObject::ILuaObject( ILuaBase* lua ) : m_pLua(lua), m_iRef(-1)
{
}

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

void ILuaObject::Set( ILuaObject* obj ) // ???
{
	m_iRef = obj->GetReference();
}

void ILuaObject::SetFromStack( int i ) // ???
{
	m_pLua->Push( i );
	m_iRef = m_pLua->ReferenceCreate();
	m_pLua->Pop( i );
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

const char* ILuaObject::GetString( void )
{
	Push();
	const char* ret = m_pLua->GetString( -1 );
	m_pLua->Pop();
	return ret;
}

int ILuaObject::GetInt( void )
{
	Push();
	int ret = (int) m_pLua->GetNumber( -1 );
	m_pLua->Pop();
	return ret;
}

double ILuaObject::GetDouble( void )
{
	Push();
	double ret = m_pLua->GetNumber( -1 );
	m_pLua->Pop();
	return ret;
}

float ILuaObject::GetFloat( void )
{
	Push();
	float ret = (float) m_pLua->GetNumber( -1 );
	m_pLua->Pop();
	return ret;
}

bool ILuaObject::GetBool( void )
{
	Push();
	bool ret = m_pLua->GetBool( -1 );
	m_pLua->Pop();
	return ret;
}

void* ILuaObject::GetUserData( void )
{
	Push();
	void* ret = m_pLua->GetUserdata( -1 );
	m_pLua->Pop();
	return ret;
}

void ILuaObject::SetMember( const char* name )
{
	Push();
	m_pLua->PushString( name );
	m_pLua->PushNil();
	m_pLua->SetTable( -3 );
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

void ILuaObject::SetMember( double fKey )
{
	Push();
	m_pLua->PushNumber( fKey );
	m_pLua->PushNil();
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( double fKey, ILuaObject* obj )
{
	Push();
	m_pLua->PushNumber( fKey );
	obj->Push();
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( double fKey, double d )
{
	Push();
	m_pLua->PushNumber( fKey );
	m_pLua->PushNumber( d );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( double fKey, bool b )
{
	Push();
	m_pLua->PushNumber( fKey );
	m_pLua->PushBool( b );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( double fKey, const char* s )
{
	Push();
	m_pLua->PushNumber( fKey );
	m_pLua->PushString( s );
	m_pLua->SetTable( -3 );
}

void ILuaObject::SetMember( double fKey, CFunc f )
{
	Push();
	m_pLua->PushNumber( fKey );
	m_pLua->PushCFunction( f );
	m_pLua->SetTable( -3 );
}

ILuaObject* ILuaObject::GetMember( const char* name )
{
	Push();
	m_pLua->GetField( -1, name );
	return new ILuaObject( m_pLua, m_pLua->ReferenceCreate() );
}

void ILuaObject::SetUserData( void* obj )
{
	Push();
	m_pLua->NewUserdata( sizeof( obj ) );
	m_pLua->PushUserdata( obj );
	m_pLua->GetUserdata( -3 );
	m_pLua->Pop();
}

bool ILuaObject::isType( int iType )
{
	Push();
	bool ret = m_pLua->GetType( -1 ) == iType;
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

