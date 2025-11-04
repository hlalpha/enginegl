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

void CBaseEntity::SUB_CallUseToggle( void *funcArgs )
{
	this->Use( NULL );
}

void CBaseEntity::SUB_CallTouchToggle( void *funcArgs )
{
	this->Touch( NULL );
}


/*
QuakeEd only writes a single float for angles (bad idea), so up and down are
just constant angles.
*/
void SetMovedir( entvars_t *pev )
{
	if ( pev->angles == Vector( 0, -1, 0 ) )
	{
		pev->movedir = Vector( 0, 0, 1 );
	}
	else if ( pev->angles == Vector( 0, -2, 0 ) )
	{
		pev->movedir = Vector( 0, 0, -1 );
	}
	else
	{
		UTIL_MakeVectors( pev->angles );
		pev->movedir = gpGlobals->v_forward;
	}

	pev->angles = g_vecZero;
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

			globals->self = OFFSET( pEnt );
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


void CBaseToggle::LinearMove( Vector vecDest, float flSpeed )
{
	m_vecFinalDest = vecDest;

	// Already there?
	if ( vecDest == pev->origin )
	{
		LinearMoveDone( NULL );
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDest - pev->origin;

	// divide vector length by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to LinearMoveDone when dest is reached
	pev->nextthink = pev->ltime + flTravelTime;
	SetThink( &CBaseToggle::LinearMoveDone );

	// scale the destdelta vector by the time spent traveling to get velocity
	pev->velocity = vecDestDelta / flTravelTime;
}

void CBaseToggle::LinearMoveDone( void *funcArgs )
{
	UTIL_SetOrigin( pev, m_vecFinalDest );
	pev->velocity = g_vecZero;
	pev->nextthink = -1;
	m_pfnCallWhenMoveDone( NULL );
}

void CBaseToggle::AngularMove( Vector vecDestAngle, float flSpeed )
{
	m_vecFinalAngle = vecDestAngle;

	// Already there?
	if ( vecDestAngle == pev->angles )
	{
		AngularMoveDone( NULL );
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDestAngle - pev->angles;

	// divide by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to AngularMoveDone when dest is reached
	pev->nextthink = pev->ltime + flTravelTime;
	SetThink( &CBaseToggle::AngularMoveDone );

	// scale the destdelta vector by the time spent traveling to get velocity
	pev->avelocity = vecDestDelta / flTravelTime;
}

void CBaseToggle::AngularMoveDone( void *funcArgs )
{
	pev->angles = m_vecFinalAngle;
	pev->avelocity = g_vecZero;
	pev->nextthink = -1;
	m_pfnCallWhenMoveDone( NULL );
}

/*static*/ void CBaseToggle::AxisDir( entvars_t *pev )
{
	if ( FBitSet( pev->spawnflags, 0x40 ) )
		pev->movedir = Vector( 0, 0, 1 );
	else if ( FBitSet( pev->spawnflags, 0x80 ) )
		pev->movedir = Vector( 0, 1, 0 );
	else
		pev->movedir = Vector( 1, 0, 0 );
}

/*static*/ float CBaseToggle::AxisDelta( int flags, Vector &angle1, Vector &angle2 )
{
	if ( FBitSet( flags, 0x40 ) )
		return angle1.z - angle2.z;
	else if ( FBitSet( flags, 0x80 ) )
		return angle1.y - angle2.y;
	else
		return angle1.x - angle2.x;
}