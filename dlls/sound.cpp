//=========================================================
// sound.cpp 
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"

// Should * 0.1 be replaced with / 10 ?
#define VOL_AND_ATT( vol, att )                                                  \
	att = FBitSet( pev->spawnflags, SF_AMBIENT_SOUND_EVERYWHERE ) ? 0.f : 2.25f; \
	vol = pev->health * 0.1;

// ==================== GENERIC AMBIENT SOUND ======================================
class CAmbientGeneric : public CBaseEntity
{
public:
	void Spawn();
	void KeyValue( KeyValueData *pkvd );
	void ToggleUse( void *funcArgs );
	void RampThink( void *funcArgs );

private:
	BOOL m_fActive; // only TRUE when the entity is playing a looping sound
};
LINK_ENTITY_TO_CLASS( ambient_generic, CAmbientGeneric );

//
// ambient_generic - general-purpose user-defined static sound
//
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

		vec3_t vecOrigin;
		VectorCopy( pev->origin, vecOrigin );

		UTIL_EmitAmbientSound( vecOrigin, sample, volume, attenuation );
	}
}

// RampThink - Think at 5hz if we are dynamically modifying
// pitch or volume of the playing sound.  This function will
// ramp pitch and/or volume up or down, modify pitch/volume
// with lfo if active.
void CAmbientGeneric::RampThink( void *funcArgs )
{
	if ( m_fActive )
	{
		float volume, attenuation;
		VOL_AND_ATT( volume, attenuation );

		EMIT_SOUND_DYN2( (edict_t *)pev->pContainingEntity, 1, STRING( pev->message ), volume, attenuation );
	}

	SetThink( NULL );
}

//
// ToggleUse - turns an ambient sound on or off.  If the
// ambient is a looping sound, mark sound as active (m_fActive)
// if it's playing, innactive if not.  If the sound is not
// a looping sound, never mark it as active.
//
void CAmbientGeneric::ToggleUse( void *funcArgs )
{
	float volume, attenuation;
	VOL_AND_ATT( volume, attenuation );

	if ( m_fActive )
	{
		m_fActive = FALSE;
		EMIT_SOUND_DYN2( ENT( pev ), 1, "common/null.wav", volume, attenuation );
	}
	else
	{
		m_fActive = TRUE;
		EMIT_SOUND_DYN2( ENT( pev ), 1, STRING( pev->message ), volume, attenuation );
	}
}

// KeyValue - load keyvalue pairs into member data of the
// ambient generic. NOTE: called BEFORE spawn!
void CAmbientGeneric::KeyValue( KeyValueData *pkvd )
{
}



// =================== ROOM SOUND FX ==========================================
class CEnvSound : public CBaseEntity
{
public:
	void KeyValue( KeyValueData* pkvd);
	void Spawn();

	void Think( void *funcArg );

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

	vec3_t vecSpot1, vecSpot2;
	VectorAdd( pev->view_ofs, pev->origin, vecSpot1 );
	VectorAdd( pevTarget->view_ofs, pevTarget->origin, vecSpot2 );

	TraceResult tr;
	UTIL_TraceLine( vecSpot1, vecSpot2, FALSE, ENT( pev ), &tr );
	
	// check if line of sight crosses water boundary, or is blocked
	if ( (tr.fInOpen && tr.fInWater) || tr.flFraction != 1 )
		return FALSE;

	// calc range from sound entity to player
	vec3_t vecRange;
	VectorSubtract( tr.vecEndPos, vecSpot1, vecRange );

	float flRange;
	flRange = DotProduct( vecRange, vecRange );

	if ( pSound->m_flRadius < flRange )		
		return FALSE;
	
	if ( pflRange )
		*pflRange = flRange;

	return TRUE;
}

//
// A client that is visible and in range of a sound entity will
// have its room_type set by that sound entity.  If two or more
// sound entities are contending for a client, then the nearest
// sound entity to the client will set the client's room_type.
// A client's room_type will remain set to its prior value until
// a new in-range, visible sound entity resets a new room_type.
//

// CONSIDER: if player in water state, autoset roomtype to 14,15 or 16. 
void CEnvSound::Think( void *funcArg )
{
#if 0 // TODO(SanyaSho): CBasePlayer
	// get pointer to client if visible; FIND_CLIENT_IN_PVS will
	// cycle through visible clients on consecutive calls.

	edict_t *pentPlayer = FIND_CLIENT_IN_PVS(edict());
	CBasePlayer *pPlayer = NULL;

	if (FNullEnt(pentPlayer))
		goto env_sound_Think_slow; // no player in pvs of sound entity, slow it down
 
	pPlayer = GetClassPtr( (CBasePlayer *)VARS(pentPlayer));
	float flRange;

	// check to see if this is the sound entity that is 
	// currently affecting this player

	if(!FNullEnt(pPlayer->m_pentSndLast) && (pPlayer->m_pentSndLast == ENT(pev))) {

		// this is the entity currently affecting player, check
		// for validity

		if (pPlayer->m_flSndRoomtype != 0 && pPlayer->m_flSndRange != 0) {
		
			// we're looking at a valid sound entity affecting
			// player, make sure it's still valid, update range

			if (FEnvSoundInRange(pev, VARS(pentPlayer), &flRange)) {
				pPlayer->m_flSndRange = flRange;
				goto env_sound_Think_fast;
			} else {
				
				// current sound entity affecting player is no longer valid,
				// flag this state by clearing room_type and range.
				// NOTE: we do not actually change the player's room_type
				// NOTE: until we have a new valid room_type to change it to.

				pPlayer->m_flSndRange = 0;
				pPlayer->m_flSndRoomtype = 0;
				goto env_sound_Think_slow;
			}
		} else {
			// entity is affecting player but is out of range,
			// wait passively for another entity to usurp it...
			goto env_sound_Think_slow;
		}
	}

	// if we got this far, we're looking at an entity that is contending
	// for current player sound. the closest entity to player wins.

	if (FEnvSoundInRange(pev, VARS(pentPlayer), &flRange)) 
	{
		if (flRange < pPlayer->m_flSndRange || pPlayer->m_flSndRange == 0) 
		{
			// new entity is closer to player, so it wins.
			pPlayer->m_pentSndLast = ENT(pev);
			pPlayer->m_flSndRoomtype = m_flRoomtype;
			pPlayer->m_flSndRange = flRange;
			
			// send room_type command to player's server.
			// this should be a rare event - once per change of room_type
			// only!

			//CLIENT_COMMAND(pentPlayer, "room_type %f", m_flRoomtype);
			
			MESSAGE_BEGIN( MSG_ONE, SVC_ROOMTYPE, NULL, pentPlayer );		// use the magic #1 for "one client"
				WRITE_SHORT( (short)m_flRoomtype );					// sequence number
			MESSAGE_END();

			// crank up nextthink rate for new active sound entity
			// by falling through to think_fast...
		}
		// player is not closer to the contending sound entity,
		// just fall through to think_fast. this effectively
		// cranks up the think_rate of entities near the player.
	} 

	// player is in pvs of sound entity, but either not visible or
	// not in range. do nothing, fall through to think_fast...

env_sound_Think_fast:
	pev->nextthink = gpGlobals->time + 0.25;
	return;

env_sound_Think_slow:
	pev->nextthink = gpGlobals->time + 0.75;
	return;
#endif
}

//
// env_sound - spawn a sound entity that will set player roomtype
// when player moves in range and sight.
//
void CEnvSound::Spawn()
{
	MARK_STUB_ENTITY;

	// spread think times
	pev->nextthink = gpGlobals->time + UTIL_RandomFloat( 0.0, 0.5 );
}

