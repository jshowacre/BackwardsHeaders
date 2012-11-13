#include "gm_tmysql.h"

#define DATABASE_NAME "Database"
#define DATABASE_ID 200

GMOD_MODULE( open_module, close_module );

ILuaObject* g_tConnections;
ILuaObject* g_DBMeta;
LUA_FUNCTION( escape );
LUA_FUNCTION( dbescape );
LUA_FUNCTION( initialize );
LUA_FUNCTION( disconnect );
LUA_FUNCTION( __tostring );
LUA_FUNCTION( setcharset );
LUA_FUNCTION( query );
LUA_FUNCTION( poll );
LUA_FUNCTION( pollall );
LUA_FUNCTION( gettable );

void DisconnectDB( ILuaInterface* gLua,  Database* mysqldb );
void DispatchCompletedQueries( ILuaInterface* gLua, Database* mysqldb, bool requireSync );
void HandleQueryCallback( ILuaInterface* gLua, Query* query );
bool PopulateTableFromQuery( ILuaInterface* gLua, ILuaObject* table, Query* query );

int open_module( lua_State* L )
{	
	ILuaInterface* gLua = Lua();

	g_tConnections = gLua->GetNewTable();
	g_DBMeta = gLua->GetMetaTable( DATABASE_NAME, DATABASE_ID );

	mysql_library_init( 0, NULL, NULL );

	gLua->SetGlobal( "QUERY_SUCCESS", QUERY_SUCCESS );

	gLua->SetGlobal( "QUERY_SUCCESS", QUERY_SUCCESS );
	gLua->SetGlobal( "QUERY_FAIL", QUERY_FAIL );

	gLua->SetGlobal( "QUERY_FLAG_ASSOC", (float)QUERY_FLAG_ASSOC );
	gLua->SetGlobal( "QUERY_FLAG_LASTID", (float)QUERY_FLAG_LASTID );

	gLua->SetGlobal( "MYSQL_VERSION", (float)mysql_get_client_version() );
    gLua->SetGlobal( "MYSQL_INFO", mysql_get_client_info() );

	ILuaObject* tmysql = gLua->GetNewTable();

		tmysql->SetMember("initialize", initialize);
		tmysql->SetMember("Connect", initialize);
		tmysql->SetMember("escape", escape);
		tmysql->SetMember("GetTable", gettable);
		tmysql->SetMember("PollAll", pollall);

		gLua->SetGlobal( "tmysql", tmysql );

	tmysql->UnReference();

	g_DBMeta->SetMember( "Query", query );
	g_DBMeta->SetMember( "Disconnect", disconnect );
	g_DBMeta->SetMember( "SetCharset", setcharset );
	g_DBMeta->SetMember( "Poll", poll );
	g_DBMeta->SetMember( "__gc", disconnect ); // This seems to be called on shutdown
	g_DBMeta->SetMember( "__tostring", __tostring );
	g_DBMeta->SetMember( "__index", g_DBMeta );
	
	// hook.Add("Think", "TMysqlPoll", tmysql.poll)
	ILuaObject *hook = gLua->GetGlobal("hook");
		ILuaObject *Add = hook->GetMember("Add");
			Add->Push();
				gLua->Push("Tick");
				gLua->Push("TMysqlPoll");
				gLua->Push(pollall);
			gLua->Call(3);
		hook->UnReference();
	Add->UnReference();

	return 0;
}

int close_module( lua_State* L )
{
	mysql_library_end();

	if ( g_tConnections )
		g_tConnections->UnReference();

	if ( g_DBMeta )
		g_DBMeta->UnReference();
	
	return 0;
}

LUA_FUNCTION( initialize )
{
	ILuaInterface* gLua = Lua();

	gLua->CheckType(1, Type::STRING);
	gLua->CheckType(2, Type::STRING);
	gLua->CheckType(3, Type::STRING);
	gLua->CheckType(4, Type::STRING);
	gLua->CheckType(5, Type::NUMBER);

	const char* host = gLua->GetString(1);
	const char* user = gLua->GetString(2);
	const char* pass = gLua->GetString(3);
	const char* db = gLua->GetString(4);
	int port = gLua->GetInteger(5);
	const char* unixSock = gLua->GetString(6); // Optional unix socket path for linux servers

	if(port == 0)
		port = 3306;

	ILuaObject* prevDB = g_tConnections->GetMember( db );

	if (prevDB->GetType() != Type::NIL )
	{
		gLua->Push( false );
		gLua->PushVA( "Connection for DB: %s already exists!", db );
		prevDB->UnReference();
		return 2;
	}

	prevDB->UnReference();

	Database* mysqldb = new Database( host, user, pass, db, port, unixSock );

	std::string error;

	if ( !mysqldb->Initialize( error ) )
	{
		gLua->Push( false );
		gLua->Push( error.c_str() );

		delete mysqldb;
		return 2;
	}

	ILuaObject* luaDB = gLua->NewUserData( g_DBMeta );
		luaDB->SetUserData( mysqldb, DATABASE_ID );
		g_tConnections->SetMember( db, luaDB );
		gLua->Push( luaDB );
	luaDB->UnReference();

	return 1;
}

LUA_FUNCTION( escape )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::STRING);

	const char* query = gLua->GetString( 1 );

	size_t len = strlen( query );
	char* escaped = new char[len*2+1];

	mysql_escape_string( escaped, query, len );	

	gLua->Push( escaped );

	delete escaped;
	return 1;
}

LUA_FUNCTION( dbescape )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, DATABASE_ID );
	gLua->CheckType( 2, Type::STRING );

	Database *mysqldb = ( Database* ) gLua->GetUserData( 1 );

	if ( !mysqldb )
		return 0;

	gLua->CheckType( 2, Type::STRING );

	const char* query = gLua->GetString( 2 );

	char* escaped = mysqldb->Escape( query );
	gLua->Push( escaped );

	delete escaped;
	return 1;
}

LUA_FUNCTION( disconnect )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, DATABASE_ID );

	Database *mysqldb = ( Database* ) gLua->GetUserData(1);

	if ( mysqldb )
		DisconnectDB( gLua, mysqldb );

	return 0;
}

LUA_FUNCTION( __tostring )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, DATABASE_ID );

	Database *mysqldb = (Database*) gLua->GetUserData(1);

	if ( !mysqldb )
		return 0;
	
	gLua->PushVA( "%s: %p", DATABASE_NAME, mysqldb );
	return 1;
}

LUA_FUNCTION( setcharset )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, DATABASE_ID );

	Database *mysqldb = ( Database* ) gLua->GetUserData(1);

	if ( !mysqldb )
		return 0;

	gLua->CheckType( 2, Type::STRING );

	const char* set = gLua->GetString( 2 );

	std::string error;
	mysqldb->SetCharacterSet( set, error );

	return 0;
}

LUA_FUNCTION( query )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, DATABASE_ID );

	Database *mysqldb = ( Database* ) gLua->GetUserData(1);

	if ( !mysqldb )
		return 0;

	gLua->CheckType( 2, Type::STRING );
	const char* query = gLua->GetString( 2 );

	int callbackfunc = -1;
	if(gLua->GetType(3) == Type::FUNCTION)
	{
		callbackfunc = gLua->GetReference(3);
	}

	int flags = gLua->GetInteger(4);

	int callbackref = -1;
	int callbackobj = gLua->GetType(5);
	if(callbackobj != Type::NIL)
	{
		callbackref = gLua->GetReference(5);
	}

	mysqldb->QueueQuery( query, callbackfunc, flags, callbackref );
	return 0;
}

LUA_FUNCTION( poll )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, DATABASE_ID );

	Database *mysqldb = ( Database* ) gLua->GetUserData(1);

	if ( !mysqldb )
		return 0;

	DispatchCompletedQueries( gLua, mysqldb, false );
	return 0;
}

LUA_FUNCTION( pollall )
{
	ILuaInterface* gLua = Lua();

	CUtlLuaVector* luaDBs = g_tConnections->GetMembers();

	FOR_LOOP( luaDBs, i )
	{
		LuaKeyValue& keyValues = luaDBs->at(i);

		Database* mysqldb = (Database*) keyValues.pValue->GetUserData();

		if ( mysqldb )
			DispatchCompletedQueries( gLua, mysqldb, false );
	}

	gLua->DeleteLuaVector( luaDBs );
	return 0;
}

LUA_FUNCTION( gettable )
{
	ILuaInterface* gLua = Lua();
	ILuaObject* connections = gLua->GetNewTable();

	gLua->Push( g_tConnections );
	return 1;
}

void DisconnectDB( ILuaInterface* gLua,  Database* mysqldb )
{
	if ( mysqldb )
	{
		while ( !mysqldb->WaitForSafeShutdown() )
		{
			DispatchCompletedQueries( gLua, mysqldb, true );
		}

		ILuaObject* luaDB = g_tConnections->GetMember( mysqldb->GetDB() );
			luaDB->SetNil();
		luaDB->UnReference();

		g_tConnections->SetMember( mysqldb->GetDB() );

		mysqldb->Shutdown();
		delete mysqldb;
	}
}

void DispatchCompletedQueries( ILuaInterface* gLua, Database* mysqldb, bool requireSync )
{
	std::deque< Query* >& completed = mysqldb->CompletedQueries();
	recursive_mutex& mutex = mysqldb->CompletedMutex();

	// peek at the size, the query threads will only add to it, so we can do this and not end up locking it for nothing
	if ( !requireSync && completed.empty() )
		return;

	{
		recursive_mutex::scoped_lock lock( mutex );

		for( std::deque< Query* >::const_iterator it = completed.begin(); it != completed.end(); ++it )
		{
			Query* query = *it;

			if ( query != NULL )
			{
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
		}

		completed.clear();
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
		gLua->Push( query->GetError().c_str() );
	}

	if ( gLua->PCall( args ) != 0 )
	{
		const char* err = gLua->GetString(-1);
		gLua->ErrorNoHalt( err );
	}

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

	while ( row != NULL )
	{
		ILuaObject* resultrow = gLua->GetNewTable();

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