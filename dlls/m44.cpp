// m44.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"

class CM44 : public CBaseEntity
{
public:
	void Spawn();
};

LINK_ENTITY_TO_CLASS( monster_m44, CM44 );

void CM44::Spawn()
{
	PRECACHE_MODEL( "models/m44.mdl" );
	SET_MODEL( ENT( pev ), "models/m44.mdl" );

	pev->solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_STEP;
	
	pev->takedamage = DAMAGE_NO;

	pev->effects = 0;

	pev->health = 80;

	UTIL_SetSize( pev, Vector( -16, -16, 0 ), Vector( 16, 16, 32 ) );

	pev->sequence = 0;
	pev->frame = 0;

	pev->nextthink += UTIL_RandomFloat( 0.0, 0.5 );
}
