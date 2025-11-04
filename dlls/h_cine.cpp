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
class CCine2Scientist : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine2-scientist.mdl" );
	}
};
class CCinePanther : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine-panther.mdl" );
	}
};

class CCineBarney : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine-barney.mdl" );
	}
};

class CCine2HeavyWeapons : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine2_hvyweapons.mdl" );
	}
};

class CCine2Slave : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine2_slave.mdl" );
	}
};

class CCine3Scientist : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine3-scientist.mdl" );
	}
};

class CCine3Barney : public CCineMonster
{
public:
	void Spawn()
	{
		CineSpawn( "models/cine3-barney.mdl" );
	}
};

//
// ********** Scientist SPAWN **********
//

LINK_ENTITY_TO_CLASS( monster_cine_scientist, CCineScientist );
LINK_ENTITY_TO_CLASS( monster_cine_panther, CCinePanther );
LINK_ENTITY_TO_CLASS( monster_cine_barney, CCineBarney );
LINK_ENTITY_TO_CLASS( monster_cine2_scientist, CCine2Scientist );
LINK_ENTITY_TO_CLASS( monster_cine2_hvyweapons, CCine2HeavyWeapons );
LINK_ENTITY_TO_CLASS( monster_cine2_slave, CCine2Slave );
LINK_ENTITY_TO_CLASS( monster_cine3_scientist, CCine3Scientist );
LINK_ENTITY_TO_CLASS( monster_cine3_barney, CCine3Barney );

//
// ********** Scientist SPAWN **********
//

void CCineMonster ::CineSpawn( char *szModel )
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

//
// CineStart
//
void CCineMonster ::Use( void *funcArgs )
{
	pev->animtime = 0; // reset the sequence
	SetThink( &CCineMonster::CineThink );
	pev->nextthink = gpGlobals->time;
}

//
// ********** Scientist DIE **********
//
void CCineMonster ::Die()
{
	SetThink( &CCineMonster::SUB_Remove );
}

void CCineMonster ::CineThink( void *funcArgs )
{
	// DBG_CheckMonsterData(pev);

	// Emit particles from origin (double check animator's placement of model)
	// THIS is a test feature
	//UTIL_ParticleEffect(pev->origin, g_vecZero, 255, 20);

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
	void Spawn();
	void BloodStart( void *funcArgs );
	void BloodGush( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( cine_blood, CCineBlood );

void CCineBlood ::BloodGush( void *funcArgs )
{
	Vector vecSplatDir;
	TraceResult tr;
	pev->nextthink = gpGlobals->time + 0.1;

#if 0
	UTIL_MakeVectors( pev->angles );
	if ( pev->health-- < 0 )
		REMOVE_ENTITY( edict() );
	// CHANGE_METHOD ( ENT(pev), em_think, SUB_Remove );

	if ( RANDOM_FLOAT( 0, 1 ) < 0.7 ) // larger chance of globs
	{
		UTIL_BloodDrips( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_RED, 10 );
	}
	else // slim chance of geyser
	{
		UTIL_BloodStream( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_RED, RANDOM_LONG( 50, 150 ) );
	}

	if ( RANDOM_FLOAT( 0, 1 ) < 0.75 )
	{ // decals the floor with blood.
		vecSplatDir = Vector( 0, 0, -1 );
		vecSplatDir = vecSplatDir + ( RANDOM_FLOAT( -1, 1 ) * 0.6 * gpGlobals->v_right ) + ( RANDOM_FLOAT( -1, 1 ) * 0.6 * gpGlobals->v_forward ); // randomize a bit
		UTIL_TraceLine( pev->origin + Vector( 0, 0, 64 ), pev->origin + vecSplatDir * 256, ignore_monsters, edict(), &tr );
		if ( tr.flFraction != 1.0 )
		{
			// Decal with a bloodsplat
			UTIL_BloodDecalTrace( &tr, BLOOD_COLOR_RED );
		}
	}
#endif
}

void CCineBlood ::BloodStart( void *funcArgs )
{
	SetThink( &CCineBlood::BloodGush );
	pev->nextthink = gpGlobals->time; // now!
}

void CCineBlood ::Spawn()
{
	pev->solid = SOLID_NOT;
	SetUse( &CCineBlood::BloodStart );
	pev->health = 20; //hacked health to count iterations
}