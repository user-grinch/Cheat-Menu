#pragma once

class RPC
{
private:
    typedef struct 
    {
		const char* state;
		const char* details;
		long long startTimestamp;
		long long endTimestamp;
		const char* largeImageKey;
		const char* largeImageText;
		const char* smallImageKey;
		const char* smallImageText;
		const char* partyId;
		int partySize;
		int partyMax;
		const char* matchSecret;
		const char* joinSecret;
		const char* spectateSecret;
		char instance;
	} DiscordRichPresence;
    
    static inline FARPROC f_Init, f_ShutDown, f_Update;
    static inline DiscordRichPresence drpc;
    static inline HMODULE hDll = NULL;
    static inline bool bInit;
	
public:
    RPC() = delete;
    RPC(RPC&) = delete;

    static void Init();
    static void Process();
    static void Shutdown();
};