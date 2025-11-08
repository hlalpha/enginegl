// cbase.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"


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

