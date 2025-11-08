// doors.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "doors.h"


extern void SetMovedir( entvars_t *pev );
extern int g_eoActivator;


//
// func_door/func_water
//
class CBaseDoor : public CBaseToggle
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	void SetWaterColor( void *funcArgs );
	void Precache();
	void DoorTouch( void *funcArgs );
	virtual void Use( void *funcArgs );
	void DoorActivate();
	void DoorGoUp( void *funcArgs );
	void DoorHitTop( void *funcArgs );
	void DoorGoDown( void *funcArgs );
	void DoorHitBottom( void *funcArgs );
	virtual void Blocked( void *funcArgs );

private:
	byte m_bHealthValue;
	byte m_bMoveSnd;
	byte m_bStopSnd;
};

void CBaseDoor::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "lip" ) )
	{
		m_flLip = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "skin" ) )
	{
		pev->skin = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "distance" ) )
	{
		m_flMoveDistance = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "movesnd" ) )
	{
		m_bMoveSnd = (byte)atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "stopsnd" ) )
	{
		m_bStopSnd = (byte)atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "healthvalue" ) )
	{
		m_bHealthValue = (byte)atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
}

LINK_ENTITY_TO_CLASS( func_door, CBaseDoor );
LINK_ENTITY_TO_CLASS( func_water, CBaseDoor );

void CBaseDoor::Spawn()
{
	Precache();
	SetMovedir( pev );

	if ( pev->skin != 0 ) // func_water
	{
		pev->solid = SOLID_NOT;

		SetThink( &CBaseDoor::SetWaterColor );
		pev->nextthink = globals->time + 1.f;
	}
	else
	{
		if ( !FBitSet( pev->spawnflags, 0x08 ) )
			pev->solid = SOLID_BSP;
		else
			pev->solid = SOLID_NOT;

		pev->movetype = MOVETYPE_PUSH;
	}

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( edict(), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( pev->dmg == 0 )
		pev->dmg = 2;

	m_vecPosition1 = pev->origin;

	float flSpeed = fabs( pev->movedir.x * pev->size.x + pev->movedir.y * pev->size.y + pev->movedir.z * pev->size.z ) - m_flLip;
	m_vecPosition2 = m_vecPosition1 + ( pev->movedir * flSpeed );

	if ( FBitSet( pev->spawnflags, 0x01 ) )
	{
		UTIL_SetOrigin( pev, m_vecPosition2 );

		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = pev->origin;
	}

	m_toggle_state = TS_AT_BOTTOM;

	if ( FBitSet( pev->spawnflags, 0x100 ) )
		SetTouch( &CBaseDoor::SUB_DoNothing );
	else
		SetTouch( &CBaseDoor::DoorTouch );
}

void CBaseDoor::SetWaterColor( void *funcArgs )
{
	WRITE_BYTE( MSG_BROADCAST, 23 ); // svc_temp_entity
	WRITE_BYTE( MSG_BROADCAST, TE_WATERCOLOR );
	WRITE_BYTE( MSG_BROADCAST, pev->rendercolor.x );
	WRITE_BYTE( MSG_BROADCAST, pev->rendercolor.y );
	WRITE_BYTE( MSG_BROADCAST, pev->rendercolor.z );
	WRITE_BYTE( MSG_BROADCAST, 1 ); // pev->renderamt

	SetThink( NULL );
}

void CBaseDoor::Precache()
{
	switch ( (int)m_bMoveSnd )
	{
	case 1:
		PRECACHE_SOUND( "doors/doormove1.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove1.wav" );
		break;
	case 2:
		PRECACHE_SOUND( "doors/doormove2.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove2.wav" );
		break;
	case 3:
		PRECACHE_SOUND( "doors/doormove3.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove3.wav" );
		break;
	case 4:
		PRECACHE_SOUND( "doors/doormove4.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove4.wav" );
		break;
	case 5:
		PRECACHE_SOUND( "doors/doormove5.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove5.wav" );
		break;
	case 6:
		PRECACHE_SOUND( "doors/doormove6.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove6.wav" );
		break;
	case 7:
		PRECACHE_SOUND( "doors/doormove7.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove7.wav" );
		break;
	case 8:
		PRECACHE_SOUND( "doors/doormove8.wav" );
		pev->noise1 = ALLOC_STRING( "doors/doormove8.wav" );
		break;
	default:
		pev->noise1 = ALLOC_STRING( "common/null.wav" );
		break;
	};

	switch ( (int)m_bStopSnd )
	{
	case 1:
		PRECACHE_SOUND( "doors/doorstop1.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop1.wav" );
		break;
	case 2:
		PRECACHE_SOUND( "doors/doorstop2.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop2.wav" );
		break;
	case 3:
		PRECACHE_SOUND( "doors/doorstop3.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop3.wav" );
		break;
	case 4:
		PRECACHE_SOUND( "doors/doorstop4.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop4.wav" );
		break;
	case 5:
		PRECACHE_SOUND( "doors/doorstop5.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop5.wav" );
		break;
	case 6:
		PRECACHE_SOUND( "doors/doorstop6.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop6.wav" );
		break;
	case 7:
		PRECACHE_SOUND( "doors/doorstop7.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop7.wav" );
		break;
	case 8:
		PRECACHE_SOUND( "doors/doorstop8.wav" );
		pev->noise2 = ALLOC_STRING( "doors/doorstop8.wav" );
		break;
	default:
		pev->noise2 = ALLOC_STRING( "common/null.wav" );
		break;
	};
}

void CBaseDoor::DoorTouch( void *funcArgs )
{
	// Must be a player!
	if ( !FClassnameIs( ENT( gpGlobals->other ), "player" ) )
		return;

	if ( pev->targetname )
		return;

	SetTouch( NULL );
	m_nActivator = gpGlobals->other;
	DoorActivate();
}

void CBaseDoor::Use( void *funcArgs )
{
	if ( m_toggle_state == TS_AT_BOTTOM || ( FBitSet( pev->spawnflags, 0x200 ) && m_toggle_state == TS_AT_TOP ) )
		DoorActivate();
}

void CBaseDoor::DoorActivate()
{
	if ( !FBitSet( pev->spawnflags, 0x20 ) || m_toggle_state != TS_AT_TOP )
	{
		// Healing stations
		entvars_t *pActivator = VARS( m_nActivator );
		if ( FClassnameIs( pActivator, "player" ) )
		{
			pActivator->health += m_bHealthValue;
		}

		DoorGoUp( NULL );
	}
	else
	{
		DoorGoDown( NULL );
	}
}

void CBaseDoor::DoorGoUp( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise1 ), 1.f, 0.8f );

	m_toggle_state = TS_GOING_UP;
	SetMoveDone( &CBaseDoor::DoorHitTop );

	if ( FClassnameIs( edict(), "func_door" ) )
	{
		LinearMove( m_vecPosition2, pev->speed );
	}
	else if ( FClassnameIs( edict(), "func_door_rotating" ) )
	{
		float flDirection = 1.f;
		entvars_t *pActivator = VARS( gpGlobals->other );

		if ( !FClassnameIs( pActivator, "player" ) )
		{
			pActivator = VARS( g_eoActivator );
		}

		if ( !FBitSet( pev->spawnflags, 0x10 ) && FClassnameIs( pActivator, "player" ) )
		{
			if ( pev->movedir.y != 0 )
			{
				Vector vecDelta = pActivator->origin - pev->origin;

				Vector vecDestAngle = pActivator->angles;
				vecDestAngle.x = 0;
				vecDestAngle.z = 0;
				UTIL_MakeVectors( vecDestAngle );

				UTIL_MakeVectors( pActivator->angles );

				Vector vnext = ( pActivator->origin + ( pActivator->velocity * 10 ) ) - pev->origin;
				if ( ( vecDelta.x * vnext.y - vecDelta.y * vnext.x ) < 0 )
					flDirection = -1.0;
			}
		}

		AngularMove( m_vecPosition2 * flDirection, pev->speed );
	}

	SUB_UseTargets( pev );
}

void CBaseDoor::DoorHitTop( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise2 ), 1.f, 0.8f );

	m_toggle_state = TS_AT_TOP;

	if ( FBitSet( pev->spawnflags, 0x20 ) )
	{
		if ( !FBitSet( pev->spawnflags, 0x100 ) )
		{
			SetTouch( &CBaseDoor::DoorTouch );
		}
	}
	else
	{
		pev->nextthink = pev->ltime + m_flWait;
		SetThink( &CBaseDoor::DoorGoDown );

		if ( m_flWait == -1 ) // -4?
		{
			pev->nextthink = -1;
		}
	}
}

void CBaseDoor::DoorGoDown( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise1 ), 1.f, 0.8f );

	m_toggle_state = TS_GOING_DOWN;
	SetMoveDone( &CBaseDoor::DoorHitBottom );

	if ( FClassnameIs( edict(), "func_door" ) )
	{
		LinearMove( m_vecPosition1, pev->speed );
	}
	else if ( FClassnameIs( edict(), "func_door_rotating" ) )
	{
		AngularMove( m_vecPosition1, pev->speed );
	}
}

void CBaseDoor::DoorHitBottom( void *funcArgs )
{
	EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise2 ), 1.f, 0.8f );

	m_toggle_state = TS_AT_BOTTOM;

	if ( FBitSet( pev->spawnflags, 0x100 ) )
		SetThink( &CBaseDoor::SUB_DoNothing );
	else
		SetThink( &CBaseDoor::DoorTouch );
}

void CBaseDoor::Blocked( void *funcArgs )
{
	// SDKTODO(SanyaSho)
}


//
// func_door_rotating
//
class CRotDoor : public CBaseDoor
{
public:
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( func_door_rotating, CRotDoor );

void CRotDoor::Spawn()
{
	Precache();
	CBaseToggle::AxisDir( pev );

	if ( FBitSet( pev->spawnflags, 0x02 ) )
		pev->movedir = pev->movedir * -1;

	m_flWait = 2.f;

	m_vecPosition1 = pev->angles;
	m_vecPosition2 = pev->angles + ( pev->movedir * m_flMoveDistance );

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( edict(), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( pev->dmg == 0 )
		pev->dmg = 2;

	if ( FBitSet( pev->spawnflags, 0x01 ) )
	{
		pev->angles = m_vecPosition2;
		Vector vecSav = m_vecPosition1;
		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = vecSav;

		pev->movedir = pev->movedir * -1;
	}

	m_toggle_state = TS_AT_BOTTOM;

	if ( FBitSet( pev->spawnflags, 0x100 ) )
		SetTouch( &CRotDoor::SUB_DoNothing );
	else
		SetTouch( &CRotDoor::DoorTouch );
}


//
// momentary_door
//
class CMomentaryDoor : public CBaseToggle
{
public:
	virtual void Spawn();
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Use( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( momentary_door, CMomentaryDoor );

void CMomentaryDoor::Spawn()
{
	SetMovedir( pev );

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( edict(), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( pev->dmg == 0 )
		pev->dmg = 2;

	m_vecPosition1 = pev->origin;

	float flSpeed = fabs( pev->movedir.x * pev->size.x + pev->movedir.y * pev->size.y + pev->movedir.z * pev->size.z );
	m_vecPosition2 = m_vecPosition1 + ( pev->movedir * flSpeed );

	SetMoveDone( &CMomentaryDoor::SUB_DoNothing ); // TODO(SanyaSho): Original DLL is calling nullsub_3!

	if ( FBitSet( pev->spawnflags, 0x01 ) )
	{
		UTIL_SetOrigin( pev, m_vecPosition2 );

		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = pev->origin;
	}
}

void CMomentaryDoor::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "lip" ) )
	{
		m_flLip = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "distance" ) )
	{
		m_flMoveDistance = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "movesnd" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "stopsnd" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "healthvalue" ) )
	{
		pkvd->fHandled = 1;
	}
}

void CMomentaryDoor::Use( void *funcArgs )
{
	// /shrug
	if ( !funcArgs )
		return;

	float flValue = *(float *)funcArgs;

	if ( flValue > 1 )
		flValue = 1;

	Vector vecMove = m_vecPosition1 + ( flValue * ( m_vecPosition2 - m_vecPosition1 ) );

	LinearMove( vecMove, pev->speed );
}

