#include "gm_tmysql.h"

Database::Database( const char* host, const char* user, const char* pass, const char* db, int port, const char* unix ) :
	m_strHost(host), m_strUser(user), m_strPass(pass), m_strDB(db), m_iPort(port), m_strUnix(unix), m_pThreadPool(NULL)
{
}

Database::~Database( void )
{

}

bool Database::Initialize( CUtlString& error )
{
	static my_bool bTrue = true;

	for( int i = NUM_CON_DEFAULT; i; i-- )
	{
		MYSQL* mysql = mysql_init(NULL);

		if ( !Connect( mysql, error ) )
		{
			return false;	
		}

		m_vecAvailableConnections.AddToTail( mysql );
		m_vecAllConnections.AddToTail( mysql );
	}

	m_pThreadPool = CreateThreadPool();

	ThreadPoolStartParams_t params;
	params.nThreads = NUM_THREADS_DEFAULT;

	if ( !m_pThreadPool->Start( params ) )
	{
		SafeRelease( m_pThreadPool );
		error.Set( "Unable to start thread pool" );

		return false;
	}

	return true;
}

bool Database::Connect( MYSQL* mysql, CUtlString& error )
{
	if ( !mysql_real_connect( mysql, m_strHost, m_strUser, m_strPass, m_strDB, m_iPort, m_strUnix, 0 ) )
	{
		error.Set( mysql_error( mysql ) );
		return false;
	}

	return true;
}

bool Database::IsSafeToShutdown( void )
{
	bool completedDispatch;
	{
		AUTO_LOCK_FM( m_vecCompleted );
		completedDispatch = m_vecCompleted.Count() == 0;
	}

	return completedDispatch && m_pThreadPool && m_pThreadPool->GetJobCount() == 0 && m_pThreadPool->NumIdleThreads() == m_pThreadPool->NumThreads();
}

/*
void Database::PreparedQuery(MYSQL* mysql, char *query)
{
	static int NO_FIELDS = 10;
	MYSQL_STMT *stmt;

	MYSQL_BIND binds[10];

	if (!(stmt = mysql_stmt_init(mysql)))
	{
		Error("mysql_stmt_init(), Error: out of memory");
		return;
	}
	if (mysql_stmt_prepare(stmt, query, strlen(query)))
	{
		Error("mysql_stmt_prepare(), Error: %s", mysql_stmt_error(stmt));
		return;
	}
	if (mysql_stmt_param_count(stmt) != NO_FIELDS)
	{
		Error("mysql_stmt_param_count(), Error: invalid parameter count");
		return;
	}
 
        memset(binds, 0, sizeof(MYSQL_BIND) * NO_FIELDS);

	// timestamp.
	// This is a number type, so there is no need to specify buffer_length.
	binds[0].buffer_type = MYSQL_TYPE_LONG;
	binds[0].buffer=  (char *)&timestamp;
	binds[0].is_null = 0;
	binds[0].length = 0;

	// clientaddress
	binds[1].buffer_type = MYSQL_TYPE_STRING;
	binds[1].buffer = (char *)clientaddress;
	binds[1].buffer_length = STRING_SIZE;
	binds[1].is_null = 0;
	binds[1].length = &clientaddresslength;

	// protocolname
	binds[2].buffer_type = MYSQL_TYPE_STRING;
	binds[2].buffer = (char *)protocolname;
	binds[2].buffer_length = STRING_SIZE;
	binds[2].is_null = 0;
	binds[2].length = &protocolnamelength;

	// outgoing
	binds[3].buffer_type = MYSQL_TYPE_LONG;
	binds[3].buffer = (char *)&outgoing;
	binds[3].is_null = 0;
	binds[3].length = 0;

	// type
	binds[4].buffer_type = MYSQL_TYPE_LONG;
	binds[4].buffer = (char *)&type;
	binds[4].is_null = 0;
	binds[4].length = 0;

	// localid
	binds[5].buffer_type = MYSQL_TYPE_STRING;
	binds[5].buffer = (char *)localid;
	binds[5].buffer_length = STRING_SIZE;
	binds[5].is_null = 0;
	binds[5].length = &localidlength;

	// remoteid
	binds[6].buffer_type = MYSQL_TYPE_STRING;
	binds[6].buffer = (char *)remoteid;
	binds[6].buffer_length = STRING_SIZE;
	binds[6].is_null = 0;
	binds[6].length = &remoteidlength;

	// filtered
	binds[7].buffer_type = MYSQL_TYPE_LONG;
	binds[7].buffer = (char *)&filtered;
	binds[7].is_null = 0;
	binds[7].length = 0;

	// categories
	binds[8].buffer_type = MYSQL_TYPE_STRING;
	binds[8].buffer = (char *)categories;
	binds[8].buffer_length = STRING_SIZE;
	binds[8].is_null = 0;
	binds[8].length = &categorieslength;

	// eventdata blob
	binds[9].buffer_type = MYSQL_TYPE_BLOB;
	binds[9].buffer = (char *)eventdata;
	binds[9].buffer_length = BUFFER_SIZE;
	binds[9].is_null = 0;
	binds[9].length = &eventdatalength;

	// Bind the buffers
	if (mysql_stmt_bind_param(stmt, binds))
	{
		Error("mysql_stmt_bind_param(), Error: %s", mysql_stmt_error(stmt));
		return;
	}
		
}*/

void Database::Shutdown( void )
{
	if ( m_pThreadPool != NULL )
	{
		m_pThreadPool->Stop();

		DestroyThreadPool( m_pThreadPool );
		m_pThreadPool = NULL;
	}

	FOR_EACH_VEC( m_vecAllConnections, i )
	{
		mysql_close( m_vecAllConnections[i] );
	}

	m_vecAllConnections.Purge();
	m_vecAvailableConnections.Purge();
	m_vecCompleted.Purge();
}

bool Database::SetCharacterSet( const char* charset, CUtlString& error )
{
	FOR_EACH_VEC( m_vecAllConnections, i )
	{
		if ( mysql_set_character_set( m_vecAllConnections[i], charset ) > 0 )
		{
			error.Set( mysql_error( m_vecAllConnections[i] ) );
			return false;
		}
	}

	return true;
}

void Database::QueueQuery( const char* query, int callback, int flags, int callbackref )
{
	Query* newquery = new Query( query, callback, flags, callbackref );

	QueueQuery( newquery );
}

void Database::QueueQuery( Query* query )
{
	CJob* job = m_pThreadPool->QueueCall( this, &Database::DoExecute, query );

	SafeRelease( job );
}

void Database::YieldPostCompleted( Query* query )
{
	AUTO_LOCK_FM( m_vecCompleted );

	m_vecCompleted.AddToTail( query );
}


void Database::DoExecute( Query* query )
{
	MYSQL* pMYSQL;
	
	{
		AUTO_LOCK_FM( m_vecAvailableConnections );
		Assert( m_vecAvailableConnections.Size() > 0 );

		pMYSQL = m_vecAvailableConnections.Head();
		m_vecAvailableConnections.Remove( 0 );

		Assert( pMYSQL );
	}


	const char* strquery = query->GetQuery();
	size_t len = query->GetQueryLength();

	int err = mysql_real_query( pMYSQL, strquery, len );

	if ( err > 0 )
	{
		int ping = mysql_ping( pMYSQL );
		if ( ping > 0 )
		{
			mysql_close( pMYSQL );
			mysql_init( pMYSQL );
			if(mysql_real_connect( pMYSQL, m_strHost, m_strUser, m_strPass, m_strDB, m_iPort, NULL, 0 ))
			{
				err = mysql_real_query( pMYSQL, strquery, len );
			} else {
				err = 1;
			}
		}
	}

	if ( err > 0 )
	{
		query->SetError( mysql_error( pMYSQL ) );
		query->SetStatus( QUERY_FAIL );
		query->SetResult( NULL );
	}
	else
	{
		query->SetStatus( QUERY_SUCCESS );
		query->SetResult( mysql_store_result( pMYSQL ) );

		if ( query->GetFlags() & QUERY_FLAG_LASTID )
		{
			query->SetLastID( mysql_insert_id( pMYSQL ) );
		}
	}

	YieldPostCompleted( query );

	{
		AUTO_LOCK_FM( m_vecAvailableConnections );
		m_vecAvailableConnections.AddToTail( pMYSQL );
	}
}