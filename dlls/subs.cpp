// subs.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"

void CBaseEntity::SUB_Remove( void *funcArgs )
{
	REMOVE_ENTITY( ENT( pev ) );
}

void CBaseEntity::SUB_DoNothing( void *funcArgs )
{
	/* Nothing */
}


void CBaseDelay::SUB_UseTargets( void *funcArgs )
{
	if ( m_flDelay != 0.f )
	{
		CBaseDelay *pDelayedUse = GetClassPtr( (CBaseDelay *)NULL );

		pDelayedUse->pev->classname = ALLOC_STRING( "DelayedUse" );
		pDelayedUse->pev->nextthink = globals->time + m_flDelay;
		pDelayedUse->SetThink( &CBaseDelay::DelayThink );
		pDelayedUse->m_iszKillTarget = m_iszKillTarget;
		pDelayedUse->m_flDelay = 0.f;
		pDelayedUse->pev->target = pev->target;

		return;
	}

	if ( m_iszKillTarget )
	{
		edict_t *pEnt = FIND_ENTITY_BY_STRING( NULL, "targetname", m_iszKillTarget );
		while ( FNullEnt( pEnt ) )
		{
			REMOVE_ENTITY( pEnt );

			pEnt = FIND_ENTITY_BY_STRING( pEnt, "targetname", m_iszKillTarget );
		}
	}

	if ( !FStringNull( pev->target ) )
	{
		int oldSelf = globals->self;
		int oldOther = globals->other;

		edict_t *pEnt = NULL;

		for (;;)
		{
			pEnt = FIND_ENTITY_BY_STRING( pEnt, "targetname", STRING( pev->target ) );
			if ( FNullEnt ( pEnt ) )
				break;

			globals->self = g_engfuncs.pfnEntOffsetOfPEntity( pEnt );
			globals->other = oldSelf;

			CPointEntity *pTarget = GetClassPtr( (CPointEntity *)pEnt );
			pTarget->Use( NULL );

			globals->self = oldSelf;
			globals->other = oldOther;
		}
	}
}

void CBaseDelay::DelayThink( void *funcArgs )
{
	SUB_UseTargets( NULL );
	REMOVE_ENTITY( ENT( pev ) );
}
