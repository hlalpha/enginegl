// sound.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"


// Should * 0.1 be replaced with / 10 ?
#define VOL_AND_ATT( vol, att )                                                  \
	att = FBitSet( pev->spawnflags, SF_AMBIENT_SOUND_EVERYWHERE ) ? 0.f : 2.25f; \
	vol = pev->health * 0.1;


//
// ambient_generic
//
class CAmbientGeneric : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void KeyValue( KeyValueData *pkvd );
	void RampThink( void *funcArgs );
	void ToggleUse( void *funcArgs );

private:
	BOOL m_fActive;
};

LINK_ENTITY_TO_CLASS( ambient_generic, CAmbientGeneric );

void CAmbientGeneric::Spawn()
{
	char *sample = (char *)STRING( pev->message );
	PRECACHE_SOUND( sample );

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;

	// Mapper want this ambient_generic entity to play sound by some external event
	if ( !FStringNull( pev->targetname ) )
	{
		SetUse( &CAmbientGeneric::ToggleUse );

		m_fActive = !FBitSet( pev->spawnflags, SF_AMBIENT_SOUND_START_SILENT );

		SetThink( &CAmbientGeneric::RampThink );
		pev->nextthink = gpGlobals->time + 1.f;
	}
	else
	{
		float volume, attenuation;
		VOL_AND_ATT( volume, attenuation );

		UTIL_EmitAmbientSound( pev->origin, sample, volume, attenuation );
	}
}

void CAmbientGeneric::RampThink( void *funcArgs )
{
	if ( m_fActive )
	{
		float volume, attenuation;
		VOL_AND_ATT( volume, attenuation );

		EMIT_SOUND_DYN2( edict(), 1, STRING( pev->message ), volume, attenuation );
	}

	SetThink( NULL );
}

void CAmbientGeneric::ToggleUse( void *funcArgs )
{
	float volume, attenuation;
	VOL_AND_ATT( volume, attenuation );

	if ( m_fActive )
	{
		m_fActive = FALSE;
		EMIT_SOUND_DYN2( edict(), 1, "common/null.wav", volume, attenuation );
	}
	else
	{
		m_fActive = TRUE;
		EMIT_SOUND_DYN2( edict(), 1, STRING( pev->message ), volume, attenuation );
	}
}

void CAmbientGeneric::KeyValue( KeyValueData *pkvd )
{
}


//
// env_sound
//
class CEnvSound : public CBaseEntity
{
public:
	virtual void KeyValue( KeyValueData* pkvd);
	virtual void Spawn();

	virtual void Think( void *funcArg );

public:
	float m_flRadius;
	float m_flRoomtype;
};

LINK_ENTITY_TO_CLASS( env_sound, CEnvSound );

void CEnvSound::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "radius" ) )
	{
		m_flRadius = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}

	if ( FStrEq( pkvd->szKeyName, "roomtype" ) )
	{
		m_flRoomtype = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
}

// returns TRUE if the given sound entity (pev) is in range 
// and can see the given player entity (pevTarget)
BOOL FEnvSoundInRange( entvars_t *pev, entvars_t *pevTarget, float *pflRange ) 
{
	CEnvSound *pSound = GetClassPtr( (CEnvSound *)pev );

	Vector vecSpot1 = pev->view_ofs + pev->origin;
	Vector vecSpot2 = pevTarget->view_ofs + pevTarget->origin;

	TraceResult tr;
	UTIL_TraceLine( vecSpot1, vecSpot2, FALSE, ENT( pev ), &tr );
	
	// check if line of sight crosses water boundary, or is blocked
	if ( (tr.fInOpen && tr.fInWater) || tr.flFraction != 1 )
		return FALSE;

	// calc range from sound entity to player
	Vector vecRange = tr.vecEndPos - vecSpot1;

	float flRange = DotProduct( vecRange, vecRange );

	if ( pSound->m_flRadius < flRange )		
		return FALSE;
	
	if ( pflRange )
		*pflRange = flRange;

	return TRUE;
}

void CEnvSound::Think( void *funcArg )
{
	// SDKTODO(SanyaSho)
}

void CEnvSound::Spawn()
{
	MARK_STUB_ENTITY;
	pev->nextthink = gpGlobals->time + UTIL_RandomFloat( 0.f, 0.5f );
}

