// john_train.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"

//
// john_train
//
class CJohnTrain : public CBaseEntity
{
public:
	virtual void Think( void *funcArgs );
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( john_train, CJohnTrain );

void CJohnTrain::Think( void *funcArgs )
{
	UTIL_MakeVectors( pev->angles );

	pev->angles.y -= 2.f;
	pev->avelocity.y = -200.f;
	pev->velocity = gpGlobals->v_forward * pev->speed;

	pev->nextthink = pev->ltime + 0.1f;
}

void CJohnTrain::Spawn()
{
	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BSP;
	SET_MODEL( edict(), STRING( pev->model ) );
	pev->nextthink = pev->ltime = 0.1f;
}

//
// train_info
//
class CTrainInfo : public CBaseEntity
{
public:
	virtual void Think( void *funcArgs );
	virtual void Touch( void *funcArgs );
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( train_info, CTrainInfo );

void CTrainInfo::Think( void *funcArgs )
{
	pev->solid = SOLID_TRIGGER;
}

void CTrainInfo::Touch( void *funcArgs )
{
	SET_MODEL( edict(), "models/scientist.mdl" );

	pev->speed = 0.f;
	pev->avelocity.y = 150.f;
}

void CTrainInfo::Spawn()
{
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	UTIL_SetSize( pev, Vector( -32, -32, -32 ), Vector( 32, 32, 32 ) );
}