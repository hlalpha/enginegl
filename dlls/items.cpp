// items.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "items.h"

LINK_ENTITY_TO_CLASS( item, CItem );

void CItem::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "type" ) )
	{
		m_nItem = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
}

void CItem::ThrowShell( Vector &vecOrigin )
{
	pev->movetype = MOVETYPE_TOSS;

	pev->classname = ALLOC_STRING( "Item" );

	pev->solid = SOLID_BBOX;

	SET_MODEL( ENT( pev ), "models/shell.mdl" );

	UTIL_SetSize( pev, Vector( -16, -16, 0 ), Vector( 16, 16, 16 ) );

	pev->origin = vecOrigin;

	//SetTouch( &??? ); // TODO(SanyaSho)
}

void CItem::Use( void *funcArgs )
{
	/* Nothing */
}

void CItem::Touch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	if ( !FClassnameIs( ENT( gpGlobals->other ), "player" ) )
		return;

	// SDKTODO(SanyaSho)
	/*if ( m_nItem >= MAX_ITEMS )
		pevToucher->items |= (1 << ( m_nItem - MAX_ITEMS ));
	else
		pevToucher->weapons |= (1 << m_nItem);*/

	SetThink( &CBaseEntity::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1f;
}

