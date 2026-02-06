// weapons.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "player.h"
#include "weapons.h"

int m_iShell;
int m_iShrapnel;

extern DLL_GLOBAL Vector g_vecAttackDir;
extern DLL_GLOBAL int g_fDrawLines;
DLL_GLOBAL int g_fFiringWeapon = 0;
globalvars_t *GetEntityGlobals( entvars_t *pev );

//sub_1000F0D0

//sub_1000F0E0

//sub_1000F440

void SpawnBlood( Vector vecSpot, int bloodColor, float flDamage )
{
	if ( flDamage > 255 )
		flDamage = 255;

	WRITE_BYTE( MSG_BROADCAST, 23 ); // svc_temp_entity
	WRITE_BYTE( MSG_BROADCAST, TE_BLOOD );
	WRITE_COORD( MSG_BROADCAST, vecSpot.x );
	WRITE_COORD( MSG_BROADCAST, vecSpot.y );
	WRITE_COORD( MSG_BROADCAST, vecSpot.z );
	WRITE_COORD( MSG_BROADCAST, g_vecAttackDir.x );
	WRITE_COORD( MSG_BROADCAST, g_vecAttackDir.y );
	WRITE_COORD( MSG_BROADCAST, g_vecAttackDir.z );
	WRITE_BYTE( MSG_BROADCAST, bloodColor );
	WRITE_BYTE( MSG_BROADCAST, flDamage );
}

//sub_1000F550

//sub_1000F980

//sub_1000FA20

void CSprayCan::Spawn( entvars_t *pevOwner )
{
	pev->origin = pevOwner->origin;
	pev->origin.z += 32.f; // make it in front of owner's eyes

	pev->angles = pevOwner->v_angle;

	pev->owner = OFFSET( ENT( pevOwner ) );

	m_fFrame = 0;

	// paint a decal
	SetThink( &CSprayCan::AdvanceSpriteFrame );
	pev->nextthink = gpGlobals->time + 0.1f;

	// emit the sound
	EMIT_SOUND_DYN2( edict(), 2, "player/sprayer.wav", 1.0f, 0.8f );
}

void CSprayCan::AdvanceSpriteFrame( void *funcArgs )
{
	UTIL_MakeVectors( pev->angles );

	Vector vecPos = pev->origin + ( GetEntityGlobals( pev )->v_forward * 128.f );

	TraceResult tr;
	UTIL_TraceLine( pev->origin, vecPos, 0, ENT( pev->owner ), &tr );

	WRITE_BYTE( MSG_BROADCAST, 23 ); // svc_temp_entity
	WRITE_BYTE( MSG_BROADCAST, TE_GUNSHOTDECAL );
	WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.x );
	WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.y );
	WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.z );
	WRITE_SHORT( MSG_BROADCAST, ENTINDEX( tr.fHit ) );
	WRITE_BYTE( MSG_BROADCAST, m_fFrame + 6 );

	byte fPrevFrame = m_fFrame;

	m_fFrame++; // increment the frame counter

	if ( fPrevFrame >= 5 )
		SetThink( &CSprayCan::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1f;
}

globalvars_t *GetEntityGlobals( entvars_t *pev )
{
	return (globalvars_t *)pev->pSystemGlobals;
}

void CBloodSplat::Spawn( entvars_t *pevOwner )
{
	pev->origin = pevOwner->origin;
	pev->origin.z += 32.f; // make it in front of owner's eyes

	pev->angles = pevOwner->v_angle;

	pev->owner = OFFSET( ENT( pevOwner ) );

	// paint a decal
	SetThink( &CBloodSplat::AdvanceSpriteFrame );
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBloodSplat::AdvanceSpriteFrame( void *funcArgs )
{
	UTIL_MakeVectors( pev->angles );

	Vector vecPos = pev->origin + ( GetEntityGlobals( pev )->v_forward * 128.f );

	TraceResult tr;
	UTIL_TraceLine( pev->origin, vecPos, 0, ENT( pev->owner ), &tr );

	WRITE_BYTE( MSG_BROADCAST, 23 ); // svc_temp_entity
	WRITE_BYTE( MSG_BROADCAST, TE_GUNSHOTDECAL );
	WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.x );
	WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.y );
	WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.z );
	WRITE_SHORT( MSG_BROADCAST, g_engfuncs.pfnIndexOfEdict( tr.fHit ) );
	WRITE_BYTE( MSG_BROADCAST, (rand()%6) + 14 );

	SetThink( &CBloodSplat::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBasePlayer::ImpulseCommands()
{
	TraceResult tr;

	if ( FBitSet( pev->button, IN_USE ) && ( gpGlobals->time > m_flNextUseUpdateTime ) )
	{
		PlayerUse();
		m_flNextUseUpdateTime = gpGlobals->time + 0.5f;
	}

	switch ( (int)pev->impulse )
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	{
		//set_next_weapon(pev, pev->impulse);
		break;
	}
	case 10: // next weapon in the list
	{
		//set_next_weapon(pev, pev->weapon+1);
		break;
	}
	case 11: // previous weapon in the list
	{
		int iWeapon = pev->weapon - 1;
		if ( iWeapon < 0 )
			iWeapon = MAX_WEAPONS - 1;

		//set_prev_weapon(pev, iWeapon);
		break;
	}
	case 100: // flashlight
	{
		if ( FBitSet( pev->effects, EF_DIMLIGHT ) )
		{
			ClearBits( pev->effects, EF_DIMLIGHT );
		}
		else
		{
			SetBits( pev->effects, EF_DIMLIGHT );
		}
		break;
	}
	case 200: // debugging lines
	{
		if ( g_fDrawLines )
		{
			g_fDrawLines = 0;
			ALERT( at_info, "Lines Off\n" );
		}
		else
		{
			g_fDrawLines = 1;
			ALERT( at_info, "Lines On\n" );
		}
		break;
	}
	case 201: // spray
	{
		UTIL_MakeVectors( pev->v_angle );

		Vector vecStart = pev->origin + pev->view_ofs;
		Vector vecEnd = pev->origin + pev->view_ofs + ( GetEntityGlobals( pev )->v_forward * 128.f );

		UTIL_TraceLine( vecStart, vecEnd, 0, edict(), &tr );

		if ( tr.flFraction != 1.f )
		{
			GetClassPtr( (CSprayCan *)NULL )->Spawn( pev );
		}
		break;
	}
	case 202: // blood spray
	{
		UTIL_MakeVectors( pev->v_angle );

		Vector vecStart = pev->origin + pev->view_ofs;
		Vector vecEnd = pev->origin + pev->view_ofs + ( GetEntityGlobals( pev )->v_forward * 128.f );

		TraceResult tr;
		UTIL_TraceLine( vecStart, vecEnd, 0, edict(), &tr );

		if ( tr.flFraction != 1.f )
		{
			GetClassPtr( (CBloodSplat *)NULL )->Spawn( pev );
		}
		break;
	}
	case 203: // remove entity
	{
		UTIL_MakeVectors( pev->v_angle );

		Vector vecStart = pev->origin + Vector( 0, 0, 16 );
		Vector vecEnd = pev->origin + Vector( 0, 0, 16 ) + ( GetEntityGlobals( pev )->v_forward * 1024.f );

		UTIL_TraceLine( vecStart, vecEnd, 0, edict(), &tr );

		entvars_t *pEnt = VARS( tr.fHit );
		if ( pEnt->takedamage != DAMAGE_NO )
		{
			CBaseEntity::Instance( pEnt )->SetThink( &CBaseEntity::SUB_Remove );
		}
		break;
	}
	};

	pev->impulse = 0; // reset the impulse
}

//sub_100103F0 // <-- fire the weapon

void CBasePlayer::GiveDefaultWeapons()
{
	switch ( pev->weapon )
	{
	case 1:
		pev->weaponmodel = ALLOC_STRING( "models/v_crowbar.mdl" );
		pev->currentammo = 99;
		break;
	case 2:
		pev->weaponmodel = ALLOC_STRING( "models/v_glock.mdl" );
		pev->currentammo = 99;
		break;
	case 3:
		pev->weaponmodel = ALLOC_STRING( "models/v_mp5.mdl" );
		pev->currentammo = 99;
		break;
	default:
		pev->weaponmodel = 0;
		pev->currentammo = 0;
		break;
	}
}

void CBasePlayer::FireMP5()
{
}

void CBasePlayer::FireGlock()
{
}

void CBasePlayer::FireCrowbar()
{
}

void CBasePlayer::UpdateWeaponAnim()
{
	if ( pev->weapon == 4 )
	{
		gpGlobals->msg_entity = OFFSET( edict() );

		WRITE_BYTE( MSG_ONE, 35 ); // svc_weaponanim
		WRITE_BYTE( MSG_ONE, 1 );

		gpGlobals->msg_entity = 0;

		m_flNextWeaponAnimUpdateTime = gpGlobals->time + 0.5f + UTIL_RandomFloat( 0.f, 5.f );
	}
}

void ExplodeModel( Vector vecOrigin, float speed, int model, int count )
{
	WRITE_BYTE( MSG_BROADCAST, 23 ); // svc_temp_entity
	WRITE_BYTE( MSG_BROADCAST, TE_EXPLODEMODEL );
	WRITE_COORD( MSG_BROADCAST, vecOrigin.x );
	WRITE_COORD( MSG_BROADCAST, vecOrigin.y );
	WRITE_COORD( MSG_BROADCAST, vecOrigin.z );
	WRITE_COORD( MSG_BROADCAST, speed );
	WRITE_SHORT( MSG_BROADCAST, model );
	WRITE_SHORT( MSG_BROADCAST, count );
	WRITE_BYTE( MSG_BROADCAST, 15 );
}

void CBasePlayer::WeaponThink()
{
	if ( globals->time < m_flNextWeaponThink )
		return;

	ImpulseCommands();

	if ( FBitSet( pev->button, IN_CANCEL ) ) // IN_CANCEL?
	{
		if ( pev->weapon & 0xFFFF0000 )
			pev->weapon = ( pev->weapon & 0xFF0000 ) >> 16; // TODO

		return;
	}

	// TODO
	{
	}

	if ( FBitSet( pev->button, IN_ATTACK ) )
	{
		/*if ( !g_fFiringWeapon )
		{
			if ( ( pev->weapon & 0xFFFF0000 ) != 0 )
			{
				pev->weapon = 1; //??
				ClearBits( pev->button, IN_ATTACK );
				GiveDefaultWeapons();
				g_fFiringWeapon = 1;
			}
			else
			{
				//FireWeapon();
			}
		}*/

		UTIL_MakeVectors( pev->v_angle );

		Vector vecStart = pev->origin;
		Vector vecEnd = pev->origin + ( gpGlobals->v_forward * 2048.f );

		TraceResult tr;
		UTIL_TraceLine( vecStart, vecEnd, 0, edict(), &tr );

		WRITE_BYTE( MSG_BROADCAST, 23 );
		WRITE_BYTE( MSG_BROADCAST, TE_TRACER );
		WRITE_COORD( MSG_BROADCAST, vecStart.x );
		WRITE_COORD( MSG_BROADCAST, vecStart.y );
		WRITE_COORD( MSG_BROADCAST, vecStart.z );
		WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.x );
		WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.y );
		WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.z );

		WRITE_BYTE( MSG_BROADCAST, 23 );
		WRITE_BYTE( MSG_BROADCAST, TE_GUNSHOTDECAL );
		WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.x );
		WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.y );
		WRITE_COORD( MSG_BROADCAST, tr.vecEndPos.z );
		WRITE_SHORT( MSG_BROADCAST, ENTINDEX( tr.fHit ) );
		WRITE_BYTE( MSG_BROADCAST, 1 );

		if ( tr.fHit != 0 )
		{
			if ( VARS( tr.fHit )->takedamage != DAMAGE_NO )
			{
				CBaseMonster *pMonster = (CBaseMonster *)CBaseMonster::Instance( tr.fHit );
				if ( pMonster )
					pMonster->TakeDamage( pev, pev, 10 );
			}
		}
	}
	/*else
	{
		g_fFiringWeapon = 0;
	}*/

	m_flNextWeaponThink = globals->time + 0.15f;
}

void W_Precache()
{
	PRECACHE_MODEL( "models/grenade.mdl" );
	PRECACHE_MODEL( "sprites/shard.spr" );
	PRECACHE_MODEL( "models/v_crowbar.mdl" );
	PRECACHE_MODEL( "models/v_glock.mdl" );
	PRECACHE_MODEL( "models/v_mp5.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shell.mdl" );
	m_iShrapnel = PRECACHE_MODEL( "models/shrapnel.mdl" );

	PRECACHE_SOUND( "weapons/debris1.wav" );
	PRECACHE_SOUND( "weapons/debris2.wav" );
	PRECACHE_SOUND( "weapons/debris3.wav" );

	PRECACHE_SOUND( "weapons/pl_shell1.wav" );
	PRECACHE_SOUND( "weapons/pl_shell2.wav" );
	PRECACHE_SOUND( "weapons/pl_shell3.wav" );

	PRECACHE_SOUND( "weapons/hks1.wav" );
	PRECACHE_SOUND( "weapons/hks2.wav" );
	PRECACHE_SOUND( "weapons/hks3.wav" );

	PRECACHE_SOUND( "weapons/pl_gun1.wav" );
	PRECACHE_SOUND( "weapons/pl_gun2.wav" );

	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );

	PRECACHE_SOUND( "weapons/g_bounce1.wav" );
	PRECACHE_SOUND( "weapons/g_bounce2.wav" );
	PRECACHE_SOUND( "weapons/g_bounce3.wav" );
}