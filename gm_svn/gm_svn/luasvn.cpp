#include <svn_repos.h>
#include <svn_pools.h>
#include <svn_error.h>
#include <svn_client.h>
#include <svn_dso.h>
#include <svn_path.h>
#include <svn_config.h>
#include <svn_cmdline.h>
#include <svn_subst.h>
#include <svn_time.h>

#define NO_SDK
#include "gmod/GMLuaModule.h"

// CVar
#include "tier1/iconvar.h"
#include "tier1/tier1.h"
#include "vstdlib/cvar.h"
#include "convar.h"

//extern ConVar svn_cache_passwords;

#define IF_ERROR_RETURN(err, pool, L) { \
	if (err) { \
	svn_string_t *sstring; \
	sstring = svn_string_create (err->message, pool); \
	svn_subst_detranslate_string (&sstring, sstring, TRUE, pool); \
	Lua()->Error(sstring->data); \
	svn_pool_destroy (pool); \
	} \
}

static void init_function ( lua_State *L, svn_client_ctx_t **ctx, apr_pool_t **pool, const char *username, const char *password) {
	apr_allocator_t *allocator;
	svn_auth_baton_t *ab;
	svn_config_t *cfg;
	svn_error_t *err;

	if (svn_cmdline_init("svn", NULL) != EXIT_SUCCESS) {
		Lua()->Error("Error initializing svn\n");
		return;
	}
	
	if (apr_allocator_create(&allocator)) {
		Lua()->Error("Error creating allocator\n");
		return;
	}

	apr_allocator_max_free_set(allocator, SVN_ALLOCATOR_RECOMMENDED_MAX_FREE);

  	*pool = svn_pool_create_ex(NULL, allocator);
	apr_allocator_owner_set(allocator, *pool);

  	err = svn_ra_initialize(*pool);
	IF_ERROR_RETURN (err, *pool, L);

	err = svn_client_create_context (ctx, *pool);
	IF_ERROR_RETURN (err, *pool, L);

	err = svn_config_get_config(&((*ctx)->config),	NULL, *pool);
	IF_ERROR_RETURN (err, *pool, L);

	cfg = (svn_config_t *)apr_hash_get((*ctx)->config, SVN_CONFIG_CATEGORY_CONFIG,
			APR_HASH_KEY_STRING);
	
	// Use the latest and greatest non-deprecated
	err = svn_cmdline_create_auth_baton( &ab,
			FALSE,
			username,
			password,
			NULL,
			TRUE, //!svn_cache_passwords.GetBool(),
			TRUE,
			cfg,
			(*ctx)->cancel_func,
			(*ctx)->cancel_baton,
			*pool );

	IF_ERROR_RETURN (err, *pool, L);

	(*ctx)->auth_baton = ab;
}

LUA_FUNCTION( svn_cleanup ) {
	apr_pool_t *pool;
	svn_error_t *err;
	svn_client_ctx_t *ctx;
	
	Lua()->CheckType(1, GLua::TYPE_STRING);
	const char *path = Lua()->GetString(1);

	init_function ( L, &ctx, &pool, NULL, NULL );
	
	path = svn_path_canonicalize (path, pool);

	err = svn_client_cleanup(path, ctx, pool);
	IF_ERROR_RETURN (err, pool, L);

	if (err)
		Lua()->Push( (bool) false );
	else
		Lua()->Push( (bool) true );
	
	svn_pool_destroy (pool);

	return 1;
}

LUA_FUNCTION( svn_update ) {
	apr_pool_t *pool;
	svn_error_t *err;
	svn_client_ctx_t *ctx;
	
	apr_array_header_t *array;
	svn_opt_revision_t revision;
	
	Lua()->CheckType(1, GLua::TYPE_STRING);

	const char *path = Lua()->GetString(1);

	svn_boolean_t recursive = TRUE;
	svn_boolean_t ignore_externals = FALSE;
	apr_array_header_t *result_revs = NULL;

	const char *username = Lua()->GetString(2);
	const char *password = Lua()->GetString(3);

	if (Lua()->GetType(4) != GLua::TYPE_NUMBER) {
		revision.kind = svn_opt_revision_head;
	} else {
		revision.kind = svn_opt_revision_number;
		revision.value.number = Lua()->GetInteger(4);
	}

	username = ( username != NULL ? username : NULL );
	password = ( password != NULL ? password : NULL );

	init_function ( L, &ctx, &pool, username, password );

	path = svn_path_canonicalize (path, pool);

	array = apr_array_make (pool, 1, sizeof (const char *));
	(*((const char **) apr_array_push (array))) = path;

	err = svn_client_update2 (&result_revs, array, &revision, recursive, ignore_externals, ctx, pool);
	IF_ERROR_RETURN (err, pool, L);	

	if (result_revs == NULL) {
		Lua()->PushNil();
	} else {
		int rev = (int) ((int **) (result_revs->elts))[0];
		Lua()->PushLong(rev);
	}

	svn_pool_destroy (pool);

	return 1;
}

LUA_FUNCTION( svn_checkout ) {
	apr_pool_t *pool;
	svn_error_t *err;
	svn_client_ctx_t *ctx;

	svn_opt_revision_t revision;
	svn_opt_revision_t peg_revision;
	svn_revnum_t rev;

	Lua()->CheckType(1, GLua::TYPE_STRING);
	Lua()->CheckType(2, GLua::TYPE_STRING);

	const char *path = Lua()->GetString(1);
	const char *dir = Lua()->GetString(2);
	
	svn_boolean_t recursive = TRUE;
	svn_boolean_t ignore_externals = FALSE;
	peg_revision.kind = svn_opt_revision_unspecified;

	if (Lua()->GetType(5) != GLua::TYPE_NUMBER) {
		revision.kind = svn_opt_revision_head;
	} else {
		revision.kind = svn_opt_revision_number;
		revision.value.number = Lua()->GetInteger(5);
	}
	
	const char *username = Lua()->GetString(3);
	const char *password = Lua()->GetString(4);

	username = ( username != NULL ? username : NULL );
	password = ( password != NULL ? password : NULL );

	init_function ( L, &ctx, &pool, username, password );

	path = svn_path_canonicalize (path, pool);
	dir = svn_path_canonicalize (dir, pool);
	
	err = svn_client_checkout2 (&rev, path, dir, &peg_revision, &revision, recursive, ignore_externals, ctx, pool);
	IF_ERROR_RETURN (err, pool, L);
	
	if (rev == NULL)
		Lua()->PushNil();
	else
		Lua()->PushLong(rev);

	svn_pool_destroy (pool);

	return 1;
}

LUA_FUNCTION( svn_commit ) {
	apr_pool_t *pool;
	svn_error_t *err;
	svn_client_ctx_t *ctx;

	apr_array_header_t *array;

	Lua()->CheckType(1, GLua::TYPE_STRING);
	Lua()->CheckType(2, GLua::TYPE_STRING);

	const char *path = Lua()->GetString(1);
	//const char *message = Lua()->GetString(2);
	
	const char *username = Lua()->GetString(2);
	const char *password = Lua()->GetString(3);

	path = ( path != NULL ? path : "" );
	//message = ( message != NULL ? message : "" );

	username = ( username != NULL ? username : NULL );
	password = ( password != NULL ? password : NULL );

	svn_boolean_t recursive = TRUE;
	svn_boolean_t keep_locks = FALSE;
	svn_commit_info_t *commit_info = NULL;

	init_function( L, &ctx, &pool, username, password );

	path = svn_path_canonicalize (path, pool);

	array = apr_array_make (pool, 1, sizeof (const char *));
	(*((const char **) apr_array_push (array))) = path;

	//make_log_msg_baton (&(ctx->log_msg_baton2), message, path, ctx->config, pool, L);
	//ctx->log_msg_func2 = log_msg_func2;

	err = svn_client_commit3(&commit_info, array, recursive, keep_locks, ctx, pool);
	IF_ERROR_RETURN( err, pool, L );	

	if (commit_info == NULL)
		Lua()->PushNil();
	else
		Lua()->PushLong(commit_info->revision);

	svn_pool_destroy (pool);

	return 1;
}