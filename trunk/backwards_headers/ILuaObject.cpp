#include "ILuaObject.h"
#include "ILuaUserData.h"

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
	if(m_iRef >= 0)
		m_pLua->ReferenceFree( m_iRef );
	
	m_iRef = obj->GetReference();
}

void ILuaObject::SetFromStack(int i) // THIS DOESN'T ToDo: Fix
{
	if(m_iRef >= 0)
		m_pLua->ReferenceFree( m_iRef );
		
	m_pLua->Push(i);
		m_iRef = m_pLua->ReferenceCreate();
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
	Push(); // +1
		int ret = m_pLua->GetType( -1 );
	m_pLua->Pop(); // -1
	return ret;
}

const char* ILuaObject::GetTypeName()
{
	Push(); // +1
		const char* ret = m_pLua->GetTypeName( m_pLua->GetType( -1 ) );
	m_pLua->Pop(); // -1
	return ret;
}

const char* ILuaObject::GetString( void )
{
	Push(); // +1
		const char* ret = m_pLua->GetString( -1 );
	m_pLua->Pop(); // -1
	return ret;
}

int ILuaObject::GetInt( void )
{
	Push(); // +1
		int ret = (int) m_pLua->GetNumber( -1 );
	m_pLua->Pop(); // -1
	return ret;
}

double ILuaObject::GetDouble( void )
{
	Push(); // +1
		double ret = m_pLua->GetNumber( -1 );
	m_pLua->Pop(); // -1
	return ret;
}

float ILuaObject::GetFloat( void )
{
	Push(); // +1
		float ret = (float) m_pLua->GetNumber( -1 );
	m_pLua->Pop(); // -1
	return ret;
}

bool ILuaObject::GetBool( void )
{
	Push(); // +1
		bool ret = m_pLua->GetBool( -1 );
	m_pLua->Pop(); // -1
	return ret;
}

void* ILuaObject::GetUserData( void )
{
	Push(); // +1
		ILuaUserData* data = (ILuaUserData*) m_pLua->GetUserdata( -1 );
	m_pLua->Pop(); // -1
	return data->obj;
}

void ILuaObject::SetMember( const char* name )
{
	Push(); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushNil(); // +1
		m_pLua->SetTable( -3 ); // +2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( const char* name, ILuaObject* obj )
{
	Push(); // +1
		m_pLua->PushString( name ); // +1
		obj->Push(); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( const char* name, double d )
{
	Push(); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushNumber( d ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( const char* name, bool b )
{
	Push(); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushBool( b ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( const char* name, const char* s )
{
	Push(); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushString( s ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( const char* name, CFunc f )
{
	Push(); // +1
		m_pLua->PushString( name ); // +1
		m_pLua->PushCFunction( f ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( double fKey )
{
	Push(); // +1
		m_pLua->PushNumber( fKey ); // +1
		m_pLua->PushNil(); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( double fKey, ILuaObject* obj )
{
	Push(); // +1
		m_pLua->PushNumber( fKey ); // +1
		obj->Push(); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( double fKey, double d )
{
	Push(); // +1
		m_pLua->PushNumber( fKey ); // +1
		m_pLua->PushNumber( d ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( double fKey, bool b )
{
	Push(); // +1
		m_pLua->PushNumber( fKey ); // +1
		m_pLua->PushBool( b ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( double fKey, const char* s )
{
	Push(); // +1
		m_pLua->PushNumber( fKey ); // +1
		m_pLua->PushString( s ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

void ILuaObject::SetMember( double fKey, CFunc f )
{
	Push(); // +1
		m_pLua->PushNumber( fKey ); // +1
		m_pLua->PushCFunction( f ); // +1
		m_pLua->SetTable( -3 ); // -2
	m_pLua->Pop(); // -1
}

ILuaObject* ILuaObject::GetMember( const char* name )
{
	Push(); // +1
		m_pLua->GetField( -1, name ); // +1
		ILuaObject* o = new ILuaObject( m_pLua, m_pLua->ReferenceCreate() ); // -1
	m_pLua->Pop(); // -1
	return o;
}

void ILuaObject::SetUserData( void* obj )
{
	Push(); // +1
		ILuaUserData *data = (ILuaUserData*) m_pLua->GetUserdata();
		data->obj = obj;
	m_pLua->Pop(); // -1
}

bool ILuaObject::isType( int iType )
{
	Push(); // +1
		bool ret = m_pLua->GetType( -1 ) == iType;
	m_pLua->Pop(); // -1
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

