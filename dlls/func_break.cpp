// func_break.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "func_break.h"


extern DLL_GLOBAL Vector g_vecAttackDir;


void CBreakable::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "explosion" ) )
	{
		if ( FStrEq( pkvd->szValue, "directed" ) )
			m_Explosion = expDirected;
		else if ( FStrEq( pkvd->szValue, "random" ) )
			m_Explosion = expRandom;
		else
			m_Explosion = expRandom;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "material" ) )
	{
		int i = (int)atof( pkvd->szValue );

		if ( i >= matLastMaterial )
			m_Material = matWood;
		else
			m_Material = (Materials)i;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "deadmodel" ) )
	{
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "shards" ) )
	{
		m_iShards = (int)atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "lip" ) || FStrEq( pkvd->szKeyName, "wait" ) )
	{
		pkvd->fHandled = TRUE;
	}
}

LINK_ENTITY_TO_CLASS( func_breakable, CBreakable );

void CBreakable::Spawn()
{
	PRECACHE_SOUND( "common/glass.wav" );

	if ( FBitSet( pev->spawnflags, 0x01 ) )
		pev->takedamage = DAMAGE_NO;
	else
		pev->takedamage = DAMAGE_AIM;

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	m_angle = pev->angles.y;

	m_iShards = MAX_SHARDS;

	switch ( m_Material )
	{
	case matGlass:
		m_idShard = PRECACHE_MODEL( "sprites/shard.spr" );

		PRECACHE_SOUND( "debris/bustglass1.wav" );
		PRECACHE_SOUND( "debris/bustglass2.wav" );
		PRECACHE_SOUND( "debris/bustglass3.wav" );
		break;
	case matWood:
		m_idShard = PRECACHE_MODEL( "models/woodgibs.mdl" );

		PRECACHE_SOUND( "debris/bustcrate1.wav" );
		PRECACHE_SOUND( "debris/bustcrate3.wav" );
		break;
	case matMetal:
		m_idShard = PRECACHE_MODEL( "models/shrapnel.mdl" );
		break;
	case matFlesh:
		m_idShard = PRECACHE_MODEL( "models/gib_b_gib.mdl" );
		break;
	case matCinderBlock:
		m_idShard = PRECACHE_MODEL( "models/cindergibs.mdl" );
		break;
	case matCeilingTile:
		m_idShard = PRECACHE_MODEL( "models/ceilinggibs.mdl" );

		PRECACHE_SOUND( "debris/bustceiling.wav" );
		break;
	};

	SET_MODEL( edict(), STRING( pev->model ) );
}

void CBreakable::Touch()
{
	entvars_t *pevToucher = VARS( gpGlobals->other );

	// Must be a client!
	if ( !FBitSet( pevToucher->flags, FL_CLIENT ) || !FBitSet( pev->spawnflags, 0x01 ) )
		return;

	float flDamage = pevToucher->velocity.Length() * 0.01f;
	if ( flDamage > 0 )
		TakeDamage( pevToucher, pevToucher, flDamage );
}

void CBreakable::Use()
{
	UTIL_MakeVectors( pev->angles );

	g_vecAttackDir = globals->v_forward;

	Die();
}

void CBreakable::Die()
{
	int iFlags = 0;

	switch ( m_Material )
	{
	case matGlass:
		iFlags = BREAK_GLASS;
		switch ( rand() % 3 )
		{
		case 0:
			EMIT_SOUND_DYN2( edict(), 2, "debris/bustglass1.wav", 1.f, 0.8f );
			break;
		case 1:
			EMIT_SOUND_DYN2( edict(), 2, "debris/bustcrate2.wav", 1.f, 0.8f );
			break;
		default:
			EMIT_SOUND_DYN2( edict(), 2, "debris/bustcrate3.wav", 1.f, 0.8f );
			break;
		};
		break;
	case matWood:
		iFlags = BREAK_WOOD;
		switch ( rand() % 2 )
		{
		case 0:
			EMIT_SOUND_DYN2( edict(), 2, "debris/bustcrate1.wav", 1.f, 0.8f );
			break;
		default:
			EMIT_SOUND_DYN2( edict(), 2, "debris/bustcrate3.wav", 1.f, 0.8f );
			break;
		};
		break;
	case matMetal:
		iFlags = BREAK_METAL | BREAK_SMOKE;
		EMIT_SOUND_DYN2( edict(), 2, "common/glass.wav", 1.f, 0.8f );
		break;
	case matFlesh:
		iFlags = BREAK_FLESH;
		EMIT_SOUND_DYN2( edict(), 2, "common/glass.wav", 1.f, 0.8f );
		break;
	case matCinderBlock:
		EMIT_SOUND_DYN2( edict(), 2, "common/glass.wav", 1.f, 0.8f );
		break;
	case matCeilingTile:
		EMIT_SOUND_DYN2( edict(), 2, "debris/bustceiling.wav", 1.f, 0.8f );
		break;
	};

	if ( m_Explosion != expDirected )
	{
		UTIL_RandomFloat( -1, 1 );
		UTIL_RandomFloat( -1, 1 );
		UTIL_RandomFloat( -1, 1 );
	}

	Vector vecPos = ( pev->mins + pev->maxs ) * 0.5f;

	WRITE_BYTE( MSG_BROADCAST, 23 ); // svc_temp_entity
	WRITE_BYTE( MSG_BROADCAST, TE_BREAKMODEL );
	WRITE_COORD( MSG_BROADCAST, vecPos.x );
	WRITE_COORD( MSG_BROADCAST, vecPos.y );
	WRITE_COORD( MSG_BROADCAST, vecPos.z );
	WRITE_COORD( MSG_BROADCAST, pev->size.x );
	WRITE_COORD( MSG_BROADCAST, pev->size.y );
	WRITE_COORD( MSG_BROADCAST, pev->size.z );
	WRITE_COORD( MSG_BROADCAST, 1 );
	WRITE_COORD( MSG_BROADCAST, 1 );
	WRITE_COORD( MSG_BROADCAST, 1 );
	WRITE_SHORT( MSG_BROADCAST, m_idShard );
	WRITE_BYTE( MSG_BROADCAST, 0 );
	WRITE_BYTE( MSG_BROADCAST, 15 );
	WRITE_BYTE( MSG_BROADCAST, iFlags );

	float flRadius = 0.f;

	if ( pev->size.x < pev->size.y )
		flRadius = pev->size.y;
	else
		flRadius = pev->size.x;

	edict_t *pEnt = UTIL_FindRadius( vecPos, flRadius );
	// TODO

	SetThink( &CBreakable::SUB_Remove );
	pev->nextthink = pev->ltime + 0.1f;
}
