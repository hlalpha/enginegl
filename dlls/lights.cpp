/*

===== lights.cpp ========================================================

  spawn and think functions for editor-placed lights

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

class CLight : public CBaseEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	void Use( void *funcArg );

private:
	int m_iStyle;
};
LINK_ENTITY_TO_CLASS( light, CLight );

//
// Cache user-entity-field values until spawn is called.
//
void CLight::KeyValue( KeyValueData *pkvd )
{
	if ( !strcmp( pkvd->szKeyName, "style" ) )
	{
		m_iStyle = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( !strcmp( pkvd->szKeyName, "pitch" ) )
	{
		pkvd->fHandled = TRUE;
	}
}

/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) LIGHT_START_OFF
Non-displayed light.
Default light value is 300
Default style is 0
If targeted, it will toggle between on or off.
*/
void CLight::Spawn()
{
	if ( pev->targetname == 0 )
	{
		// inert light
		REMOVE_ENTITY( (edict_t *)pev->pContainingEntity );
		return;
	}

	if ( m_iStyle >= 32 )
	{
		//CHANGE_METHOD(ENT(pev), em_use, light_use);
		if ( FBitSet( pev->spawnflags, SF_LIGHT_START_OFF ) )
			LIGHT_STYLE( m_iStyle, "a" );
		else
			LIGHT_STYLE( m_iStyle, "m" );
	}
}

void CLight::Use( void *funcArg )
{
	if ( m_iStyle >= 32 )
	{
		if ( FBitSet( pev->spawnflags, SF_LIGHT_START_OFF ) )
		{
			LIGHT_STYLE( m_iStyle, "m" );
			ClearBits( pev->spawnflags, SF_LIGHT_START_OFF );
		}
		else
		{
			LIGHT_STYLE( m_iStyle, "a" );
			SetBits( pev->spawnflags, SF_LIGHT_START_OFF );
		}
	}
}

//
// shut up spawn functions for new spotlights
//
LINK_ENTITY_TO_CLASS( light_spot, CLight );

