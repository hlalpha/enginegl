/*

===== world.cpp ========================================================

  precaches and defs for entities and other data that must always be available.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

/*extern*/ CBaseEntity				*g_pLastSpawn; // TODO(SanyaSho): Move it into player.cpp
DLL_GLOBAL EOFFSET					g_eoBodyQueueHead;

//extern void W_Precache();
void W_Precache()
{
#if 0
	PRECACHE_MODEL( "models/grenade.mdl" );
	PRECACHE_MODEL( "sprites/shard.spr" );
	PRECACHE_MODEL( "models/v_crowbar.mdl" );
	PRECACHE_MODEL( "models/v_glock.mdl" );
	PRECACHE_MODEL( "models/v_mp5.mdl" );
#endif

#if 0
	m_iShell = PRECACHE_MODEL( "models/shell.mdl" );
	m_iShrapnel = PRECACHE_MODEL( "models/shrapnel.mdl" );
#endif

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

char *g_szDecals[] =
{
	"{shot1",
	"{shot2",
	"{shot3",
	"{shot4",
	"{shot5",
	"{hl",
	"{lambda01",
	"{lambda02",
	"{lambda03",
	"{lambda04",
	"{lambda05",
	"{lambda06",
	"{scorch1",
	"{scorch2",
	"{blood1",
	"{blood2",
	"{blood3",
	"{blood4",
	"{blood5",
	"{blood6",
	"{yblood1",
	"{yblood2",
	"{yblood3",
	"{yblood4",
	"{yblood5",
	"{yblood6",
	"{break1",
	"{break2",
	"{break3"
};


/*
==============================================================================

BODY QUE

==============================================================================
*/
static void InitBodyQue()
{
	string_t istrClassname = ALLOC_STRING( "bodyque" );

	g_eoBodyQueueHead = OFFSET( CREATE_ENTITY() );

	entvars_t *pev = VARS( g_eoBodyQueueHead );
	pev->classname = istrClassname;

	// Reserve 3 more slots for dead bodies
	for ( int i = 0; i < 3; i++ )
	{
		pev->owner = OFFSET( CREATE_ENTITY() );
		pev->classname = istrClassname;
		pev = VARS( pev->owner );
	}

	pev->owner = g_eoBodyQueueHead;
}

//
// make a body que entry for the given ent so the ent can be respawned elsewhere
//
// GLOBALS ASSUMED SET:  g_eoBodyQueueHead
//
void CopyToBodyQue( entvars_t *pev )
{
	entvars_t *pevHead = VARS( ENT( g_eoBodyQueueHead ) );

	pevHead->angles = pev->angles;
	pevHead->model = pev->model;
	pevHead->modelindex = pev->modelindex;
	pevHead->frame = pev->frame;
	pevHead->colormap = pev->colormap;
	pevHead->movetype = pev->movetype;
	pevHead->velocity = pev->velocity;
	pevHead->flags = 0;
	pevHead->deadflag = pev->deadflag;
	pevHead->sequence = pev->sequence;

	UTIL_SetOrigin( pevHead, pev->origin );

	UTIL_SetSize( pevHead, pev->mins, pev->maxs );

	g_eoBodyQueueHead = pevHead->owner;
}


// moved CWorld class definition to cbase.h
//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================

LINK_ENTITY_TO_CLASS( worldspawn, CWorld );

void CWorld::Spawn()
{
	g_pLastSpawn = NULL;

	InitBodyQue();

	CVAR_SET_STRING( "sv_gravity", "800" );	// 67ft/sec
	CVAR_SET_STRING( "room_type", "0" );	// clear DSP

	//dword_10032878 = 0;
	//dword_100328AC = 0;

// player precaches     
	W_Precache ();									// get weapon precaches

	PRECACHE_SOUND( "common/null.wav" );
	PRECACHE_SOUND( "common/thump.wav" );
	PRECACHE_SOUND( "common/h2ohit1.wav" );
	PRECACHE_SOUND( "common/outwater.wav" );
	PRECACHE_SOUND( "common/talk.wav" );
	PRECACHE_SOUND( "player/gasp2.wav" );
	PRECACHE_SOUND( "player/sprayer.wav" );
	PRECACHE_SOUND( "player/pl_jumpland2.wav" );
	PRECACHE_SOUND( "player/pl_fallpain1.wav" );
	PRECACHE_SOUND( "player/pl_fallpain2.wav" );
	PRECACHE_SOUND( "player/pl_fallpain3.wav" );
	PRECACHE_SOUND( "player/gasp1.wav" );
	PRECACHE_SOUND( "player/pl_jump1.wav" );
	PRECACHE_SOUND( "player/pl_jump2.wav" );
	PRECACHE_SOUND( "player/pl_step1.wav" );
	PRECACHE_SOUND( "player/pl_step2.wav" );
	PRECACHE_SOUND( "player/pl_step3.wav" );
	PRECACHE_SOUND( "player/pl_step4.wav" );
	PRECACHE_SOUND( "hgrunt/gr_step1.wav" );
	PRECACHE_SOUND( "hgrunt/gr_step2.wav" );
	PRECACHE_SOUND( "hgrunt/gr_step3.wav" );
	PRECACHE_SOUND( "hgrunt/gr_step4.wav" );
	PRECACHE_SOUND( "player/inh2o.wav" );
	PRECACHE_SOUND( "common/bodysplat.wav" );
	PRECACHE_SOUND( "player/tornoff2.wav" );
	PRECACHE_SOUND( "player/pl_pain2.wav" );
	PRECACHE_SOUND( "player/pl_pain4.wav" );
	PRECACHE_SOUND( "player/pl_pain5.wav" );
	PRECACHE_SOUND( "player/pl_pain6.wav" );
	PRECACHE_SOUND( "player/pl_pain7.wav" );
	PRECACHE_SOUND( "common/water1.wav" );
	PRECACHE_SOUND( "common/water2.wav" );

	PRECACHE_MODEL( "models/doctor.mdl" );
	PRECACHE_MODEL( "models/gib_b_bone.mdl" );
	PRECACHE_MODEL( "models/gib_b_gib.mdl" );
	PRECACHE_MODEL( "models/gib_legbone.mdl" );
	PRECACHE_MODEL( "models/gib_skull.mdl" );
	PRECACHE_MODEL( "models/gib_lung.mdl" );

//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//

	// 0 normal
	LIGHT_STYLE(0, "m");

	// 1 FLICKER (first variety)
	LIGHT_STYLE(1, "mmnmmommommnonmmonqnmmo");

	// 2 SLOW STRONG PULSE
	LIGHT_STYLE(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");

	// 3 CANDLE (first variety)
	LIGHT_STYLE(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");

	// 4 FAST STROBE
	LIGHT_STYLE(4, "mamamamamama");

	// 5 GENTLE PULSE 1
	LIGHT_STYLE(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");

	// 6 FLICKER (second variety)
	LIGHT_STYLE(6, "nmonqnmomnmomomno");

	// 7 CANDLE (second variety)
	LIGHT_STYLE(7, "mmmaaaabcdefgmmmmaaaammmaamm");

	// 8 CANDLE (third variety)
	LIGHT_STYLE(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");

	// 9 SLOW STROBE (fourth variety)
	LIGHT_STYLE(9, "aaaaaaaazzzzzzzz");

	// 10 FLUORESCENT FLICKER
	LIGHT_STYLE(10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	LIGHT_STYLE(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");

	// 12 UNDERWATER LIGHT MUTATION
	// this light only distorts the lightmap - no contribution
	// is made to the brightness of affected surfaces
	LIGHT_STYLE(12, "mmnnmmnnnmmnn");

	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	LIGHT_STYLE(63, "a");

	for ( int i = 0; i < ARRAYSIZE( g_szDecals ); i++ )
		DECAL_INDEX( i, g_szDecals[i] );
}

//
// Just to ignore the "wad" field.
//
void CWorld::KeyValue( KeyValueData *pkvd )
{
}

