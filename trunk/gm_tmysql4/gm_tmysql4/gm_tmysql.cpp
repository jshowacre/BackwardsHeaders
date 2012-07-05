#pragma comment (lib, "tier0.lib")
#pragma comment (lib, "tier1.lib")
#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#include "gm_tmysql.h"

#define GMOD_BETA

#define DATABASE_NAME "tmysqlDB"
#define DATABASE_ID 6603

GMOD_MODULE(Start, Close);

CUtlVectorMT<CUtlVector<Database*>> m_vecConnections;
LUA_FUNCTION( escape );
LUA_FUNCTION( initialize );
LUA_FUNCTION( disconnect );
LUA_FUNCTION( __tostring );
LUA_FUNCTION( setcharset );
LUA_FUNCTION( query );
LUA_FUNCTION( poll );
LUA_FUNCTION( pollall );
LUA_FUNCTION( gettable );

void DispatchCompletedQueries( ILuaInterface* gLua, Database* mysqldb, bool requireSync );
bool PopulateTableFromQuery( ILuaInterface* gLua, ILuaObject* table, Query* query );
void HandleQueryCallback( ILuaInterface* gLua, Query* query );
void DisconnectDB( ILuaInterface* gLua, Database* mysqldb );

int Start(lua_State *L)
{
	mysql_library_init( 0, NULL, NULL );

	ILuaInterface *gLua = Lua();

	gLua->SetGlobal( "QUERY_SUCCESS", QUERY_SUCCESS );
	gLua->SetGlobal( "QUERY_FAIL", QUERY_FAIL );

	gLua->SetGlobal( "QUERY_FLAG_ASSOC", (float)QUERY_FLAG_ASSOC );
	gLua->SetGlobal( "QUERY_FLAG_LASTID", (float)QUERY_FLAG_LASTID );

	ILuaObject* mfunc = gLua->GetNewTable();
		mfunc->SetMember("initialize", initialize);
		mfunc->SetMember("Connect", initialize);
		mfunc->SetMember("escape", escape );
		mfunc->SetMember("GetTable", gettable);
	gLua->SetGlobal( "tmysql", mfunc );

	mfunc->UnReference();

	ILuaObject *metaT = Lua()->GetMetaTable( DATABASE_NAME, DATABASE_ID );
		metaT->SetMember( "Query", query );
		metaT->SetMember( "Disconnect", disconnect );
		metaT->SetMember( "SetCharset", setcharset );
		metaT->SetMember( "Poll", poll );
		metaT->SetMember( "__gc", disconnect ); // This seems to be called on shutdown
		metaT->SetMember( "__tostring", __tostring );
		metaT->SetMember( "__index", metaT );
	metaT->UnReference();

	// hook.Add("Think", "TMysqlPoll", tmysql.poll)
	ILuaObject *hookt = gLua->GetGlobal("hook");
	ILuaObject *addf = hookt->GetMember("Add");
	addf->Push();
	gLua->Push("Think");
	gLua->Push("TMysqlPoll");
	gLua->Push(pollall);
	gLua->Call(3);

	hookt->UnReference();
	addf->UnReference();

	return 0;
}

int Close(lua_State *L)
{
	mysql_library_end();
	return 0;
}

LUA_FUNCTION( escape )
{
	ILuaInterface* gLua = Lua();
	
	gLua->CheckType( 1, GLua::TYPE_STRING );

	const char* query = gLua->GetString( 1 );

	size_t len = Q_strlen( query );
	char* escaped = new char[len*2+1];

	mysql_escape_string( escaped, query, len );	

	gLua->Push( escaped );

	delete escaped;
	return 1;
}

LUA_FUNCTION( initialize )
{
	ILuaInterface* gLua = Lua();

	gLua->CheckType(1, GLua::TYPE_STRING);
	gLua->CheckType(2, GLua::TYPE_STRING);
	gLua->CheckType(3, GLua::TYPE_STRING);
	gLua->CheckType(4, GLua::TYPE_STRING);
	gLua->CheckType(5, GLua::TYPE_NUMBER);

	const char* host = gLua->GetString(1);
	const char* user = gLua->GetString(2);
	const char* pass = gLua->GetString(3);
	const char* db = gLua->GetString(4);
	int port = gLua->GetInteger(5);
	const char* unix = gLua->GetString(6);

	if(port == 0)
		port = 3306;

	Database* mysqldb = new Database( host, user, pass, db, port, unix );
	mysqldb->m_iIndex = m_vecConnections.AddToTail(mysqldb);

	CUtlString error;

	if ( !mysqldb->Initialize( error ) )
	{
		char buffer[1024];

		Q_snprintf( buffer, sizeof(buffer), "Error connecting to DB: %s", error.Get() );
		gLua->Msg( "%s\n", buffer );

		gLua->Push( false );
		gLua->Push( buffer );

		return 2;
	}
	
	ILuaObject *metaT = Lua()->GetMetaTable( DATABASE_NAME, DATABASE_ID );
		Lua()->PushUserData( metaT, mysqldb );
	metaT->UnReference();
	return 1;
}

LUA_FUNCTION( disconnect )
{
	Lua()->CheckType( 1, DATABASE_ID );
	Database *mysqldb = ( Database* ) Lua()->GetUserData(1);
	DisconnectDB( Lua(), mysqldb );
	return 0;
}

LUA_FUNCTION( __tostring )
{
	Lua()->CheckType( 1, DATABASE_ID );

	Database *mysqldb = ( Database* ) Lua()->GetUserData(1);

	if ( !mysqldb )
		return 0;
	
	Lua()->PushVA( "%s: %p", DATABASE_NAME, mysqldb );

	return 1;
}

LUA_FUNCTION( setcharset )
{
	ILuaInterface* gLua = Lua();
	Database *mysqldb = ( Database* ) Lua()->GetUserData(1);

	if ( !mysqldb )
		return 0;

	gLua->CheckType( 2, GLua::TYPE_STRING );

	const char* set = gLua->GetString( 2 );

	CUtlString error;
	mysqldb->SetCharacterSet( set, error );

	return 0;
}

LUA_FUNCTION( query )
{
	ILuaInterface* gLua = Lua();
	Database *mysqldb = ( Database* ) Lua()->GetUserData(1);

	if ( !mysqldb )
		return 0;

	gLua->CheckType( 2, GLua::TYPE_STRING );
	const char* query = gLua->GetString( 2 );

	int callbackfunc = -1;
	if(gLua->GetType(3) == GLua::TYPE_FUNCTION)
	{
		callbackfunc = gLua->GetReference(3);
	}

	int flags = gLua->GetInteger(4);

	int callbackref = -1;
	int callbackobj = gLua->GetType(5);
	if(callbackobj != GLua::TYPE_NIL)
	{
		callbackref = gLua->GetReference(5);
	}

	mysqldb->QueueQuery( query, callbackfunc, flags, callbackref );
	return 0;
}

LUA_FUNCTION( poll )
{
	ILuaInterface* gLua = Lua();
	Database *mysqldb = ( Database* ) Lua()->GetUserData(1);

	if ( !mysqldb )
		return 0;

	DispatchCompletedQueries( gLua, mysqldb, false );
	return 0;
}

LUA_FUNCTION( pollall )
{
	ILuaInterface* gLua = Lua();

	for(int i = 0; i < m_vecConnections.Count(); i++)
	{
		Database* mysqldb = m_vecConnections.Element(i);

		if ( mysqldb )
			DispatchCompletedQueries( gLua, mysqldb, false );
	}
	return 0;
}

LUA_FUNCTION( gettable )
{
	ILuaObject* connections = Lua()->GetNewTable();

	for(int i = 0; i < m_vecConnections.Count(); i++)
	{
		Database* mysqldb = m_vecConnections.Element(i);

		ILuaObject *metaT = Lua()->GetMetaTable( DATABASE_NAME, DATABASE_ID );
			ILuaObject* luaDB = Lua()->NewUserData( metaT );
				luaDB->SetUserData( mysqldb );
				connections->SetMember( i+1, luaDB );
			luaDB->UnReference();
		metaT->UnReference();
	}
	return 1;
}

void DisconnectDB( ILuaInterface* gLua, Database* mysqldb )
{
	if ( mysqldb )
	{
		while ( !mysqldb->IsSafeToShutdown() )
		{
			DispatchCompletedQueries( gLua, mysqldb, true );
			ThreadSleep( 10 );
		}

		m_vecConnections.Remove( mysqldb->m_iIndex );
		mysqldb->Shutdown();
		delete mysqldb;
	}
}

void DispatchCompletedQueries( ILuaInterface* gLua, Database* mysqldb, bool requireSync )
{
	CUtlVectorMT<CUtlVector<Query*> >& completed = mysqldb->CompletedQueries();

	// peek at the size, the query threads will only add to it, so we can do this and not end up locking it for nothing
	if( !requireSync && completed.Size() <= 0 )
		return;

	{
		AUTO_LOCK_FM( completed );

		FOR_EACH_VEC( completed, i )
		{
			Query* query = completed[i];

			if ( query->GetCallback() >= 0 )
			{
				HandleQueryCallback( gLua, query );
			}

			if ( query->GetResult() != NULL )
			{
				mysql_free_result( query->GetResult() );
			}

			delete query;
		}

		completed.RemoveAll();
	}
}

void HandleQueryCallback( ILuaInterface* gLua, Query* query )
{
	ILuaObject *resultTable = gLua->GetNewTable();

	if( !PopulateTableFromQuery( gLua, resultTable, query ) )
	{
		gLua->Error("Unable to populate result table");
	}

	gLua->PushReference( query->GetCallback() );
	gLua->FreeReference( query->GetCallback() );

	int args = 3;
	if( query->GetCallbackRef() >= 0)
	{
		args = 4;
		gLua->PushReference( query->GetCallbackRef() );
		gLua->FreeReference( query->GetCallbackRef() );
	}

	gLua->Push( resultTable );
	resultTable->UnReference();
	gLua->Push( query->GetStatus() );

	if ( query->GetStatus() )
	{
		gLua->Push( (float)query->GetLastID() );
	}
	else
	{
		gLua->Push( query->GetError() );
	}

	gLua->Call(args);
}

bool PopulateTableFromQuery( ILuaInterface* gLua, ILuaObject* table, Query* query )
{
	MYSQL_RES* result = query->GetResult();

	// no result to push, continue, this isn't fatal
	if ( result == NULL )
		return true;

	MYSQL_FIELD* fields;
	MYSQL_ROW row = mysql_fetch_row( result );
	int field_count = mysql_num_fields( result );

	if ( query->GetFlags() & QUERY_FLAG_ASSOC )
	{
		fields = mysql_fetch_fields( result );

		if ( fields == NULL )
			return false;
	}

	int rowid = 1;
	ILuaObject* resultrow = gLua->NewTemporaryObject();

	while ( row != NULL )
	{
		// black magic warning: we use a temp and assign it so that we avoid consuming all the temp objects and causing horrible disasters
		gLua->NewTable();
		resultrow->SetFromStack(-1);
		gLua->Pop();

		for ( int i = 0; i < field_count; i++ )
		{
			if ( query->GetFlags() & QUERY_FLAG_ASSOC )
			{
				resultrow->SetMember( fields[i].name, row[i] );
			} 
			else
			{
				resultrow->SetMember( (float)i+1, row[i] );
			}
		}

		table->SetMember( (float)rowid, resultrow );
		resultrow->UnReference();

		row = mysql_fetch_row( result );
		rowid++;
	}

	return true;
}

/*
int main(int argc, char** argv)
{
	Database* database = new Database("127.0.0.1", "root", "", "test", 3306);

	CUtlString error;
	if ( !database->Initialize( error ) )
	{
		printf("error: %s\n", error);
		return 0;
	}

	CUtlVectorMT<CUtlVector<Query*>>& completed = database->CompletedQueries();

	static int i = 0;
	while ( true )
	{
		{
			AUTO_LOCK_FM( completed );

			FOR_EACH_VEC( completed, i )
			{
				Query* query = completed[i];
				printf( "query: %s; callback %d; status %d time: %f\n", query->GetQuery(), query->GetCallback(), query->GetStatus(), query->GetQueryTime() );

				BuildTableFromQuery( NULL, query );
			}

			completed.RemoveAll();
		}
		Sleep( 1 );
	}
}
*/