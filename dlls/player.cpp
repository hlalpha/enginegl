// player.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "player.h"

extern DLL_GLOBAL unsigned int g_ulModelIndexPlayer;
extern void LoadSavedParams( entvars_t *pev );

DLL_GLOBAL EOFFSET g_eoLastSpawn;

void set_suicide_frame( entvars_t *pent )
{
	if ( FStrEq( STRING( pent->model ), "models/doctor.mdl" ) )
	{
		pent->solid = SOLID_NOT;
		pent->movetype = MOVETYPE_TOSS;
		pent->deadflag = DEAD_DEAD;
		pent->nextthink = -1;
	}
}

void PlayerDeathSound( entvars_t *pent );
void CBasePlayer::Die()
{
	pev->modelindex = g_ulModelIndexPlayer;
	pev->weaponmodel = 0; // hide weapon model

	pev->view_ofs = Vector( 0, 0, -8 ); // TODO(SanyaSho): Y is unknown!

	pev->deadflag = DEAD_DEAD;
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_TOSS;
	//remove some flag from pev->flags // TODO

	if ( pev->velocity.z < 10 )
		pev->velocity.z += UTIL_RandomFloat( 0.f, 300.f );

	if ( pev->health <= -40 )
	{
		PlayerDeathSound( pev );

		pev->angles.x = 0;
		pev->angles.z = 0;

		SetActivity( 35 );

		SetThink( &CBasePlayer::PlayerDeathThink );
		pev->nextthink = 0.1;
	}
}

void PlayerDeathSound( entvars_t *pent )
{
	switch ( UTIL_RandomLong( 1, 5 ) )
	{
	case 1:
		EMIT_SOUND_DYN2( ENT( pent ), 2, "player/pl_pain5.wav", 1.0, 0.8 );
		break;
	case 2:
		EMIT_SOUND_DYN2( ENT( pent ), 2, "player/pl_pain6.wav", 1.0, 0.8 );
		break;
	case 3:
		EMIT_SOUND_DYN2( ENT( pent ), 2, "player/pl_pain7.wav", 1.0, 0.8 );
		break;
	};
}

void CBasePlayer::SetActivity( int iActivity )
{
	int animDesired = 0;

	switch ( iActivity )
	{
	case 1:
		animDesired = 7;
		break;
	case 4:
		animDesired = 0;
		break;
	case 30:
		animDesired = ( pev->velocity.x != 0 || pev->velocity.y != 0 ) ? 2 : 3;
		break;
	case 35: // death
		animDesired = 6;
		break;
	case 40:
		animDesired = 5;
		break;
	}

	if ( pev->sequence != animDesired )
	{
		pev->sequence = animDesired;
		pev->frame = 0.f;
		ResetSequenceInfo( 0.1f );

		if ( animDesired > 6 )
		{
			m_flFrameRate = 0.f;
			m_flGroundSpeed = 0.f;
		}
	}
}

void CBasePlayer::nullsub_13()
{
}

void CBasePlayer::WaterMove()
{
	// SDKTODO
}

void CBasePlayer::CheckWaterJump()
{
	// SDKTODO
}

extern void respawn( entvars_t *pev );
void CBasePlayer::PlayerDeathThink( void *funcArgs )
{
	if ( FBitSet( pev->flags, FL_ONGROUND ) )
	{
		float flForward = pev->velocity.Length() - 20;
		if ( flForward <= 0 )
		{
			pev->velocity = g_vecZero;
		}
		else
		{
			pev->velocity = flForward * pev->velocity.Normalize();
		}
	}

	if ( !m_fSequenceFinished && ( pev->deadflag == DEAD_DYING ) )
	{
		StudioFrameAdvance( 0.1 );
	}

	if ( pev->deadflag == DEAD_DYING )
	{
		pev->deadflag = DEAD_DEAD;
	}

	if ( pev->deadflag == DEAD_DEAD )
	{
		if ( pev->button )
			return;

		pev->deadflag = DEAD_RESPAWNABLE;
		return;
	}

	if ( !pev->button )
	{
		return;
	}

	pev->button = 0;
	respawn( pev );
	pev->nextthink = -1;
}

void CBasePlayer::PlayerUse()
{
#if 0
	// SDKTODO
	edict_t *pEnt = UTIL_FindRadius( pev->origin, 64.f );

	while ( pEnt != NULL )
	{
		if ( FNullEnt( pEnt ) )
			continue;

		ALERT( at_info, "%s\n", STRING( pEnt->v.classname ) );

		pEnt = g_engfuncs.pfnPEntityOfEntOffset( g_engfuncs.pfnGetVarsOfEnt( pEnt )->chain );
	}
#endif
}

void CBasePlayer::Jump()
{
	if ( FBitSet( pev->flags, FL_WATERJUMP ) )
		return;

	if ( pev->waterlevel < 2 )
	{
		if ( FBitSet( pev->flags, FL_ONGROUND ) && FBitSet( pev->flags, FL_JUMPRELEASED ) )
		{
			ClearBits( pev->flags, FL_ONGROUND );
			ClearBits( pev->flags, FL_JUMPRELEASED );

			SetActivity( 40 );

			pev->button &= ~IN_JUMP;

			if ( UTIL_RandomFloat( 0.f, 1.f ) < 0.5f )
				EMIT_SOUND_DYN2( edict(), 4, "player/pl_jump1.wav", 1.f, 0.8f );
			else
				EMIT_SOUND_DYN2( edict(), 4, "player/pl_jump2.wav", 1.f, 0.8f );

			pev->velocity.z += 270.f;
		}
	}
	else
	{
		switch ( (int)pev->watertype )
		{
		case -4:
			pev->velocity.z = 80;
			break;
		case -3:
			pev->velocity.z = 100;
			break;
		default:
			pev->velocity.z = 50;
		};

		if ( gpGlobals->time > m_flNextWaterMoveSound )
		{
			m_flNextWaterMoveSound = gpGlobals->time + 1.f;

			if ( UTIL_RandomFloat( 0.f, 1.f ) < 0.5f )
				EMIT_SOUND_DYN2( edict(), 4, "common/water1.wav", 1.f, 0.8f );
			else
				EMIT_SOUND_DYN2( edict(), 4, "common/water2.wav", 1.f, 0.8f );
		}
	}
}

void CBasePlayer::Duck()
{
	if ( FBitSet( pev->button, IN_DUCK ) )
	{
		if ( !FBitSet( pev->flags, FL_DUCKING ) )
		{
			UTIL_SetSize( pev, Vector( -16, -16, -18 ), Vector( 16, 16, 18 ) );

			pev->view_ofs = Vector( 0, 0, 12 );

			SetBits( pev->flags, FL_DUCKING );
		}
	}
	else
	{
		Vector origOrigin = pev->origin;

		UTIL_SetSize( pev, Vector( -16, -16, -36 ), Vector( 16, 16, 36 ) );

		pev->origin.z += 18.f;
		SetBits( pev->flags, FL_PARTIALGROUND );

		if ( WALK_MOVE( edict(), 0.f, 0.f ) == 0.f )
		{
			UTIL_SetSize( pev, Vector( -16, -16, -18 ), Vector( 16, 16, 18 ) );
			pev->origin = origOrigin;
		}
		else
		{
			ClearBits( pev->flags, FL_DUCKING );
			pev->view_ofs = Vector( 0, 0, 28 );
		}
	}
}

int CBasePlayer::Classify()
{
	return 4; // SDKTODO
}

void CBasePlayer::nullsub_14()
{
}

void __stdcall PlayerPreThink( globalvars_t *funcArgs ) // exported
{
	CBasePlayer *pPlayer = (CBasePlayer *)CBasePlayer::Instance( funcArgs->self );
	if ( pPlayer )
		pPlayer->PreThink();
}

void CBasePlayer::PreThink()
{
	if ( pev->view_ofs == g_vecZero )
		return;

	UTIL_MakeVectors( pev->v_angle );

	nullsub_14();

	nullsub_13();

	WaterMove();

	if ( pev->waterlevel == 2 )
		CheckWaterJump();

	if ( pev->deadflag >= DEAD_DYING )
	{
		PlayerDeathThink( NULL );
		return;
	}

	if ( FBitSet( pev->button, IN_JUMP ) )
		Jump();
	else
		SetBits( pev->flags, FL_JUMPRELEASED );

	if ( FBitSet( pev->button, IN_DUCK ) || FBitSet( pev->flags, FL_DUCKING ) )
		Duck();

	// ???
	/*if ( pSystemGlobals[10].y < (double)*(float *)&this->unk66 )
	{
		v6->velocity = g_vecZero;
	}*/

	if ( gpGlobals->time > m_flNextStepSoundTime && FBitSet( pev->flags, FL_ONGROUND ) )
	{
		if ( pev->velocity != g_vecZero && pev->velocity.Length() > 200 )
		{
			SetActivity( 4 );

			float flVolume = 0.f, flAttenuation = 0.f;
			char *pSound = NULL;

			float flRand = UTIL_RandomFloat( 0.f, 1.f );
			if ( flRand < 0.25f )
			{
				flAttenuation = 0.8f;
				flVolume = 1.f;
				pSound = "player/pl_step1.wav";
			}
			else if ( flRand < 0.5f )
			{
				flAttenuation = 0.8f;
				flVolume = 1.f;
				pSound = "player/pl_step2.wav";
			}
			else
			{
				flAttenuation = 0.8f;
				flVolume = 1.f;

				if ( flRand <= 0.75f )
					pSound = "player/pl_step3.wav";
				else
					pSound = "player/pl_step4.wav";
			}

			EMIT_SOUND_DYN2( edict(), 4, pSound, flVolume, flAttenuation );

			m_flNextStepSoundTime = gpGlobals->time + 0.3f;
		}
	}

	if ( gpGlobals->time > m_flNextWeaponAnimUpdateTime )
		UpdateWeaponAnim();

	UTIL_MakeVectors( pev->angles );

	// TODO: ladder code
}

void __stdcall PlayerPostThink( globalvars_t *funcArgs ) // exported
{
	CBasePlayer *pPlayer = (CBasePlayer *)CBasePlayer::Instance( funcArgs->self );
	if ( pPlayer )
		pPlayer->PostThink();
}

extern void set_player_model( entvars_t *pent );
void CBasePlayer::PostThink()
{
	// SDKTODO
	if ( pev->view_ofs == g_vecZero || pev->deadflag != DEAD_NO )
	{
		return;
	}

	WeaponThink();

	// TODO
	{
	}

	StudioFrameAdvance( 0.1f );

	set_player_model( pev );
}

void set_player_model( entvars_t *pent )
{
	if ( pent->health < 0 )
		return;

	pent->modelindex = g_ulModelIndexPlayer;
}

extern edict_t *EntSelectSpawnPoint( globalvars_t *globals );
void CBasePlayer::Spawn()
{
	pev->classname = ALLOC_STRING( "player" );

	pev->health = 100;

	pev->takedamage = DAMAGE_AIM;
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_WALK;

	pev->max_health = 100;

	pev->flags = FL_CLIENT;

	pev->air_finished = globals->time + 12;

	pev->dmg = 2;
	pev->effects = 0;
	pev->sequence = 9; // TODO

	pev->deadflag = DEAD_NO;

	LoadSavedParams( pev ); // load params from saved struct

	//m_flNextSecondaryAttack = 0.0f;
	//m_flNextStepSoundTime = 0.0f;
	unk67 = 70;
	m_flNextWeaponThink = globals->time;
	unk66 = 0;

#if 0
	SDK_SaveRestore *saved = (SDK_SaveRestore *)globals->parm1;
	Vector spawnOrigin, spawnAngles;
	edict_t *pSpawnSpot = nullptr;

	if ( !saved || !saved->level_transition )
	{
		pSpawnSpot = EntSelectSpawnPoint( globals );
		entvars_t *spotVars = g_engfuncs.pfnGetVarsOfEnt( pSpawnSpot );

		spawnOrigin = spotVars->origin + Vector( 0, 0, 1 );
		spawnAngles = spotVars->angles;

		pev->weapon = 2;
	}
	else
	{
		edict_t *pTarget = g_engfuncs.pfnFindEntityByString( nullptr, "targetname", saved->target );
		if ( !pTarget || !g_engfuncs.pfnEntOffsetOfPEntity( pTarget ) )
		{
			g_engfuncs.pfnAlertMessage( at_info, "No Landmark: %s\n", saved->target );
			pSpawnSpot = EntSelectSpawnPoint( globals );
			entvars_t *spotVars = g_engfuncs.pfnGetVarsOfEnt( pSpawnSpot );
			spawnOrigin = spotVars->origin + Vector( 0, 0, 1 );
			spawnAngles = spotVars->angles;
		}
		else
		{
			entvars_t *pTargetVars = g_engfuncs.pfnGetVarsOfEnt( pTarget );
			spawnOrigin = pTargetVars->origin + saved->vecOrigin;
			spawnAngles = saved->vecAngles;
			pev->v_angle = saved->vecVAngle * -1;
			pev->weapon = saved->weapon;
		}

		pev->fixangle = 1;
		free( saved );
		globals->parm1 = 0; // reset previous state
	}

	pev->origin = spawnOrigin;
	pev->angles = spawnAngles;
#endif

	pev->origin = Vector( 0, 0, 64 );
	pev->angles = g_vecZero;

	SAVE_SPAWN_PARMS( ENT( pev ) );		// update save data

	SET_MODEL( edict(), "models/doctor.mdl" );
	g_ulModelIndexPlayer = pev->modelindex;

	UTIL_SetSize( pev, Vector( -16, -16, -36 ), Vector( 16, 16, 36 ) );

	pev->weapons = 23; // TODO

	GiveDefaultWeapons();

	pev->view_ofs = Vector( 0, 0, 28 );
}

extern void __stdcall SetNewParms( globalvars_t *globals );
void __stdcall SetChangeParms( globalvars_t *globals ) // exported
{
	entvars_t *pClient = VARS( globals->self );

	if ( pClient->health <= 0 )
	{
		SetNewParms( globals );
		return;
	}

	if ( pClient->health > 100 )
		pClient->health = 100;
	if ( pClient->health < 50 )
		pClient->health = 50; // :heavyweightsad:

	SDK_SaveRestore *pSaveRestore = (SDK_SaveRestore *)&globals->parm1;
	pSaveRestore->items = pClient->items;
	pSaveRestore->health = pClient->health;
	pSaveRestore->armorvalue = pClient->armorvalue;
	pSaveRestore->ammo_1 = pClient->ammo_1;
	pSaveRestore->ammo_2 = pClient->ammo_2;
	pSaveRestore->ammo_3 = pClient->ammo_3;
	pSaveRestore->ammo_4 = pClient->ammo_4;
	pSaveRestore->weapon = pClient->weapon;
	pSaveRestore->armortype = pClient->armortype * 100;
}