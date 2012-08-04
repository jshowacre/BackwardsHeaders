#ifdef _WIN32
#pragma once;
#endif

#ifdef GMOD_BETA
#define GMODLUASHAREDINTERFACE "LuaShared002"
#else
#define GMODLUASHAREDINTERFACE "LuaShared001"
#endif

struct ScriptData
{
        char path[MAX_PATH];
        int crc;
        char* contents;
        int timestamp;
        bool somebool;
};

#ifndef NO_SDK
        typedef CUtlVector<char[64]> AddonVector; // this may be a struct, but this can be solved with amazing C++ typedefs
#else
        typedef void* AddonVector;
#endif

class ILuaShared
{
public:
	virtual ~ILuaShared();
#ifdef GMOD_BETA
	virtual void GetInterfaceByState(lua_State *);
	virtual void Init(void * (*)(char  const*,int *),bool,void * /* CSteamAPIContext* */,void * /* IGet* */);
	virtual void Shutdown(void);
	virtual void DumpStats(void);
	virtual void CreateLuaInterface(ILuaCallback *,unsigned char);
	virtual void CloseLuaInterface(ILuaInterface *);
	virtual ILuaInterface* GetLuaInterface ( unsigned char interfaceNumber );
	virtual void LoadFile(); //LoadFile(std::string  const&,std::string  const&,bool,bool);
	virtual void MountLua(char  const*);
	virtual void MountLuaAdd(char  const*,char  const*);
	virtual void UnMountLua(char  const*);
	virtual void SetFileContents(char  const*,char  const*);
	virtual void SetLuaFindHook(void * /* LuaClientDatatableHook* */);
	//virtual void FindScripts(std::string  const&,std::string  const&,std::vector<std::string,std::allocator<std::string>> &);
#else
	virtual void DumpStats(void);
	virtual void GetFile(char  const*,char  const*,bool,bool,bool *);
	virtual void FileExists(char  const*,char  const*,bool,bool,bool *);
	virtual void SetTranslateHook(void * /* ILuaTranslate* */);
	virtual void LZMACompress(unsigned char *,unsigned int);
	virtual void MountContent(void * /* IGarrysMod* */);
	virtual void MountAddons(void * /* IGarrysMod* */);
	virtual void MountGamemodes(void * /* IGarrysMod* */);
	virtual void MountLua(char  const*,bool);
	virtual void MountLuaAdd(char  const*,char  const*);
	virtual void UnMountLua(char  const*);
	virtual void CloseLuaInterface(ILuaInterface *);
	virtual ILuaInterface* GetLuaInterface ( unsigned char interfaceNumber );
	virtual ILuaInterface* CreateLuaInterface(ILuaCallback *,unsigned char);
	virtual void GetAddonList(void);
	virtual void GetGamemodeList(void);
	virtual void Init(void * (*)(char  const*,int *),bool,void * /* CSteamAPIContext* */);
	virtual void LoadCache(void);
	virtual void SaveCache(void);
	virtual void Shutdown(void);
	virtual void GetContentList(void);
	virtual void GetCommaSeperatedContentList(void);
	virtual void GetInterfaceByState(lua_State *);
	virtual void SetDepotMountable(char  const*,bool);
#endif
};