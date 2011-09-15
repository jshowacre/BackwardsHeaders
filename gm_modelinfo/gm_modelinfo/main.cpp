//gm_modelinfo by Nevec Extended/Modified by blackops

//#pragma comment (lib, "psapi.lib")
#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#ifdef _LINUX
	#define ENGINE_LIB "engine.so"
#else
	#define ENGINE_LIB "engine.dll"
#endif

#undef _UNICODE

//#include <windows.h>
//#include <psapi.h>

//String stuff
#include <string>
#include <sstream>

//Lua module interface
#include "interface.h"
#include "gmod/GMLuaModule.h"
#include "engine/ivmodelinfo.h"
#include "studio.h"
#include "datacache/imdlcache.h"

IVModelInfo* g_IVModelInfo	= NULL;
//IMDLCache* g_IMDLCache = NULL;

GMOD_MODULE( Init, Shutdown );

inline const model_t* GetEntityModel( lua_State* L, int i )
{
	ILuaObject *ent = Lua()->GetObject( i );
	ILuaObject *entMeta = Lua()->GetMetaTable( "Entity", GLua::TYPE_ENTITY );
	ILuaObject *method = entMeta->GetMember( "GetModel" );
     
	Lua()->Push( method );
	Lua()->Push( ent );
	Lua()->Call( 1, 1 );
     
	ILuaObject *retrn = Lua()->GetReturn( 0 );

	const char* retrnStr = retrn->GetString();
     
	ent->UnReference();
	entMeta->UnReference();
	method->UnReference();
	retrn->UnReference();

	return g_IVModelInfo->FindOrLoadModel( retrnStr );
}

inline int GetEntityModelIndex( lua_State* L, int i )
{
	ILuaObject *ent = Lua()->GetObject( i );
	ILuaObject *entMeta = Lua()->GetMetaTable( "Entity", GLua::TYPE_ENTITY );
	ILuaObject *method = entMeta->GetMember( "GetModel" );
     
	Lua()->Push( method );
	Lua()->Push( ent );
	Lua()->Call( 1, 1 );
     
	ILuaObject *retrn = Lua()->GetReturn( 0 );

	const char* retrnStr = retrn->GetString();
     
	ent->UnReference();
	entMeta->UnReference();
	method->UnReference();
	retrn->UnReference();

	return g_IVModelInfo->GetModelIndex( retrnStr );
}

inline ILuaObject* NewVectorObject( lua_State* L, Vector& vec )
{
	ILuaObject* func = Lua()->GetGlobal( "Vector" );
	Lua()->Push( func );
	Lua()->Push( vec.x );
	Lua()->Push( vec.y );
	Lua()->Push( vec.z );
	if( !Lua()->Call( 3, 1 ) ) {
		func->UnReference();
		return NULL;
	}
	return Lua()->GetReturn( 0 );
}

inline void PushVector( lua_State* L, Vector& vec )
{
	ILuaObject* obj = NewVectorObject( L, vec );
	Lua()->Push( obj );
	obj->UnReference();
}

/*LUA_FUNCTION( ModelInfo )
{
	Lua()->CheckType(1, GLua::TYPE_STRING );

	const model_t* pModel = g_IVModelInfo->FindOrLoadModel( Lua()->GetString(1) );
	studiohdr_t* sModel = g_IVModelInfo->GetStudiomodel( pModel );

	ILuaObject* pInfo = Lua()->GetNewTable();

	pInfo->SetMember( "translucent", g_IVModelInfo->IsTranslucent( pModel ) );
	pInfo->SetMember( "translucenttwopass", g_IVModelInfo->IsTranslucentTwoPass( pModel ) );
	pInfo->SetMember( "hasmaterialproxy", g_IVModelInfo->ModelHasMaterialProxy( pModel ) );
	pInfo->SetMember( "planecount", (float) g_IVModelInfo->GetBrushModelPlaneCount( pModel ) );
	pInfo->SetMember( "numtextures", (float) sModel->numtextures );

	Lua()->Push( pInfo );

	pInfo->UnReference();
	return 1;
}*/

/*LUA_FUNCTION( GetModelMaterialColorAndLighting )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	
	const model_t* pModel = GetEntityModel(L,1);

	ICollideable *pCollide = g_IVModelInfo->GetVCollide( pModel )->solids;

	// Ask the model info about what we need to know
	g_IVModelInfo->GetModelMaterialColorAndLighting( pModel, pCollide->GetCollisionOrigin(),
		pCollide->GetCollisionAngles(), trace, diffuseColor, baseColor );
	return 1;
}*/

LUA_FUNCTION( IsTranslucent )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	const model_t* pModel = GetEntityModel(L,1);
	Lua()->Push( g_IVModelInfo->IsTranslucent( pModel ) );
	return 1;
}

LUA_FUNCTION( IsTranslucentTwoPass )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	const model_t* pModel = GetEntityModel(L,1);
	Lua()->Push( g_IVModelInfo->IsTranslucentTwoPass( pModel ) );
	return 1;
}

LUA_FUNCTION( ModelHasMaterialProxy )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	const model_t* pModel = GetEntityModel(L,1);
	Lua()->Push( g_IVModelInfo->ModelHasMaterialProxy( pModel ) );
	return 1;
}

LUA_FUNCTION( GetBrushModelPlaneCount )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	const model_t* pModel = GetEntityModel(L,1);
	Lua()->Push( (float) g_IVModelInfo->GetBrushModelPlaneCount( pModel ) );
	return 1;
}

LUA_FUNCTION( GetBrushModelPlane )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );

	const model_t* pModel = GetEntityModel(L,1);
	
	cplane_t localPlane;
	Vector vecOrigin(0,0,0);

	g_IVModelInfo->GetBrushModelPlane( pModel, Lua()->GetInteger(), localPlane, &vecOrigin );

	//Figured pushing all 3 seperatly would be better than this
	/*ILuaObject* pPlane = Lua()->GetNewTable();

	pPlane->SetMember( "normal", NewVectorObject( L, localPlane.normal ) );
	pPlane->SetMember( "dist", localPlane.dist );

	PushVector( L, vecOrigin );
	Lua()->Push( pPlane );

	pPlane->UnReference();
	return 2;*/

	PushVector( L, vecOrigin );
	PushVector( L, localPlane.normal );
	Lua()->Push( (float) localPlane.dist );

	return 3;
}

LUA_FUNCTION( GetNumTextures )
{	
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* sModel = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	Lua()->Push( (float) sModel->numtextures );
	return 1;
}

LUA_FUNCTION( GetAllTextures )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );

	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	
	ILuaObject* pTable = Lua()->GetNewTable();

	for( int i = 0; i < pStudio->numtextures; i++ ) {
		// materials are a dumb bitch
		mstudiotexture_t* pTexture = pStudio->pTexture( i );
		
		char* pszTextureName = pTexture->pszName();
		char* pszTexturePath = pStudio->pCdtexture( pTexture->used ); //"";
		
		char pszFullPath[ MAX_PATH ];
		strcpy( pszFullPath, pszTexturePath );
		
		strcat( pszFullPath, pszTextureName );
		
		Q_FixSlashes( pszFullPath, '/' );
		
		pTable->SetMember( i + 1, pszFullPath );
	}

	Lua()->Push( pTable );
	pTable->UnReference();

	return 1;
}

LUA_FUNCTION( GetModelContents )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	int modelIndex = GetEntityModelIndex( L, 1 );
	Lua()->Push( (float) g_IVModelInfo->GetModelContents( modelIndex ) );
	return 1;
}

LUA_FUNCTION( GetNumPoseParamaters )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );	
	Lua()->Push( (float) pStudio->numlocalposeparameters );
	return 1;
}

LUA_FUNCTION( GetPoseParamaterName )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	mstudioposeparamdesc_t* pPoseParam = pStudio->pLocalPoseParameter( Lua()->GetInteger(2) );	
	Lua()->Push( pPoseParam->pszName() );
	return 1;
}

LUA_FUNCTION( GetPoseParamaters )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	
	ILuaObject* pTable = Lua()->GetNewTable();
	
	for( int i = 0; i < pStudio->numlocalposeparameters; i++ )
	{
		mstudioposeparamdesc_t* pPoseParam = pStudio->pLocalPoseParameter( i );
		pTable->SetMember( i + 1, pPoseParam->pszName() );
	}
	
	Lua()->Push( pTable );
	pTable->UnReference();
	
	return 1;
}

LUA_FUNCTION( GetNumBodyGroups )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	Lua()->Push( (float) pStudio->numbodyparts );
	return 1;
}

LUA_FUNCTION( GetBodyGroupName )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	
	mstudiobodyparts_t* pBodyPart = pStudio->pBodypart( Lua()->GetInteger(2) );
	Lua()->Push( pBodyPart->pszName() );
	return 1;
}

LUA_FUNCTION( GetBodyGroups )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	
	ILuaObject* pTable = Lua()->GetNewTable();
	
	for( int i = 0; i < pStudio->numbodyparts; i++ )
	{
		mstudiobodyparts_t* pBodyPart = pStudio->pBodypart( i );
		pTable->SetMember( i + 1, pBodyPart->pszName() );
	}
	
	Lua()->Push( pTable );
	pTable->UnReference();
	
	return 1;
}

LUA_FUNCTION( GetNumAttachments )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );	
	Lua()->Push( (float) pStudio->numlocalattachments );
	return 1;
}

LUA_FUNCTION( GetAttachmentName )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	mstudioattachment_t* pAttachment = pStudio->pLocalAttachment( Lua()->GetInteger(2) );
	Lua()->Push( pAttachment->pszName() );	
	return 1;
}

LUA_FUNCTION( GetAttachments )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	
	ILuaObject* pTable = Lua()->GetNewTable();
	
	for( int i = 0; i < pStudio->numlocalattachments; i++ )
	{
		mstudioattachment_t* pAttachment = pStudio->pLocalAttachment( i );
		pTable->SetMember( i + 1, pAttachment->pszName() );
	}
	
	Lua()->Push( pTable );
	pTable->UnReference();
	
	return 1;
}

inline int GetNumSeq( studiohdr_t* pStudio )
{
	if (pStudio->numincludemodels == NULL)
		return pStudio->numlocalseq;

	virtualmodel_t *pVModel = g_IVModelInfo->GetVirtualModel( pStudio );
	return pVModel->m_seq.Count();
}

const inline mstudioseqdesc_t* pSeqdesc( studiohdr_t* m_pStudioHdr, int i = 0 )
{

	if (m_pStudioHdr->numincludemodels == 0) 
		return m_pStudioHdr->pLocalSeqdesc( i );

	virtualmodel_t *pVModel = g_IVModelInfo->GetVirtualModel( m_pStudioHdr );
	Assert( pVModel );

	if ( !pVModel )
		return m_pStudioHdr->pLocalSeqdesc( i );
	
	//virtualgroup_t *pGroup = pVModel->pSeqGroup( i );
	virtualgroup_t *pGroup = &pVModel->m_group[ pVModel->m_seq[i].group ];

	//const studiohdr_t *pStudioHdr = pGroup->GetStudioHdr();
	const studiohdr_t *pStudioHdr = g_IVModelInfo->FindModel( pGroup->cache );
	Assert( pStudioHdr );

	return pStudioHdr->pLocalSeqdesc( pVModel->m_seq[i].index );
}

const inline mstudioanimdesc_t* pAnimdesc( studiohdr_t* m_pStudioHdr, int i = 0 )
{ 
	if (m_pStudioHdr->numincludemodels == 0)
		return m_pStudioHdr->pLocalAnimdesc( i );

	virtualmodel_t *pVModel = g_IVModelInfo->GetVirtualModel( m_pStudioHdr );
	Assert( pVModel );

	virtualgroup_t *pGroup = &pVModel->m_group[ pVModel->m_anim[i].group ];
	//const studiohdr_t *pStudioHdr = pGroup->GetStudioHdr();
	const studiohdr_t *pStudioHdr = g_IVModelInfo->FindModel( pGroup->cache );
	Assert( pStudioHdr );

	return pStudioHdr->pLocalAnimdesc( pVModel->m_anim[i].index );
}

inline bool SequencesAvailable( studiohdr_t* pStudioHdr )
{
	if (pStudioHdr->numincludemodels == 0)
		return true;

	return ( g_IVModelInfo->GetVirtualModel( pStudioHdr ) != NULL );
}

LUA_FUNCTION( LookupSequenceEx )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_STRING );

	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );

	if (!SequencesAvailable( pStudio ) )
		Lua()->Push( (float) -1 );

	for (int i = 0; i < GetNumSeq( pStudio ); i++)
	{
		const mstudioseqdesc_t* pSequ = pSeqdesc( pStudio, i );
		if (stricmp( pSequ->pszLabel(), Lua()->GetString(2) ) == 0)
		{
			Lua()->Push( (float) i );
			return 1;
		}
	}

	Lua()->Push( (float) -1 );
	return 1;
}

LUA_FUNCTION( GetNumSequences )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );

	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );

	Lua()->Push( (float) GetNumSeq( pStudio ) );
	return 1;
}

/*LUA_FUNCTION( GetSequenceName )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );
	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	mstudioseqdesc_t *pSequ = pSeqdesc( pStudio, Lua()->GetInteger(2) );
	
	Lua()->Push( pSequ->pszLabel() );
	return 1;
}*/

LUA_FUNCTION( GetSequenceFPS )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );

	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	const mstudioanimdesc_t* pAnim = pAnimdesc( pStudio, Lua()->GetInteger(2) );

	Lua()->Push( pAnim->fps );
	return 1;
}

LUA_FUNCTION( GetNumSequenceFrames )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );

	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	const mstudioanimdesc_t* pAnim = pAnimdesc( pStudio, Lua()->GetInteger(2) );

	Lua()->Push( (float) pAnim->numframes );
	return 1;
}

LUA_FUNCTION( GetSequenceInfo )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );

	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	const mstudioanimdesc_t* pAnim = pAnimdesc( pStudio, Lua()->GetInteger(2) );
	
	ILuaObject* pTable = Lua()->GetNewTable( );
	{
		pTable->SetMember( "name",	pAnim->pszName() );
		pTable->SetMember( "fps",	pAnim->fps );
		pTable->SetMember( "frames", (float) pAnim->numframes );
	}
	Lua()->Push( pTable );
	
	return 1;
}

inline int InsertSequenceInfoForStudio( lua_State* L, studiohdr_t* pStudio, ILuaObject* pTable, int indexOffset = 1, bool doIncludes = false )
{
	for( int i = 0; i < pStudio->numlocalanim; i++ )
	{
		const mstudioanimdesc_t* pAnim = pAnimdesc( pStudio, i );
		const mstudioseqdesc_t* pSequ = pSeqdesc( pStudio, i );
		
		ILuaObject* pTableInfo = Lua()->GetNewTable();

			pTableInfo->SetMember( "name",	pSequ->pszLabel() );
			pTableInfo->SetMember( "fps",	pAnim->fps );
			pTableInfo->SetMember( "frames", (float) pAnim->numframes );

			pTable->SetMember( indexOffset + i, pTableInfo );

		pTableInfo->UnReference();
	}

	if( doIncludes ) {
		for( int i = 0; i < pStudio->numincludemodels; i++ )
		{
			mstudiomodelgroup_t* pModelGroup = pStudio->pModelGroup( i );
			studiohdr_t* psubStudio = g_IVModelInfo->GetStudiomodel( g_IVModelInfo->FindOrLoadModel( pModelGroup->pszName() ) );
			indexOffset += InsertSequenceInfoForStudio( L, psubStudio, pTable, indexOffset, doIncludes ); // Recursion ftw
		}
	}

	return pStudio->numlocalanim;
}

//Attempt to index a nil value error, dunno how to fix
/*LUA_FUNCTION( GetAllSequenceInfo )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );

	bool doIncludes = ( Lua()->GetBool(2) != NULL ? Lua()->GetBool(2) : false );

	ILuaObject* pTable = Lua()->GetNewTable();

	int endIndex = InsertSequenceInfoForStudio( L, pStudio, pTable, 1, doIncludes );

	Lua()->Push( pTable );
	pTable->UnReference();
	
	return 1;
}*/

LUA_FUNCTION( GetSubModelSequenceInfo )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );

	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	bool doIncludes = ( Lua()->GetBool(2) != NULL ? Lua()->GetBool(2) : false );

	ILuaObject* pTable = Lua()->GetNewTable();

	mstudiomodelgroup_t* pModelGroup = pStudio->pModelGroup( Lua()->GetInteger(2) );
	studiohdr_t* psubStudio = g_IVModelInfo->GetStudiomodel( g_IVModelInfo->FindOrLoadModel( pModelGroup->pszName() ) );
	InsertSequenceInfoForStudio( L, psubStudio, pTable, 1, doIncludes );

	Lua()->Push( pTable );
	pTable->UnReference();
	
	return 1;
}

LUA_FUNCTION( IsModelVertexLit )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	const model_t* pModel = GetEntityModel(L,1);
	Lua()->Push( g_IVModelInfo->IsModelVertexLit( pModel ) );
	return 1;
}

LUA_FUNCTION( GetModelName )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	const model_t* pModel = GetEntityModel(L,1);
	Lua()->Push( g_IVModelInfo->GetModelName( pModel ) );
	return 1;
}

LUA_FUNCTION( GetNumSubModels )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	studiohdr_t* pStudio = g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );	
	Lua()->Push( (float) pStudio->numincludemodels );
	return 1;
}

LUA_FUNCTION( GetSubModelName )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	Lua()->CheckType(2, GLua::TYPE_NUMBER );

	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( GetEntityModel(L,1) );
	
	mstudiomodelgroup_t* pModelGroup = pStudio->pModelGroup( Lua()->GetInteger(2) );	
	Lua()->Push( pModelGroup->pszName() );
	return 1;
}

inline int InsertIncludedModels( lua_State* L, studiohdr_t* pStudio, ILuaObject* pTable, int indexOffset = 1, bool doIncludes = false )
{
	for( int i = 0; i < pStudio->numincludemodels; i++ )
	{
		mstudiomodelgroup_t* pModelGroup = pStudio->pModelGroup( i );
		pTable->SetMember( indexOffset + i, pModelGroup->pszName() );

		if( doIncludes )
		{
			studiohdr_t* psubStudio = g_IVModelInfo->GetStudiomodel( g_IVModelInfo->FindOrLoadModel( pModelGroup->pszName() ) );
			indexOffset += InsertIncludedModels( L, psubStudio, pTable, indexOffset, doIncludes ); // Recursion ftw
		}
	}
	return pStudio->numincludemodels;
}


LUA_FUNCTION( GetSubModels )
{
	Lua()->CheckType(1, GLua::TYPE_ENTITY );
	const model_t* pModel = GetEntityModel(L,1);
	bool doIncludes = ( Lua()->GetBool(2) != NULL ? Lua()->GetBool(2) : false );

	studiohdr_t* pStudio	= g_IVModelInfo->GetStudiomodel( pModel );
	
	ILuaObject* pTable = Lua()->GetNewTable();
	
	InsertIncludedModels( L, pStudio, pTable, 1, doIncludes );
	
	Lua()->Push( pTable );
	pTable->UnReference();
	
	return 1;
}

/*void DumpVTable(const char* name, const char* modulename, void *object)
{
	MODULEINFO mi;
	memset(&mi, 0, sizeof(MODULEINFO));

	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(modulename), &mi, sizeof(MODULEINFO));	

	void ***mem = (void ***)object;
	size_t base = (size_t)mi.lpBaseOfDll;
	size_t imgsz = (size_t)mi.SizeOfImage;
	size_t dlloffsetbase = 0x10000000;

	Msg("Dumping %s VTable @ dlloffset %p:\n", name, (void *)((size_t)(*mem) - base + dlloffsetbase));

	for(int i = 0; (size_t)(*mem)[i] - base < imgsz; i++)
	{
		Msg("\tindex=%d addr=%p dlloffset=%p\n", i, (*mem)[i], (void *)((size_t)(*mem)[i] - base + dlloffsetbase));
	}
	
	Msg("\n");
}*/

int Init( lua_State *L )
{
	
	CreateInterfaceFn EngineFactory = Sys_GetFactory( ENGINE_LIB );
	if ( !EngineFactory )
		Lua()->Error( "gm_modelinfo: Error getting " ENGINE_LIB " factory.\n" );

	g_IVModelInfo = ( IVModelInfo* )EngineFactory( Lua()->IsClient() ? VMODELINFO_CLIENT_INTERFACE_VERSION : VMODELINFO_SERVER_INTERFACE_VERSION, NULL );
	if ( !g_IVModelInfo )
		Lua()->Error( "gm_modelinfo: Error getting IVModelInfo interface.\n" );

	/*g_IMDLCache = ( IMDLCache* )EngineFactory( MDLCACHE_INTERFACE_VERSION, NULL );
	if ( !g_IMDLCache )
		Lua()->Error( "gm_modelinfo: Error getting IMDLCache interface.\n" );*/

	//DumpVTable( "IVModelInfo", ENGINE_LIB, g_IVModelInfo );

	//Lua()->SetGlobal( "ModelInfo", ModelInfo );

	ILuaObject* entityMeta = Lua()->GetMetaTable( "Entity", GLua::TYPE_ENTITY );
	if( entityMeta )
	{
		entityMeta->SetMember( "GetNumTextures",	GetNumTextures );
		entityMeta->SetMember( "GetAllTextures",	GetAllTextures );
		entityMeta->SetMember( "IsTranslucent",	IsTranslucent );
		entityMeta->SetMember( "IsTranslucentTwoPass",	IsTranslucentTwoPass );
		entityMeta->SetMember( "ModelHasMaterialProxy",	ModelHasMaterialProxy );
		entityMeta->SetMember( "GetModelContents",	GetModelContents );
		entityMeta->SetMember( "GetBrushModelPlaneCount", GetBrushModelPlaneCount );
		entityMeta->SetMember( "GetBrushModelPlane", GetBrushModelPlane );
		entityMeta->SetMember( "GetNumPoseParamaters", GetNumPoseParamaters );
		entityMeta->SetMember( "GetPoseParamaterName", GetPoseParamaterName );
		entityMeta->SetMember( "GetPoseParamaters", GetPoseParamaters );
		entityMeta->SetMember( "GetNumBodyGroups", GetNumBodyGroups );
		entityMeta->SetMember( "GetBodyGroupName", GetBodyGroupName );
		entityMeta->SetMember( "GetBodyGroups", GetBodyGroups );
		entityMeta->SetMember( "GetNumAttachments", GetNumAttachments );
		entityMeta->SetMember( "GetAttachmentName", GetAttachmentName );
		entityMeta->SetMember( "GetAttachments", GetAttachments );
		entityMeta->SetMember( "GetNumSequences", GetNumSequences );
		entityMeta->SetMember( "LookupSequenceEx", LookupSequenceEx );
		entityMeta->SetMember( "GetSequenceFPS", GetSequenceFPS );
		entityMeta->SetMember( "GetNumSequenceFrames", GetNumSequenceFrames );
		//entityMeta->SetMember( "GetSequenceName", GetSequenceName );
		entityMeta->SetMember( "GetSequenceInfo", GetSequenceInfo );
		//entityMeta->SetMember( "GetAllSequenceInfo", GetAllSequenceInfo );
		entityMeta->SetMember( "GetSubModelSequenceInfo", GetSubModelSequenceInfo );
		entityMeta->SetMember( "IsModelVertexLit", IsModelVertexLit );
		//entityMeta->SetMember( "GetModelName", GetModelName );
		entityMeta->SetMember( "GetNumSubModels", GetNumSubModels );
		entityMeta->SetMember( "GetSubModelName", GetSubModelName );
		entityMeta->SetMember( "GetSubModels", GetSubModels );
	}
	entityMeta->UnReference();
	
	return 0;
}

int Shutdown(lua_State *L)
{
	return 0;
}