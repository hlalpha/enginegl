#if !defined( SYS_DLL_H )
#define SYS_DLL_H

#include "eiface.h"

typedef struct
{
	HANDLE lDLLHandle;
} extensiondll_t;

typedef void (__stdcall *GIVEFNPTRSTODLL_PROC)( enginefuncs_t *pengfuncsFromEngine ); // Prototype for "GiveFnptrsToDll"
typedef void (__cdecl *DISPATCHFUNC)( entvars_t *pev, void *funcArgs ); // Prototype for "Dispatch*"
typedef void (__stdcall *QCCFUNC)( globalvars_t *pglobals );

extern DISPATCHFUNC g_pfnDispatchSpawn;
extern DISPATCHFUNC g_pfnDispatchThink;
extern DISPATCHFUNC g_pfnDispatchUse;
extern DISPATCHFUNC g_pfnDispatchTouch;
extern DISPATCHFUNC g_pfnDispatchSave;
extern DISPATCHFUNC g_pfnDispatchRestore;
extern DISPATCHFUNC g_pfnDispatchKeyValue;
extern DISPATCHFUNC g_pfnDispatchBlocked;

typedef struct
{
	QCCFUNC func;
} qccwrap_t;

#define DISPATCHFUNC_SPAWN 0
#define DISPATCHFUNC_THINK 1
#define DISPATCHFUNC_TOUCH 2
#define DISPATCHFUNC_USE 3
#define DISPATCHFUNC_BLOCKED 4
#define DISPATCHFUNC_KEYVALUE 5
#define DISPATCHFUNC_SAVE 6

extern enginefuncs_t g_engfuncsExportedToDlls;

DISPATCHFUNC GetDispatchFuncById( edict_t *ent, int dllFunc );
void CallDispatchFunc( edict_t *ent, int dllFunc, void *funcArg );

void ED_SetGameDLLVars( edict_t *ent );
void *ED_AllocatePrivateData( edict_t *pEdict, int size );
void *ED_GetPrivateData( edict_t *pEdict );
void ED_FreePrivateData( edict_t *pEdict );

DISPATCHFUNC FunctionFromName( LPCSTR lpProcName );
void LoadServerDLL( const char *pBaseDir );
void LoadThisDll( LPCSTR lpLibFileName );
void ReleaseEntityDlls();

int EngineFprintf( FILE *f, char *format, ... );
void AlertMessage( int atype, const char *szFmt, ... );

void PR_ExecuteProgramFromDLL( int nProgram );

#endif // !defined( SYS_DLL_H )