#include "extdll.h"
#include "util.h"
#include "cbase.h"

// QC Wrapped Functions
extern "C" __declspec(dllexport) void DispatchSpawn( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
	if ( pBaseEntity )
			pBaseEntity->Spawn();
}

extern "C" __declspec(dllexport) void DispatchThink( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
	if ( pBaseEntity )
			pBaseEntity->Think( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchUse( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
	if ( pBaseEntity )
			pBaseEntity->Use( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchTouch( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
	if ( pBaseEntity )
			pBaseEntity->Touch( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchSave( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
	if ( pBaseEntity )
			pBaseEntity->Save( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchRestore( entvars_t *pev, void *funcArgs )
{
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
	if ( pBaseEntity )
			pBaseEntity->Restore( funcArgs );
}

extern "C" __declspec(dllexport) void DispatchKeyValue( entvars_t *pev, KeyValueData *pkvd )
{
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
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
	CBaseEntity *pBaseEntity = (CBaseEntity *)GET_PRIVATE( ENT( pev ) );
	if ( pBaseEntity )
		pBaseEntity->Blocked( funcArgs );
}

extern "C" __declspec(dllexport) void __stdcall ClientConnect( globalvars_t *pglobals )
{
}


CBaseEntity::CBaseEntity()
{
}

void CBaseEntity::Spawn()
{
}

void CBaseEntity::KeyValue( KeyValueData *pkvd )
{
}

void CBaseEntity::UNKNOWN( void *funcArgs )
{
}

void CBaseEntity::Save( void *funcArgs )
{
	ALERT( at_log, "Saving %s\n", STRING( pev->classname ) );
}

void CBaseEntity::Restore( void *funcArgs )
{
}

void CBaseEntity::Think( void *funcArgs )
{
	if ( m_pfnThink )
		(this->*m_pfnThink)( funcArgs );
}

void CBaseEntity::Touch( void *funcArgs )
{
	if ( m_pfnTouch )
		(this->*m_pfnTouch)( funcArgs );
}

void CBaseEntity::Use( void *funcArgs )
{
	if ( m_pfnUse )
		(this->*m_pfnUse)( funcArgs );
}

void CBaseEntity::Blocked( void *funcArgs )
{
	if ( m_pfnBlocked )
		(this->*m_pfnBlocked)( funcArgs );
}
