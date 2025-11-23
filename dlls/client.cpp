// client.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "player.h"

int framecount;

extern DLL_GLOBAL unsigned int g_ulModelIndexPlayer;
extern DLL_GLOBAL int g_iSkillLevel;
extern DLL_GLOBAL EOFFSET g_eoLastSpawn;

extern void set_suicide_frame( entvars_t *pent );
extern void CopyToBodyQue( entvars_t *pev );

void __stdcall ClientConnect( globalvars_t *globals ) // exported
{
	/* Nothing */
}

void __stdcall ClientDisconnect( globalvars_t *globals ) // exported
{
	/*if ( g_fGameOver )
		return;*/

	EMIT_SOUND_DYN2( ENT( globals->self ), 4, "player/tornoff2.wav", 1.0, 0 );
	set_suicide_frame( VARS( globals->self ) );
}

void __stdcall PutClientInServer( globalvars_t *globals );
void __stdcall SetNewParms( globalvars_t *globals );

void respawn( entvars_t *pev )
{
	if ( gpGlobals->coop || gpGlobals->deathmatch )
	{
		CopyToBodyQue( pev );

		if ( gpGlobals->coop )
			SET_SPAWN_PARMS( ENT( pev ) );
		else
			SetNewParms( gpGlobals );

		gpGlobals->self = OFFSET( pev );
		PutClientInServer( gpGlobals );
	}
	else
	{
		SERVER_COMMAND( "restart\n" );
	}
}

void __stdcall ClientKill( globalvars_t *globals ) // exported
{
	entvars_t *pClient = VARS( globals->self );
	set_suicide_frame( pClient );
	pClient->modelindex = g_ulModelIndexPlayer;
	pClient->frags -= 2;
	respawn( pClient );
}

void LoadSavedParams( entvars_t *pev )
{
	if ( !gpGlobals->parm1 )
		SetNewParms( gpGlobals );

#if 0
	SDK_SaveRestore *pSaveRestore = (SDK_SaveRestore *)gpGlobals->parm1;
	if ( pSaveRestore )
	{
		pev->items = pSaveRestore->items;
		pev->health = pSaveRestore->health;
		pev->armorvalue = pSaveRestore->armorvalue;
		pev->ammo_1 = pSaveRestore->ammo_1;
		pev->ammo_2 = pSaveRestore->ammo_2;
		pev->ammo_3 = pSaveRestore->ammo_3;
		pev->ammo_4 = pSaveRestore->ammo_4;
		pev->weapon = pSaveRestore->weapon;
		pev->armortype = pSaveRestore->armortype * 0.01;

		if ( pSaveRestore->buttons & IN_CANCEL ) // IN_CANCEL?
		{
			SetBits( pev->flags, 0x4000 ); // SDKTODO

			UTIL_SetSize( pev, Vector( -16, -16, -18 ), Vector( 16, 16, 18 ) );
		}
	}
#endif
}

edict_t *EntSelectSpawnPoint( globalvars_t *globals )
{
	edict_t *pSpot;

	if ( globals->coop )
	{
		// Try to use info_player_coop as a spawn point
		pSpot = FIND_ENTITY_BY_STRING( ENT( g_eoLastSpawn ), "classname", "info_player_coop" );
		if ( !FNullEnt( pSpot ) )
			goto ReturnSpot;

		// If there's no info_player_coop then fallback to info_player_start
		pSpot = FIND_ENTITY_BY_STRING( ENT( g_eoLastSpawn ), "classname", "info_player_start" );
		if ( !FNullEnt( pSpot ) )
			goto ReturnSpot;
	}
	else if ( globals->deathmatch )
	{
		// SDKTODO
		pSpot = FIND_ENTITY_BY_STRING( NULL, "classname", "info_player_deathmatch" );
		if ( pSpot )
			goto ReturnSpot;
	}
	else
	{
		pSpot = FIND_ENTITY_BY_STRING( NULL, "classname", "info_player_start" );
		if ( pSpot )
			goto ReturnSpot;
	}

ReturnSpot:
	if ( FNullEnt( pSpot ) )
	{
		ALERT( at_error, "PutClientInServer: no info_player_start on level" );
	}

	g_eoLastSpawn = OFFSET( pSpot );
	return pSpot;
}

void __stdcall PutClientInServer( globalvars_t *globals ) // exported
{
	CBasePlayer *pPlayer = GetClassPtr( (CBasePlayer *)VARS( globals->self ) ); // HACK(SanyaSho): FIXME
	pPlayer->Spawn();
}

void __stdcall SetNewParms( globalvars_t *globals ) // exported
{
	SDK_SaveRestore *pSaveRestore = (SDK_SaveRestore *)malloc( sizeof( SDK_SaveRestore ) );

	// SDKTODO

	// New client
	pSaveRestore->level_transition = FALSE;

	globals->parm1 = *(float *)pSaveRestore;
}

void __stdcall StartFrame( globalvars_t *globals ) // exported
{
	globals->teamplay = CVAR_GET_FLOAT( "teamplay" );
	g_iSkillLevel = (int)CVAR_GET_FLOAT( "skill" );
	framecount++;
}
