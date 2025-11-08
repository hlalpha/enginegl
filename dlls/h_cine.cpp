// h_cine.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"

class CCineMonster : public CBaseMonster
{
public:
	void CineSpawn( char *szModel );
	virtual void Use( void *funcArgs );
	void CineThink( void *funcArgs );
	virtual void Die();
};

class CCineScientist : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine-scientist.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine_scientist, CCineScientist );

class CCinePanther : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine-panther.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine_panther, CCinePanther );

class CCineBarney : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine-barney.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine_barney, CCineBarney );

class CCine2Scientist : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine2-scientist.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine2_scientist, CCine2Scientist );

class CCine2HeavyWeapons : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine2_hvyweapons.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine2_hvyweapons, CCine2HeavyWeapons );

class CCine2Slave : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine2_slave.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine2_slave, CCine2Slave );

class CCine3Scientist : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine3-scientist.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine3_scientist, CCine3Scientist );

class CCine3Barney : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine3-barney.mdl" );
	}
};

LINK_ENTITY_TO_CLASS( monster_cine3_barney, CCine3Barney );

void CCineMonster::CineSpawn( char *szModel )
{
	PRECACHE_MODEL( szModel );
	SET_MODEL( edict(), szModel );

	UTIL_SetSize( pev, Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 1;
	pev->yaw_speed = 10;

	// ugly alpha hack, can't set ints from the bsp.
	pev->sequence = (int)pev->impulse;
	ResetSequenceInfo( 0.1 );
	pev->framerate = 0.0;

	//m_bloodColor = BLOOD_COLOR_RED;
	unk57 = 1232348144;
	unk67 = 70;

	// if no targetname, start now
	if ( FStringNull( pev->targetname ) )
	{
		SetThink( &CCineMonster::CineThink );
		pev->nextthink += 1.0;
	}
}

void CCineMonster::Use( void *funcArgs )
{
	pev->animtime = 0; // reset the sequence
	SetThink( &CCineMonster::CineThink );
	pev->nextthink = gpGlobals->time;
}

void CCineMonster::Die()
{
	SetThink( &CCineMonster::SUB_Remove );
}

void CCineMonster ::CineThink( void *funcArgs )
{
	if ( !pev->animtime )
		ResetSequenceInfo( 0.1f );

	pev->nextthink = gpGlobals->time + 1.0;

	if ( pev->spawnflags != 0 && m_fSequenceFinished )
	{
		Die();
		return;
	}

	StudioFrameAdvance( 1.0f );
}


//
// cine_blood
//
class CCineBlood : public CBaseEntity
{
public:
	virtual void Spawn();
	void BloodStart( void *funcArgs );
	void BloodGush( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( cine_blood, CCineBlood );

void CCineBlood::BloodGush( void *funcArgs )
{
	// SDKTODO(SanyaSho)
}

void CCineBlood::BloodStart( void *funcArgs )
{
	SetThink( &CCineBlood::BloodGush );
	pev->nextthink = gpGlobals->time;
}

void CCineBlood::Spawn()
{
	pev->solid = SOLID_NOT;
	SetUse( &CCineBlood::BloodStart );
	pev->health = 20;
}

