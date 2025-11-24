// buttons.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"


extern void SetMovedir( entvars_t *pev );


//
// multisource
//
LINK_ENTITY_TO_CLASS( multisource, CMultiSource );

void CMultiSource::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "style" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "height" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "killtarget" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "value1" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "value2" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "value3" ) )
	{
		pkvd->fHandled = 1;
	}
}

void CMultiSource::Spawn()
{
	pev->nextthink = pev->ltime + 1.f;
	SetThink( &CMultiSource::Register );

	SetUse( &CMultiSource::SUB_CallUseToggle );
}

void CMultiSource::Use( void *funcArgs )
{
	if ( m_iUnknown <= 0 )
		return;

	m_iUnknown--;

	if ( m_iUnknown != 0 )
		return;

	if ( !FStringNull( pev->target ) )
	{
		int oldSelf = globals->self;
		int oldOther = globals->other;

		edict_t *pentTarget = NULL;

		for ( ;; )
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->target ) );
			if ( FNullEnt( pentTarget ) )
				break;

			globals->self = OFFSET( pentTarget );
			globals->other = oldSelf;

			CPointEntity *pTarget = (CPointEntity *)CPointEntity::Instance( pentTarget );
			pTarget->Use( NULL );

			globals->self = oldSelf;
			globals->other = oldOther;
		}
	}
}

void CMultiSource::sub_10015810( void *funcArgs )
{
	if ( m_iTotal > m_iUnknown )
		m_iUnknown++;
}

void CMultiSource::Register( void *funcArgs )
{
	m_iTotal = 0;
	m_iUnknown = 0;

	SetThink( &CMultiSource::SUB_DoNothing );

	edict_t *pentTarget = FIND_ENTITY_BY_TARGET( NULL, STRING( pev->targetname ) );
	while ( pentTarget )
	{
		if ( FNullEnt( pentTarget ) )
			break;

		if ( m_iTotal >= 10 )
			break;

		m_iTotal++;

		pentTarget = ENT( VARS( pentTarget )->chain );
	}

	m_iUnknown = m_iTotal;
}


//
// func_button
//
void CBaseButton::KeyValue( KeyValueData *pkvd )
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
	else if ( FStrEq( pkvd->szKeyName, "delay" ) )
	{
		m_flDelay = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "distance" ) )
	{
		m_flMoveDistance = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "master" ) )
	{
		m_iszMaster = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = 1;
	}
}

void CBaseButton::Pain( float flDamage )
{
	pev->health = 9999;

	if ( m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN )
		return;

	m_eoActivator = gpGlobals->other;

	if ( m_toggle_state == TS_AT_TOP )
	{
		if ( /*FBitSet( pev->spawnflags, 0x20 ) &&*/ !m_fStayPushed )
		{
			EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise ), 1.f, 0.8f );

			SUB_UseTargets( pev );
			ButtonReturn( NULL );
		}
	}
	else
	{
		ButtonActivate();
	}
}

LINK_ENTITY_TO_CLASS( func_button, CBaseButton );

void CBaseButton::Spawn()
{
	char *szSound = ButtonSound( (int)pev->sounds );
	PRECACHE_SOUND( szSound );
	pev->noise = ALLOC_STRING( szSound );

	if ( FBitSet( pev->spawnflags, 0x40 ) )
	{
		PRECACHE_SOUND( "buttons/spark1.wav" );
		PRECACHE_SOUND( "buttons/spark2.wav" );
		PRECACHE_SOUND( "buttons/spark3.wav" );
		PRECACHE_SOUND( "buttons/spark4.wav" );
		PRECACHE_SOUND( "buttons/spark5.wav" );
		PRECACHE_SOUND( "buttons/spark6.wav" );

		SetThink( &CBaseButton::ButtonSpark );
		pev->nextthink = gpGlobals->time + 0.5f;
	}

	SetMovedir( pev );

	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BSP;

	SET_MODEL( edict(), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 40;

	if ( pev->health > 0 )
		pev->takedamage = DAMAGE_YES;

	if ( m_flWait == 0 )
		m_flWait = 1;

	if ( m_flLip == 0 )
		m_flLip = 4;

	m_toggle_state = TS_AT_BOTTOM;

	m_vecPosition1 = pev->origin;

	float flSpeed = fabs( pev->movedir.x * pev->size.x + pev->movedir.y * pev->size.y + pev->movedir.z * pev->size.z ) - m_flLip;
	m_vecPosition2 = m_vecPosition1 + ( pev->movedir * flSpeed );

	m_fRotating = FALSE;
	m_fStayPushed = ( m_flWait == -1 );

	if ( FBitSet( pev->spawnflags, 0x100 ) )
	{
		SetTouch( &CBaseButton::ButtonTouch );
	}
	else
	{
		SetTouch( &CBaseButton::SUB_DoNothing );
		SetTouch( &CBaseButton::ButtonUse );
	}
}

char *ButtonSound( int sound )
{
	switch ( sound )
	{
	case 0:
		return "common/null.wav";
	case 1: // Big zap & Warmup
		return "buttons/button1.wav";
	case 2: // Access Denied
		return "buttons/button2.wav";
	case 3: // Access Granted
		return "buttons/button3.wav";
	case 4: // Quick Combolock
		return "buttons/button4.wav";
	case 5: // Power Deadbolt 1
		return "buttons/button5.wav";
	case 6: // Power Deadbolt 2
		return "buttons/button6.wav";
	case 7: // Plunger
		return "buttons/button7.wav";
	case 8: // Small zap
		return "buttons/button8.wav";
	case 9: // Keycard Sound
		return "buttons/button9.wav";
	case 10: // Buzz In
		return "buttons/button10.wav";
	case 11: // Buzz Off
		return "buttons/button11.wav";
	default:
		return "buttons/button9.wav";
	}

	return NULL;
}

void CBaseButton::ButtonSpark( void *funcArgs )
{
	SetThink( &CBaseButton::ButtonSpark );
	pev->nextthink = gpGlobals->time + 0.1f + UTIL_RandomFloat( 0.f, 1.5f );

	Vector vecSparkPos = pev->mins + pev->size * 0.5f;

	WRITE_BYTE( MSG_BROADCAST, 23 ); // svc_temp_entity
	WRITE_BYTE( MSG_BROADCAST, TE_SPARKS );
	WRITE_COORD( MSG_BROADCAST, vecSparkPos.x );
	WRITE_COORD( MSG_BROADCAST, vecSparkPos.y );
	WRITE_COORD( MSG_BROADCAST, vecSparkPos.z );

	float flVolume = UTIL_RandomFloat( 0.25f, 0.75f );

	switch ( (int)UTIL_RandomFloat( 0.f, 1.f ) * 6 )
	{
	case 0:
		EMIT_SOUND_DYN2( edict(), 2, "buttons/spark1.wav", flVolume, 0.8f );
		break;
	case 1:
		EMIT_SOUND_DYN2( edict(), 2, "buttons/spark2.wav", flVolume, 0.8f );
		break;
	case 2:
		EMIT_SOUND_DYN2( edict(), 2, "buttons/spark3.wav", flVolume, 0.8f );
		break;
	case 3:
		EMIT_SOUND_DYN2( edict(), 2, "buttons/spark4.wav", flVolume, 0.8f );
		break;
	case 4:
		EMIT_SOUND_DYN2( edict(), 2, "buttons/spark5.wav", flVolume, 0.8f );
		break;
	case 5:
		EMIT_SOUND_DYN2( edict(), 2, "buttons/spark6.wav", flVolume, 0.8f );
		break;
	}
}

void CBaseButton::ButtonUse( void *funcArgs )
{
	ALERT( at_log, "%d\n", m_fStayPushed );

	if ( m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN )
		return;

	m_eoActivator = gpGlobals->other;

	if ( m_toggle_state == TS_AT_TOP )
	{
		if ( !m_fStayPushed && FBitSet( pev->spawnflags, 0x20 ) )
		{
			EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise ), 1.f, 0.8f );

			SUB_UseTargets( pev );
			ButtonReturn( NULL );
		}
	}
	else
	{
		ButtonActivate();
	}
}

void CBaseButton::ButtonTouch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( gpGlobals->other );

	// Must be a player!
	if ( !FClassnameIs( pevToucher, "player" ) )
		return;

	if ( m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN )
		return;

	if ( !FStringNull( m_iszMaster ) )
	{
		edict_t *pentTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING( m_iszMaster ) );
		if ( !FNullEnt( pentTarget ) && FClassnameIs( pentTarget, "multisource" ) )
		{
			CMultiSource *pMultiSource = (CMultiSource *)CMultiSource::Instance( pentTarget );
			if ( pMultiSource && pMultiSource->m_iUnknown )
				return;
		}
	}

	SetTouch( NULL );

	m_eoActivator = gpGlobals->other;

	if ( m_toggle_state == TS_AT_TOP )
	{
		if ( FBitSet( pev->spawnflags, 0x20 ) && !m_fStayPushed )
		{
			EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise ), 1.f, 0.8f );

			SUB_UseTargets( pev );
			ButtonReturn( NULL );
		}
	}
	else
	{
		ButtonActivate();
	}
}

void CBaseButton::ButtonActivate()
{
	EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise ), 1.f, 0.8f );

	m_toggle_state = TS_GOING_UP;

	SetMoveDone( &CBaseButton::TriggerAndWait );

	if ( !m_fRotating )
	{
		LinearMove( m_vecPosition2, pev->speed );
	}
	else
	{
		AngularMove( m_vecPosition2, pev->speed );
	}
}

void CBaseButton::TriggerAndWait( void *funcArgs )
{
	m_toggle_state = TS_AT_TOP;

	if ( FBitSet( pev->spawnflags, 0x20 ) || m_fStayPushed )
	{
		if ( FBitSet( pev->spawnflags, 0x100 ) )
			SetTouch( &CBaseButton::ButtonTouch );
		else
			SetTouch( &CBaseButton::SUB_DoNothing );
	}
	else
	{
		pev->nextthink = pev->ltime + m_flWait;
		SetThink( &CBaseButton::ButtonReturn );
	}

	pev->frame = 1.f;

	SUB_UseTargets( NULL );
}

void CBaseButton::ButtonReturn( void *funcArgs )
{
	m_toggle_state = TS_GOING_DOWN;

	SetMoveDone( &CBaseButton::ButtonBackHome );

	if ( !m_fRotating )
	{
		LinearMove( m_vecPosition1, pev->speed );
	}
	else
	{
		AngularMove( m_vecPosition1, pev->speed );
	}

	pev->frame = 0.f;
}

void CBaseButton::ButtonBackHome( void *funcArgs )
{
	m_toggle_state = TS_AT_BOTTOM;

	if ( !FStringNull( pev->target ) )
	{
		edict_t *pentTarget = NULL;
		for ( ;; )
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->target ) );
			if ( FNullEnt( pentTarget ) )
				break;

			if ( !FClassnameIs( pentTarget, "multisource" ) )
				break;

			CMultiSource *pMultiSource = (CMultiSource *)CMultiSource::Instance( pentTarget );
			if ( pMultiSource )
			{
				pMultiSource->sub_10015810( NULL );
			}
		}
	}

	if ( FBitSet( pev->spawnflags, 0x100 ) )
		SetTouch( &CBaseButton::ButtonTouch );
	else
		SetTouch( &CBaseEntity::SUB_DoNothing );

	if ( FBitSet( pev->spawnflags, 0x40 ) )
	{
		SetThink( &CBaseButton::ButtonSpark );
		pev->nextthink = gpGlobals->time + 0.5f;
	}
}


//
// func_rot_button
//
class CRotButton : public CBaseButton
{
public:
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( func_rot_button, CRotButton );

void CRotButton::Spawn()
{
	char *szSound = ButtonSound( (int)pev->sounds );
	PRECACHE_SOUND( szSound );
	pev->noise = ALLOC_STRING( szSound );

	CBaseToggle::AxisDir( pev );

	if ( FBitSet( pev->spawnflags, 0x02 ) )
		pev->movedir = pev->movedir * -1;

	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BSP;

	SET_MODEL( edict(), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 40;

	if ( m_flWait == 0 )
		m_flWait = 1;

	m_toggle_state = TS_AT_BOTTOM;

	m_vecPosition1 = pev->angles;
	m_vecPosition2 = pev->angles + ( pev->movedir * m_flMoveDistance );

	m_fRotating = TRUE;
	m_fStayPushed = ( m_flWait == -1 );

	if ( FBitSet( pev->spawnflags, 0x100 ) )
	{
		SetTouch( &CRotButton::ButtonTouch );
	}
	else
	{
		SetTouch( &CRotButton::SUB_DoNothing );
		SetTouch( &CRotButton::ButtonUse );
	}
}


//
// momentary_rot_button
//
class CMomentaryRotButton : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void KeyValue( KeyValueData *pkvd );
	void PlaySound();
	virtual void Use( void *funcArgs );
	void UpdateTarget( void *funcArgs );
	void Off( void *funcArgs );
	void Return( void *funcArgs );

private:
	int m_lastUsed;
	int m_direction;
	float m_moveDistance;
	float m_returnSpeed;
	vec3_t m_start;
	vec3_t m_end;
};

LINK_ENTITY_TO_CLASS( momentary_rot_button, CMomentaryRotButton );

void CMomentaryRotButton::Spawn()
{
	CBaseToggle::AxisDir( pev );

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( m_moveDistance <= 0 )
	{
		m_start = pev->angles;
		m_end = pev->angles + ( pev->movedir * m_moveDistance );

		m_direction = -1;
	}
	else
	{
		m_start = pev->angles + ( pev->movedir * m_moveDistance );
		m_end = pev->angles;

		m_moveDistance = -m_moveDistance;
		m_direction = 1;
	}

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( edict(), STRING( pev->model ) );

	char *szSound = ButtonSound( (int)pev->sounds );
	PRECACHE_SOUND( szSound );
	pev->noise = ALLOC_STRING( szSound );

	m_lastUsed = 0;
}

void CMomentaryRotButton::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "lip" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "delay" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "distance" ) )
	{
		m_moveDistance = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "master" ) )
	{
		pkvd->fHandled = 1;
	}
	else if ( FStrEq( pkvd->szKeyName, "returnspeed" ) )
	{
		m_returnSpeed = atof( pkvd->szValue );
		pkvd->fHandled = 1;
	}
}

void CMomentaryRotButton::PlaySound()
{
	if ( m_lastUsed )
		return;

	EMIT_SOUND_DYN2( edict(), 2, STRING( pev->noise ), 1.f, 0.8f );
}

void CMomentaryRotButton::Use( void *funcArgs )
{
	// SDKTODO
}

void CMomentaryRotButton::UpdateTarget( void *funcArgs )
{
	// SDKTODO
}

void CMomentaryRotButton::Off( void *funcArgs )
{
	pev->avelocity = g_vecZero;
	m_lastUsed = 0;
	if ( FBitSet( pev->spawnflags, 0x10 ) && ( m_returnSpeed > 0 ) )
	{
		SetThink( &CMomentaryRotButton::Return );
		pev->nextthink = pev->ltime + 0.1f;
		m_direction = -1;
	}
	else
	{
		SetThink( NULL );
	}
}

void CMomentaryRotButton::Return( void *funcArgs )
{
	// SDKTODO
}