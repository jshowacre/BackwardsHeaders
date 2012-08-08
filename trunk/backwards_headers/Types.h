
#ifndef GARRYSMOD_LUA_TYPES_H
#define GARRYSMOD_LUA_TYPES_H

#ifdef ENTITY
#undef ENTITY
#endif 

#ifdef VECTOR
#undef VECTOR
#endif 

namespace GarrysMod 
{
	namespace Lua
	{
		namespace Type
		{
			enum
			{
				INVALID = -1,
				NIL, 
				STRING, 
				NUMBER, 
				TABLE,
				BOOL,
				FUNCTION,
				THREAD,

				// UserData
				ENTITY, 
				VECTOR, 
				ANGLE,
				PHYSOBJ,
				SAVE,
				RESTORE,
				DAMAGEINFO,
				EFFECTDATA,
				MOVEDATA,
				RECIPIENTFILTER,
				USERCMD,
				SCRIPTEDVEHICLE,

				// Client Only
				MATERIAL,
				PANEL,
				PARTICLE,
				PARTICLEEMITTER,
				TEXTURE,
				USERMSG,

				CONVAR,
				IMESH,
				MATRIX,
				SOUND,
				PIXELVISHANDLE,
				DLIGHT,
				LIGHTUSERDATA,
				VIDEO,
				FILE,

				COUNT
			};

			static const char* Name[] = 
			{
				"nil",
				"string",
				"number",
				"table",
				"bool",
				"function",
				"thread",
				"entity",
				"vector",
				"angle",
				"physobj",
				"save",
				"restore",
				"damageinfo",
				"effectdata",
				"movedata",
				"recipientfilter",
				"usercmd",
				"vehicle",
				"material",
				"panel",
				"particle",
				"particleemitter",
				"texture",
				"usermsg",
				"convar",
				"mesh",
				"matrix",
				"sound",
				"pixelvishandle",
				"dlight",
				"lightuserdata",
				"video",
				"file",

				0
			};
		}
	}
}

#endif 

