#include <queue>

#include <windows.h>
#include <sapi.h>

#define GMMODULE

#include "ILuaModuleManager.h"

ISpVoice* Voice = NULL;
USHORT DefaultVolume;
long DefaultRate;
ISpObjectToken* DefaultVoice;

GMOD_MODULE( Init, Shutdown );

//gspeak.Say(msg, volume (0~100), rate (-10~10))
typedef struct
{
	LPCWSTR msg;
	USHORT volume;
	short rate;
	int args;
} SAYINFO;

static void Speak();

std::queue<SAYINFO> SpeakQueue;
HANDLE SpeakerThread = NULL;
bool Paused = false;

LUA_FUNCTION( Say )
{
	ILuaInterface* gLua = Lua();
	//form message
	const char* myString = gLua->GetString(1);

	SAYINFO info;

	// Find out required string size
	int iRequiredSize = ::MultiByteToWideChar(CP_ACP, NULL, myString, -1, NULL, 0);

	// Alloc buffer for converted string
	WCHAR* pwchString = new WCHAR[iRequiredSize];

	// Do the string conversion
	::MultiByteToWideChar(CP_ACP, NULL, myString , -1, pwchString, iRequiredSize);
	
	info.msg = pwchString;
	info.args = gLua->Top();
	
	//volume and rate optional arguments
	if(info.args > 1)
	{
		info.volume = gLua->GetInteger(2);

		if(info.args > 2)
			info.rate = gLua->GetInteger(3);
	}

	SpeakQueue.push(info);
	
	DWORD Status;
	GetExitCodeThread(SpeakerThread, &Status);

	if(Status != STILL_ACTIVE)
		SpeakerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Speak, NULL, 0, NULL);
	
	return 0;
}

LUA_FUNCTION(IsSpeaking)
{
	ILuaInterface* gLua = Lua();

	if(Paused)
		gLua->Push(false);
	else
		gLua->Push(!SpeakQueue.empty());
	return 1;
}

LUA_FUNCTION(QueueSize)
{
	ILuaInterface* gLua = Lua();
	gLua->Push(static_cast<float>(SpeakQueue.size()));
	return 1;
}

LUA_FUNCTION(Pause)
{
	if(Paused || SpeakQueue.empty()) 
		return 0;
	
	Voice->Pause();
	Paused = true;
	return 0;
}

LUA_FUNCTION(Resume)
{
	Voice->Resume();
	Paused = false;
	return 0;
}

//thread loop
static void Speak()
{
	while(!SpeakQueue.empty())
	{
		SAYINFO info = SpeakQueue.front();

		if(info.args > 1)
		{
			Voice->SetVolume(info.volume);

			if(info.args > 2)
				Voice->SetRate(info.rate);
		}

		Voice->SetVoice(DefaultVoice);
		
		Voice->Speak(info.msg, 0, NULL);
		
		Voice->SetVolume(DefaultVolume);
		Voice->SetRate(DefaultRate);
		
		delete info.msg;
		SpeakQueue.pop();
	}
}

int Init( lua_State *L )
{
	if (FAILED(::CoInitialize(NULL)))
        return 1;

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&Voice);
    if( !SUCCEEDED( hr ) )
        return 1;

	Voice->GetVolume(&DefaultVolume);
	Voice->GetRate(&DefaultRate);
	Voice->GetVoice(&DefaultVoice);

	ILuaInterface* gLua = Lua();

	ILuaObject* tts = gLua->GetNewTable();
		tts->SetMember( "Say", Say );
		tts->SetMember( "IsSpeaking", IsSpeaking );
		tts->SetMember( "QueueSize", QueueSize );
		tts->SetMember( "Pause", Pause );
		tts->SetMember( "Resume", Resume );
		gLua->SetGlobal( "tts", tts );
	tts->UnReference();
	return 0;
}

//Shut down SAPI
int Shutdown( lua_State *L )
{
	DWORD Status;
	GetExitCodeThread(SpeakerThread, &Status);

	if(Status == STILL_ACTIVE)
		TerminateThread(SpeakerThread, 0);

	while(!SpeakQueue.empty())
	{
		SAYINFO info = SpeakQueue.front();
		delete info.msg;
		SpeakQueue.pop();
	}

	Voice->Release();
    Voice = NULL;

	::CoUninitialize();
	return 0;
}

