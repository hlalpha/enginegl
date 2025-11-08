// h_dispatch.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"

// QC Wrapped Functions
extern "C" __declspec(dllexport) void DispatchSpawn( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
			pBaseEntity->Spawn();
}

extern "C" __declspec(dllexport) void DispatchThink( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
			pBaseEntity->Think( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchUse( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
			pBaseEntity->Use( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchTouch( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
			pBaseEntity->Touch( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchSave( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
			pBaseEntity->Save( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchRestore( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
			pBaseEntity->Restore( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchKeyValue( entvars_t *pev, KeyValueData *pkvd )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
			pBaseEntity->KeyValue( pkvd );

	// FIXME(SanyaSho): What does this do???
	if ( pkvd && !pkvd->fHandled )
	{
		const char *key = pkvd->szKeyName;
		const char *cmp = "transitionid";

		while ( *key && *cmp )
		{
			if ( key[0] != cmp[0] || key[1] != cmp[1] )
				break;

			key += 2;
			cmp += 2;
		}
	}
}

extern "C" __declspec(dllexport) void DispatchBlocked( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = CBaseEntity::Instance( pev );
	if ( pBaseEntity )
		pBaseEntity->Blocked( funcArgs );
}

