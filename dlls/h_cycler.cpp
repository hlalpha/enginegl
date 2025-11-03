// h_cycler.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"

class CCycler : public CBaseMonster
{
public:
	void GenericCyclerSpawn( char *szModel, Vector vecMin, Vector vecMax );
	virtual void Think( void *funcArgs );
	virtual void Use( void *funcArgs );

	virtual void Pain( float flDamage );

private:
	double m_flNextSpinTime; // FIXME(SanyaSho): This is not a double!
};

void CCycler::GenericCyclerSpawn( char *szModel, Vector vecMin, Vector vecMax )
{
	PRECACHE_MODEL( szModel );
	SET_MODEL( ENT( pev ), szModel );

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_AIM;
	pev->effects = 0;
	pev->health = 80000;
	pev->yaw_speed = 5;

	UTIL_SetSize( pev, vecMin, vecMax );

	m_flGroundSpeed = 0.f;
	m_flFrameRate = 1.f;

	m_flNextSpinTime = 0.f;

	pev->classname = ALLOC_STRING( "cycler" );
	pev->sequence = 0;
	pev->frame = 0;

	pev->nextthink += 1;

	ResetSequenceInfo( 0.1 );

	SetUse( &CCycler::SUB_CallUseToggle );
}

void CCycler::Think( void *funcArgs )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if ( gpGlobals->time < m_flNextSpinTime )
		pev->angles[1] += 4.f;

	StudioFrameAdvance( 0.1 );
}

void CCycler::Use( void *funcArgs )
{
	m_flNextSpinTime = gpGlobals->time + 4.5f;
}

void CCycler::Pain( float flDamage )
{
	pev->health += flDamage;

	pev->sequence++; // set the next sequence

	ResetSequenceInfo( 0.1 );

	if ( m_flFrameRate == 0 )
	{
		pev->sequence = 0;
		ResetSequenceInfo( 0.1 );
	}

	pev->frame = 0.0;
}

class CCyclerScientist : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/scientist.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_scientist, CCyclerScientist );

class CCyclerHeadcrab : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/headcrab.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_headcrab, CCyclerHeadcrab );

class CCyclerPanther : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/panther.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_panther, CCyclerPanther );

class CCyclerHoundeye : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/houndeye.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_houndeye, CCyclerHoundeye );

class CCyclerSecure : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/barney.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_secure, CCyclerSecure );

class CCyclerBullchicken : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/bullchik.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_bullchicken, CCyclerBullchicken );

class CCyclerDoctor : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/doctor.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_doctor, CCyclerDoctor );

class CCyclerReddoc : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/reddoc.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_reddoc, CCyclerReddoc );

class CCyclerGreendoc : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/greendoc.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_greendoc, CCyclerGreendoc );

class CCyclerBluedoc : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/bluedoc.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_bluedoc, CCyclerBluedoc );

class CCyclerTurret : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/turret.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};

class CCyclerHumanAssault : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/hassault.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_human_assault, CCyclerHumanAssault );

class CCyclerHumanGrunt : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/hgrunt.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_human_grunt, CCyclerHumanGrunt );

class CCyclerHumanDesert : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/desert.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_desert, CCyclerHumanDesert );

class CCyclerOlive : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/olive.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_olive, CCyclerOlive );

class CCyclerAlienGrunt : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/agrunt.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_alien_grunt, CCyclerAlienGrunt );

class CCyclerAlienSlave : public CCycler
{
public:
	virtual void Spawn()
	{
		GenericCyclerSpawn( "models/islave.mdl", Vector( -16, -16, 0 ), Vector( 16, 16, 64 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_alien_slave, CCyclerAlienSlave );

class CCyclerPrdroid : public CCycler
{
public:
	virtual void Spawn()
	{
		pev->origin.z += 16; // HACK

		GenericCyclerSpawn( "models/prdroid.mdl", Vector( -16, -16, -16 ), Vector( 16, 16, 16 ) );
	}
};
LINK_ENTITY_TO_CLASS( cycler_prdroid, CCyclerPrdroid );

LINK_ENTITY_TO_CLASS( cycler_turret, CCyclerTurret ); // ??

