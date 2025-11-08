// lights.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"

class CLight : public CBaseEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Use( void *funcArg );

private:
	int m_iStyle;
};

LINK_ENTITY_TO_CLASS( light, CLight );

void CLight::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "style" ) )
	{
		m_iStyle = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}

	if ( FStrEq( pkvd->szKeyName, "pitch" ) )
	{
		pkvd->fHandled = TRUE;
	}
}

void CLight::Spawn()
{
	if ( FStringNull( pev->targetname ) )
	{
		REMOVE_ENTITY( edict() );
		return;
	}

	if ( m_iStyle >= 32 )
	{
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

LINK_ENTITY_TO_CLASS( light_spot, CLight );

