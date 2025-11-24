// plats.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "doors.h"


void PlatSpawnInsideTrigger( entvars_t *pevPlatform );


class CBasePlatTrain : public CBaseToggle
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
};

void CBasePlatTrain::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "lip" ) )
	{
		m_flLip = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "height" ) )
	{
		m_flHeight = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
}


//
// func_plat
//
class CFuncPlat : public CBasePlatTrain
{
public:
	virtual void Spawn();
	void PlatUse( void *funcArgs );
	void GoDown( void *funcArgs );
	void CallHitBottom( void *funcArgs );
	void GoUp( void *funcArgs );
	void CallHitTop( void *funcArgs );
	virtual void Blocked( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( func_plat, CFuncPlat );

void CFuncPlat::Spawn()
{
	PRECACHE_SOUND( "plats/platmove1.wav" );
	PRECACHE_SOUND( "plats/platstop1.wav" );

	pev->noise = ALLOC_STRING( "plats/platmove1.wav" );
	pev->noise1 = ALLOC_STRING( "plats/platstop1.wav" );

	if ( m_flTLength == 0 )
		m_flTLength = 80;
	if ( m_flTWidth == 0 )
		m_flTWidth = 10;

	pev->angles = g_vecZero;

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize( pev, pev->mins, pev->maxs );
	SET_MODEL( edict(), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 150;

	m_vecPosition1 = m_vecPosition2 = pev->origin;

	if ( m_flHeight != 0 )
		m_vecPosition2.z = pev->origin.z - m_flHeight;
	else
		m_vecPosition2.z = pev->origin.z - pev->size.z + 8.f;

	PlatSpawnInsideTrigger( pev );

	if ( !FStringNull( pev->targetname ) )
	{
		UTIL_SetOrigin( pev, m_vecPosition1 );

		m_toggle_state = TS_AT_TOP;
		SetUse( &CFuncPlat::PlatUse );
	}
	else
	{
		UTIL_SetOrigin( pev, m_vecPosition2 );

		m_toggle_state = TS_AT_BOTTOM;
	}
}


//
// func_plat
//
class CPlatTrigger : public CBaseEntity
{
public:
	void SpawnInsideTrigger( CFuncPlat *pPlatform );
	virtual void Touch( void *funcArgs );

private:
	CFuncPlat *m_pPlatform;
};

void PlatSpawnInsideTrigger( entvars_t *pevPlatform )
{
	GetClassPtr( (CPlatTrigger *)NULL )->SpawnInsideTrigger( GetClassPtr( (CFuncPlat *)pevPlatform ) );
}

void CPlatTrigger::SpawnInsideTrigger( CFuncPlat *pPlatform )
{
	m_pPlatform = pPlatform;

	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;

	Vector vecMins = m_pPlatform->pev->mins + Vector( 25, 25, 0 );
	Vector vecMaxs = m_pPlatform->pev->maxs + Vector( 25, 25, 8 );

	vecMins.z = vecMaxs.z - ( m_pPlatform->m_vecPosition1.z - m_pPlatform->m_vecPosition2.z + 8 );

	if ( m_pPlatform->pev->size.x <= 50 )
	{
		vecMins.x = ( m_pPlatform->pev->mins.x + m_pPlatform->pev->maxs.x ) * 0.5f;
		vecMaxs.x = vecMins.x + 1.f;
	}

	if ( m_pPlatform->pev->size.y <= 50 )
	{
		vecMins.y = ( m_pPlatform->pev->mins.y + m_pPlatform->pev->maxs.y ) * 0.5f;
		vecMaxs.y = vecMins.y + 1.f;
	}

	UTIL_SetSize( pev, vecMins, vecMaxs );
}

void CPlatTrigger::Touch( void *funcArgs )
{
	entvars_t *pToucher = VARS( gpGlobals->other );

	// Must be a alive player!
	if ( !FClassnameIs( pToucher, "player" ) || ( pToucher->health < 0 ) )
		return;

	switch ( m_pPlatform->m_toggle_state )
	{
	case TS_AT_BOTTOM:
		m_pPlatform->GoUp( NULL );
		break;
	case TS_AT_TOP:
		m_pPlatform->pev->nextthink = m_pPlatform->pev->ltime + 1.f;
		break;
	};
}


void CFuncPlat::PlatUse( void *funcArgs )
{
	SetUse( NULL );

	if ( m_toggle_state == TS_AT_TOP )
		GoDown( NULL );
}

void CFuncPlat::GoDown( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 1, STRING( pev->noise ), 1.f, 0.8f );

	m_toggle_state = TS_GOING_DOWN;
	SetMoveDone( &CFuncPlat::CallHitBottom );

	LinearMove( m_vecPosition2, pev->speed );
}

void CFuncPlat::CallHitBottom( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 1, STRING( pev->noise1 ), 1.f, 0.8f );
	m_toggle_state = TS_AT_BOTTOM;
}

void CFuncPlat::GoUp( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 1, STRING( pev->noise ), 1.f, 0.8f );

	m_toggle_state = TS_GOING_UP;
	SetMoveDone( &CFuncPlat::CallHitTop );

	LinearMove( m_vecPosition1, pev->speed );
}

void CFuncPlat::CallHitTop( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 1, STRING( pev->noise1 ), 1.f, 0.8f );
	m_toggle_state = TS_AT_TOP;

	SetThink( &CFuncPlat::GoDown );
	pev->nextthink = pev->ltime + 3.f;
}

void CFuncPlat::Blocked( void *funcArgs )
{
	CBaseMonster *pOther = (CBaseMonster *)GET_PRIVATE( ENT( gpGlobals->other ) );
	pOther->TakeDamage( pev, pev, 1 );

	switch ( m_toggle_state )
	{
	case TS_GOING_UP:
		GoDown( NULL );
		break;
	case TS_GOING_DOWN:
		GoUp( NULL );
		break;
	};
}


//
// func_train
//
class CFuncTrain : public CBasePlatTrain
{
public:
	virtual void Blocked( void *funcArgs );
	virtual void Use( void *funcArgs );
	void Wait( void *funcArgs );
	void Next( void *funcArgs );
	void Activate( void *funcArgs );
	virtual void Spawn();
};

void CFuncTrain::Blocked( void *funcArgs )
{
	entvars_t *pBlocked = VARS( gpGlobals->other );

	if ( gpGlobals->time >= m_flActivateFinished )
	{
		m_flActivateFinished = gpGlobals->time + 0.5f;

		CBaseMonster *pMonster = (CBaseMonster *)GET_PRIVATE( ENT( pBlocked ) );
		pMonster->TakeDamage( pev, pev, pev->dmg );
	}
}

LINK_ENTITY_TO_CLASS( func_train, CFuncTrain );

void CFuncTrain::Use( void *funcArgs )
{
	// Must think about cheese!
	if ( m_pfnThink != &CFuncTrain::Activate )
		return;

	Next( NULL );
}

void CFuncTrain::Wait( void *funcArgs )
{
	entvars_t *pEnemy = VARS( pev->enemy );

	if ( FBitSet( pEnemy->spawnflags, 0x01 ) )
	{
		EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise ), 1.f, 0.8f );

		pev->nextthink = gpGlobals->time + 999999.f;
		SetThink( &CFuncTrain::Activate );
		return;
	}

	if ( m_flWait != 0 )
	{
		pev->nextthink = pev->ltime + m_flWait;

		EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise ), 1.f, 0.8f );

		SetThink( &CFuncTrain::Next );
	}
	else
	{
		Next( NULL ); // now
	}
}

void CFuncTrain::Next( void *funcArgs )
{
	entvars_t *pTarget = VARS( FIND_ENTITY_BY_TARGETNAME( NULL, STRING( pev->target ) ) );
	if ( FNullEnt( pTarget ) )
	{
		ALERT( at_error, "TrainNext--cannot find next target '%s'", STRING( pev->target ) );
		return;
	}

	pev->target = pTarget->target;

	if ( !pev->target )
	{
		ALERT( at_error, "TrainNext: No next target" );
		return;
	}

	CBaseToggle *pTargetToggle = GetClassPtr( (CBaseToggle *)pTarget );

	if ( pTargetToggle->m_flWait != 0 )
		m_flWait = pTargetToggle->m_flWait;
	else
		m_flWait = 0.f;

	EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise1 ), 1.f, 0.8f );

	pev->enemy = OFFSET( pTarget );

	SetMoveDone( &CFuncTrain::Wait );

	LinearMove( pTarget->origin - ( pev->mins + pev->maxs ) * 0.5, pTarget->speed );

	if ( pTarget->speed != 0 )
		pev->speed = pTarget->speed;
}

void CFuncTrain::Activate( void *funcArgs )
{
	entvars_t *pTarget = VARS( FIND_ENTITY_BY_TARGETNAME( NULL, STRING( pev->target ) ) );
	pev->target = pTarget->target;

	UTIL_SetOrigin( pev, pTarget->origin - ( pev->mins + pev->maxs ) * 0.5 );

	if ( !FStringNull( pev->target ) )
	{
		pev->nextthink = pev->ltime + 0.1f;
		SetThink( &CFuncTrain::Next );
	}
}

void CFuncTrain::Spawn()
{
	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( FStringNull( pev->target ) )
		ALERT( at_error, "FuncTrain with no target" );

	if ( pev->dmg == 0 )
		pev->dmg = 2;

	PRECACHE_SOUND( "plats/train2.wav" );
	PRECACHE_SOUND( "plats/train1.wav" );

	pev->noise = ALLOC_STRING( "plats/train2.wav" );
	pev->noise1 = ALLOC_STRING( "plats/train1.wav" );

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	pev->classname = ALLOC_STRING( "train" );
	SET_MODEL( edict(), STRING( pev->model ) );
	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );

	pev->nextthink = pev->ltime + 0.1f;
	SetThink( &CFuncTrain::Activate );
}

