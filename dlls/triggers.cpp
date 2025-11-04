// triggers.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"

//
// func_friction
//
class CFrictionModifier : public CBaseEntity
{
public:
	virtual void Spawn();
	void ChangeFriction( void *funcArgs );
	virtual void KeyValue( KeyValueData *pkvd );

private:
	float m_flFrictionModifier;
};

LINK_ENTITY_TO_CLASS( func_friction, CFrictionModifier );

void CFrictionModifier::Spawn()
{
	pev->solid = SOLID_TRIGGER;

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	pev->movetype = MOVETYPE_NONE;

	SetTouch( &CFrictionModifier::ChangeFriction );
}

void CFrictionModifier::ChangeFriction( void *funcArgs )
{
	edict_t *pOther = ENT( gpGlobals->other );
	entvars_t *pVars = VARS( pOther );

	//pVars->friction = m_flFrictionModifier; // TODO(SanyaSho)
}

void CFrictionModifier::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "modifier" ) )
	{
		m_flFrictionModifier = atof( pkvd->szValue ) * 0.01;
		pkvd->fHandled = TRUE;
	}
}


//
// multi_manager
//
/*class CMultiManager : public CBaseToggle
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();

	void ManagerThink( void *funcArgs );
	void ManagerUse( void *funcArgs );
};*/


#define SPAWNFLAG_NOMESSAGE 1

//
// trigger
//
class CBaseTrigger : public CBaseDelay
{
public:
	void InitTrigger();

	virtual void KeyValue( KeyValueData *pkvd );

	// for trigger_multiple
	void MultiTouch( void *funcArgs );
	void ActivateMultiTrigger();
	void MultiWaitOver( void *funcArgs );

	// for trigger_counter
	void CounterUse( void *funcArgs );

protected:
	int unk3;
	int unk4;
	int unk5;
	float m_flWait;
	int unk7;
	int unk8;
	int unk9;
	int unk10;
	int unk11;
	int unk12;
	int unk13;
	int unk14;
	int unk15;
	int m_cTriggersLeft;
	float m_flHeight;
	int unk18;
	int unk19;
	int unk20;
	int unk21;
	int unk22;
	int unk23;
	int unk24;
	int unk25;
};

LINK_ENTITY_TO_CLASS( trigger, CBaseTrigger );

int g_eoActivator;

void CBaseTrigger::InitTrigger()
{
	// TODO(SanyaSho)
//	if ( pev->angles[0] != 0 || pev->angles[1] != 0 || pev->angles[2] != 0 )
//		SetMovedir( pev );

	pev->solid = SOLID_TRIGGER;

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	pev->movetype = MOVETYPE_NONE;
	pev->modelindex = 0;
	pev->model = 0;
}

void CBaseTrigger::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "damage" ) )
	{
		// TODO(SanyaSho)
		//pev->dmg = atof( pkvd->szValue );
		//pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "count" ) )
	{
		m_cTriggersLeft = (int)atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "delay" ) )
	{
		m_flDelay = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
}

//
// trigger_hurt
//
class CTriggerHurt : public CBaseTrigger
{
public:
	virtual void Spawn();

	void ToggleUse( void *funcArgs );
	void HurtTouch( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( trigger_hurt, CTriggerHurt );

//
// trigger_monsterjump
//
class CTriggerMonsterJump : public CBaseTrigger
{
public:
	virtual void Spawn();

	void ToggleUse( void *funcArgs );
	void TriggerTouch( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( trigger_monsterjump, CTriggerMonsterJump );

void CTriggerMonsterJump::Spawn()
{
	// TODO(SanyaSho)
	//SetMovedir( pev );

	InitTrigger();

	SetUse( &CTriggerMonsterJump::ToggleUse );
	SetTouch( &CTriggerMonsterJump::TriggerTouch );

	// TODO(SanyaSho)
	//pev->speed = 200;

	m_flHeight = 150.f;

	if ( !FStringNull( pev->targetname ) )
	{
		pev->solid = SOLID_NOT;
	}
}

void CTriggerMonsterJump::ToggleUse( void *funcArgs )
{
	if ( pev->solid != SOLID_NOT )
	{
		pev->solid = SOLID_NOT;
	}
	else
	{
		pev->solid = SOLID_TRIGGER;
	}
}

void CTriggerMonsterJump::TriggerTouch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	// only monsters can pass
	if ( !FBitSet( pevToucher->flags, FL_MONSTER ) )
		return;

	pevToucher->origin.z += 1.f; // HACK: Move the entity 1 unit above the ground to prevent stuck

	if ( FBitSet( pevToucher->flags, FL_ONGROUND ) )
		pevToucher->flags -= FL_ONGROUND; // FIXME(SanyaSho)

	// TODO(SanyaSho)
	//pev->velocity = pev->movedir * pev->speed;

	// Make a "jump"
	pev->velocity.z += m_flHeight;

	// Disable the trigger
	pev->solid = SOLID_NOT;
}

//
// trigger_cdaudio
//
class CTriggerCDAudio : public CBaseTrigger
{
public:
	virtual void Spawn();

	void PlayTrack( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( trigger_cdaudio, CTriggerCDAudio );

void CTriggerCDAudio::Spawn()
{
	InitTrigger();

	SetTouch( &CTriggerCDAudio::PlayTrack );
}

void CTriggerCDAudio::PlayTrack( void *funcArgs )
{
	// Not a player, don't play the audio
	if ( !FClassnameIs( ENT( gpGlobals->other ), "player" ) )
		return;

	switch ( (int)pev->health )
	{
	case -1:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd stop\n" );
		break;
	case 1:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 1\n" );
		break;
	case 2:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 2\n" );
		break;
	case 3:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 3\n" );
		break;
	case 4:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 4\n" );
		break;
	case 5:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 5\n" );
		break;
	case 6:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 6\n" );
		break;
	case 7:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 7\n" );
		break;
	case 8:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 8\n" );
		break;
	case 9:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 9\n" );
		break;
	case 10:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 10\n" );
		break;
	case 11:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 11\n" );
		break;
	case 12:
		SERVER_EXECUTE( ENT( gpGlobals->other ), "cd play 12\n" );
		break;
	default:
		ALERT( at_log, "Unknown Track!\n" );
		break;
	};

	SetTouch( &CBaseEntity::SUB_DoNothing );
	SetThink( &CBaseEntity::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CTriggerHurt::Spawn()
{
	InitTrigger();

	SetTouch( &CTriggerHurt::HurtTouch );

	if ( !FStringNull( pev->targetname ) )
		SetUse( &CTriggerHurt::ToggleUse );
	else
		SetUse( &CTriggerHurt::SUB_DoNothing );
}

void CTriggerHurt::ToggleUse( void *funcArgs )
{
	if ( pev->solid != SOLID_NOT )
	{
		pev->solid = SOLID_NOT;
	}
	else
	{
		pev->solid = SOLID_TRIGGER;
	}
}

void CTriggerHurt::HurtTouch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	if ( pevToucher->takedamage != DAMAGE_NO && gpGlobals->time >= m_flDelay )
	{
		CBaseMonster *pMonster = (CBaseMonster *)GET_PRIVATE( ENT( gpGlobals->other ) );
		//pMonster->TakeDamage( pev, pev, pev->dmg ); // TODO(SanyaSho)
		pMonster->TakeDamage( pev, pev, 10.f );

		m_flDelay += 0.5f;
	}
}

#define SPAWNFLAG_NOTOUCH 1

//
// trigger_multiple
//
class CTriggerMultiple : public CBaseTrigger
{
public:
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( trigger_multiple, CTriggerMultiple );

void CTriggerMultiple::Spawn()
{
	// Init the noise sound
	PRECACHE_SOUND( "common/null.wav" );
	pev->noise = ALLOC_STRING( "common/null.wav" );

	if ( m_flWait == 0.f )
		m_flWait = 0.2f;

	InitTrigger();

	if ( !FBitSet( pev->spawnflags, SPAWNFLAG_NOTOUCH ) )
		SetTouch( &CBaseTrigger::MultiTouch );
}

//
// trigger_once
//
class CTriggerOnce : public CTriggerMultiple
{
public:
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( trigger_once, CTriggerOnce );

void CTriggerOnce::Spawn()
{
	m_flWait = -1.f;
	CTriggerMultiple::Spawn();
}

void CBaseTrigger::MultiTouch( void *funcArgs )
{
	edict_t *pevToucher = ENT( globals->other );

	if ( !FClassnameIs( pevToucher, "player" ) )
	{
		return;
	}

	if ( !FClassnameIs( pevToucher, "player" ) && !FClassnameIs( pevToucher, "monster_probedroid" ) )
	{
		return;
	}

	if ( pev->movedir != g_vecZero )
	{
		UTIL_MakeVectors( VARS( pevToucher )->angles );

		if ( DotProduct( gpGlobals->v_forward, pev->movedir ) < 0 )
			return;
	}

	pev->enemy = globals->other; // Don't collide twice with the same player
	ActivateMultiTrigger();
}

void CBaseTrigger::ActivateMultiTrigger()
{
	if ( pev->nextthink > globals->time )
		return;

	// found a secret, yay
	if ( FClassnameIs( pev, "trigger_secret" ) )
	{
		if ( !FClassnameIs( ENT( pev->enemy ), "player" ) )
			return;

		globals->found_secrets++;
		WRITE_BYTE( MSG_ALL, 28 ); // svc_foundsecret
	}

	// make some noise
	if ( !FStringNull( pev->noise ) )
	{
		EMIT_SOUND_DYN2( ENT( pev ), 2, STRING( pev->noise ), 1.f, 0.8f );
	}

	g_eoActivator = pev->enemy;
	SUB_UseTargets( pev );

	if ( m_flWait > 0 )
	{
		SetThink( &CBaseTrigger::MultiWaitOver );
		pev->nextthink = globals->time + m_flWait;
	}
	else
	{
		SetTouch( NULL );

		pev->nextthink = globals->time + 0.1;
		SetThink( &CBaseEntity::SUB_Remove );
	}
}

void CBaseTrigger::MultiWaitOver( void *funcArgs )
{
	SetThink( NULL );
}

void CBaseTrigger::CounterUse( void *funcArgs )
{
	m_cTriggersLeft--;

	if ( m_cTriggersLeft < 0 )
		return;

	BOOL fTellActivator = FStrEq( STRING( VARS( ENT( g_eoActivator ) )->classname ), "player" ) && !FBitSet( pev->spawnflags, SPAWNFLAG_NOMESSAGE );

	if ( m_cTriggersLeft )
	{
		if ( fTellActivator )
			switch ( m_cTriggersLeft )
			{
			case 1:
				ALERT( at_log, "Only 1 more to go..." );
				break;
			case 2:
				ALERT( at_log, "Only 2 more to go..." );
				break;
			case 3:
				ALERT( at_log, "Only 3 more to go..." );
				break;
			default:
				ALERT( at_log, "There are more to go..." );
				break;
			};
	}
	else
	{
		if ( fTellActivator )
			ALERT( at_log, "Sequence completed!" );

		pev->enemy = g_eoActivator;
		ActivateMultiTrigger();
	}
}

//
// trigger_counter
//
class CTriggerCounter : public CBaseTrigger
{
public:
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( trigger_counter, CTriggerCounter );

void CTriggerCounter::Spawn()
{
	m_flWait = -1.f;

	if ( m_cTriggersLeft == 0 )
		m_cTriggersLeft = 2;

	SetUse( &CBaseTrigger::CounterUse );
}

//
// trigger_changelevel
//
class CChangeLevel : public CBaseTrigger
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();

	void TouchChangeLevel( void *funcArgs );

private:
	char m_szMapName[cchMapNameMost];
};

//LINK_ENTITY_TO_CLASS( trigger_changelevel, CChangeLevel );

void CChangeLevel::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "map" ) )
	{
		strcpy( m_szMapName, pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
}

void CChangeLevel::Spawn()
{
	if ( FStrEq( m_szMapName, "" ) )
		ALERT( at_log, "a trigger_changelevel doesn't have a map" );

	InitTrigger();

	SetTouch( &CChangeLevel::TouchChangeLevel );
}

void CChangeLevel::TouchChangeLevel( void *funcArgs )
{
	// SDKTODO(SanyaSho)
}

//
// func_ladder
//
class CLadder : public CBaseTrigger
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Touch( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( func_ladder, CLadder );

void CLadder::KeyValue( KeyValueData *pkvd )
{
	/* Nothing */
}

void CLadder::Spawn()
{
	pev->solid = SOLID_TRIGGER;
	pev->solid = SOLID_BSP;

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	pev->movetype = MOVETYPE_PUSH;
	pev->renderamt = 0;
	pev->rendermode = kRenderModeColor;
}

void CLadder::Touch( void *funcArgs )
{
	// SDKTODO(SanyaSho)
	/*edict_t *pevToucher = ENT( globals->other );
	entvars_t *pevToucherVars = VARS( pevToucher );

	if ( FClassnameIs( pevToucherVars, "player" ) )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE( pevToucher );
		pPlayer->unk10 |= 1;
	}*/
}

//
// trigger_push
//
class CTriggerPush : public CBaseTrigger
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Touch( void *funcArgs );
	virtual void Use( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( trigger_push, CTriggerPush );

void CTriggerPush::KeyValue( KeyValueData *pkvd )
{
	/* Nothing */
}

void CTriggerPush::Spawn()
{
	if ( pev->angles == g_vecZero )
		pev->angles[1] = 360.f;

	InitTrigger();

	// TODO(SanyaSho)
	//if ( pev->speed == 0.f )
	//	pev->speed = 1000.f;

	if ( FBitSet( pev->spawnflags, 2 ) )
		pev->solid = SOLID_NOT;
}

void CTriggerPush::Touch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	if ( pevToucher->health > 0 )
	{
		//pevToucher->velocity = pev->movedir * pev->speed * 10.f; // TODO(SanyaSho)
		pevToucher->velocity = pev->movedir * 10.f * 10.f;
	}

	if ( FBitSet( pev->spawnflags, 1 ) )
		REMOVE_ENTITY( ENT( pev ) );
}

void CTriggerPush::Use( void *funcArgs )
{
	if ( pev->solid != SOLID_NOT )
	{
		pev->solid = SOLID_NOT;
	}
	else
	{
		pev->solid = SOLID_TRIGGER;
	}
}