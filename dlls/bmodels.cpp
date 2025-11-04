// bmodels.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "enginecallback.h"
#include "basemonster.h"


//
// BModelOrigin - calculates origin of a bmodel from absmin/size because all bmodel origins are 0 0 0
//
Vector VecBModelOrigin( entvars_t *pevBModel )
{
	return pevBModel->absmin + ( pevBModel->size * 0.5 );
}


//
// func_wall
//
class CFuncWall : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Use( void *funcArgs );
};

LINK_ENTITY_TO_CLASS( func_wall, CFuncWall );

void CFuncWall::Spawn()
{
	pev->angles = g_vecZero;

	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BSP;

	SET_MODEL( edict(), STRING( pev->model ) );
}

void CFuncWall::Use( void *funcArgs )
{
	pev->frame -= 1;
}

//
// func_illusionary
//
class CFuncIllusionary : public CBaseToggle
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
};

LINK_ENTITY_TO_CLASS( func_illusionary, CFuncIllusionary );

void CFuncIllusionary::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "skin" ) )
	{
		pev->skin = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
}

void CFuncIllusionary::Spawn()
{
	pev->angles = g_vecZero;

	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	SET_MODEL( edict(), STRING( pev->model ) );

	MAKE_STATIC( edict() );
}

//
// func_glass
//
class CFuncGlass : public CBaseMonster
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Use( void *funcArgs );
	virtual void Touch( void *funcArgs );

	virtual void Die();

public:
	int m_nBreakModel;
};

LINK_ENTITY_TO_CLASS( func_glass, CFuncGlass );

void CFuncGlass::KeyValue( KeyValueData *pkvd )
{
	/* Nothing */
}

void CFuncGlass::Spawn()
{
	PRECACHE_SOUND( "common/glass.wav" );

	m_nBreakModel = PRECACHE_MODEL( "sprites/shard.spr" );

	if ( !FBitSet( pev->spawnflags, 1 ) )
		pev->takedamage = DAMAGE_YES;
	else
		pev->takedamage = DAMAGE_NO;

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	SET_MODEL( edict(), STRING( pev->model ) );
}

void CFuncGlass::Use( void *funcArgs )
{
	Die( /*funcArgs*/ ); // FIXME
}

void CFuncGlass::Touch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	// Not a player
	if ( !FBitSet( pevToucher->flags, FL_CLIENT ) )
		return;

	// Can't be killed
	if ( pev->takedamage == DAMAGE_NO )
		return;

	// Kill the thing
	if ( ( pevToucher->velocity.Length() * 0.1 ) > pev->health )
		TakeDamage( pevToucher, pevToucher, pev->health );
}

void CFuncGlass::Die()
{
	EMIT_SOUND_DYN2( edict(), 2, "common/glass.wav", 1.0, 0.8 );

	UTIL_MakeVectors( pev->angles );

	WRITE_BYTE( MSG_ONE, 23 );											// msg
	WRITE_BYTE( MSG_ONE, TE_BREAKMODEL );								// teid
	WRITE_COORD( MSG_ONE, ( pev->mins[0] + pev->maxs[0] ) * 0.5f );		// x
	WRITE_COORD( MSG_ONE, ( pev->mins[1] + pev->maxs[1] ) * 0.5f );		// y
	WRITE_COORD( MSG_ONE, ( pev->mins[2] + pev->maxs[2] ) * 0.5f );		// z
	WRITE_COORD( MSG_ONE, pev->size[0] );								// sizex
	WRITE_COORD( MSG_ONE, pev->size[1] );								// sizey
	WRITE_COORD( MSG_ONE, pev->size[2] );								// sizez
	WRITE_COORD( MSG_ONE, 1 );											// pitch
	WRITE_COORD( MSG_ONE, 1 );											// yaw
	WRITE_COORD( MSG_ONE, 1 );											// roll
	WRITE_SHORT( MSG_ONE, m_nBreakModel );								// modelindex
	WRITE_BYTE( MSG_ONE, 12 );											// count
	WRITE_BYTE( MSG_ONE, 15 );											// lifetime * 0.1
	WRITE_BYTE( MSG_ONE, BREAK_GLASS );									// flags

	// Make entities fall thru the glass
	pev->solid = SOLID_NOT;
	pev->model = 0;
	
	// Kill the thing
	SetThink( &CBaseEntity::SUB_Remove );
	pev->nextthink = pev->ltime;
}

//
// func_rotating
//
class CFuncRotating : public CBaseEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Touch( void *funcArgs );
	void SpinUp( void *funcArgs );
	void SpinDown( void *funcArgs );
	virtual void Use( void *funcArgs );
	virtual void Blocked( void *funcArgs );

public:
	BYTE m_fFriction;
};

LINK_ENTITY_TO_CLASS( func_rotating, CFuncRotating );

void CFuncRotating::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "friction" ) )
	{
		m_fFriction = (BYTE)atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
}

void CFuncRotating::Spawn()
{
	switch ( (int)pev->sounds )
	{
	case 1:
		PRECACHE_SOUND( "fans/fan1on.wav" );
		PRECACHE_SOUND( "fans/fan1off.wav" );
		PRECACHE_SOUND( "fans/fan1.wav" );

		pev->noise1 = ALLOC_STRING( "fans/fan1on.wav" );
		pev->noise2 = ALLOC_STRING( "fans/fan1off.wav" );
		pev->noise3 = ALLOC_STRING( "fans/fan1.wav" );
		break;
	case 2:
		PRECACHE_SOUND( "fans/fan2on.wav" );
		PRECACHE_SOUND( "fans/fan2off.wav" );
		PRECACHE_SOUND( "fans/fan2.wav" );

		pev->noise1 = ALLOC_STRING( "fans/fan2on.wav" );
		pev->noise2 = ALLOC_STRING( "fans/fan2off.wav" );
		pev->noise3 = ALLOC_STRING( "fans/fan2.wav" );
		break;
	case 3:
		PRECACHE_SOUND( "fans/fan3on.wav" );
		PRECACHE_SOUND( "fans/fan3off.wav" );
		PRECACHE_SOUND( "fans/fan3.wav" );

		pev->noise1 = ALLOC_STRING( "fans/fan3on.wav" );
		pev->noise2 = ALLOC_STRING( "fans/fan3off.wav" );
		pev->noise3 = ALLOC_STRING( "fans/fan3.wav" );
		break;
	case 4:
		PRECACHE_SOUND( "fans/fan4on.wav" );
		PRECACHE_SOUND( "fans/fan4off.wav" );
		PRECACHE_SOUND( "fans/fan4.wav" );

		pev->noise1 = ALLOC_STRING( "fans/fan4on.wav" );
		pev->noise2 = ALLOC_STRING( "fans/fan4off.wav" );
		pev->noise3 = ALLOC_STRING( "fans/fan4.wav" );
		break;
	case 5:
		PRECACHE_SOUND( "fans/fan5on.wav" );
		PRECACHE_SOUND( "fans/fan5off.wav" );
		PRECACHE_SOUND( "fans/fan5.wav" );

		pev->noise1 = ALLOC_STRING( "fans/fan5on.wav" );
		pev->noise2 = ALLOC_STRING( "fans/fan5off.wav" );
		pev->noise3 = ALLOC_STRING( "fans/fan5.wav" );
		break;
	default:
		PRECACHE_SOUND( "common/null.wav" );

		pev->noise1 = ALLOC_STRING( "common/null.wav" );
		pev->noise2 = ALLOC_STRING( "common/null.wav" );
		pev->noise3 = ALLOC_STRING( "common/null.wav" );
		break;
	}

	if ( m_fFriction == 0 )
		m_fFriction = 1;

	if ( FBitSet( pev->spawnflags, 4 ) )
		pev->movedir = Vector( 0, 0, 1 );
	else if ( FBitSet( pev->spawnflags, 8 ) )
		pev->movedir = Vector( 1, 0, 0 );
	else
		pev->movedir = Vector( 0, 1, 0 );

	if ( FBitSet( pev->spawnflags, 2 ) )
		pev->movedir = -pev->movedir;

	if ( FBitSet( pev->spawnflags, 0x40 ) )
	{
		pev->solid = SOLID_NOT;
		pev->skin = -1;
	}
	else
	{
		pev->solid = SOLID_BSP;
	}

	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );

	// SDKTODO(SanyaSho)
	//if ( pev->speed <= 0 )
	//	pev->speed = 0;

	//if ( pev->dmg == 0 )
	//	pev->dmg = 2;

	if ( FBitSet( pev->spawnflags, 1 ) )
	{
		SetThink( &CFuncRotating::SUB_CallUseToggle );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	if ( FBitSet( pev->spawnflags, 0x20 ) )
	{
		SetTouch( &CFuncRotating::SUB_CallTouchToggle );
	}
}

void CFuncRotating::Touch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	if ( pevToucher->takedamage == DAMAGE_NO )
		return;

	/*pev->dmg*/ float dmg = pev->avelocity.Length() * 0.1; // SDKTODO(SanyaSho)

	CBaseMonster *pMonster = GetClassPtr( (CBaseMonster *)pevToucher );
	//pMonster->TakeDamage( pev, pev, pev->dmg ); // SDKTODO(SanyaSho)
	pMonster->TakeDamage( pev, pev, dmg );

	pevToucher->velocity = ( pevToucher->origin - VecBModelOrigin( pev ) ).Normalize() * /*pev->dmg*/ dmg; // SDKTODO(SanyaSho)
}

void CFuncRotating::SpinUp( void *funcArgs )
{
	pev->nextthink = pev->ltime + 0.1;

	pev->avelocity = pev->avelocity + ( pev->movedir * ( /*pev->speed*/ 10.f * m_fFriction ) ); // SDKTODO(SanyaSho)

	// SDKTODO(SanyaSho)
	//if ( ( pev->movedir[0] * pev->speed ) <= pev->avelocity[0] && ( pev->movedir[1] * pev->speed ) <= pev->avelocity[1] && ( pev->movedir[2] * pev->speed ) <= pev->avelocity[2] )
	if ( ( pev->movedir[0] * 10.f ) <= pev->avelocity[0] && ( pev->movedir[1] * 10.f ) <= pev->avelocity[1] && ( pev->movedir[2] * 10.f ) <= pev->avelocity[2] )
	{
		// SDKTODO(SanyaSho)
		//pev->avelocity = pev->movedir * pev->speed;
		pev->avelocity = pev->movedir * 10.f;

		EMIT_SOUND_DYN2( edict(), 1, STRING( pev->noise3 ), 1.0, 0.8 );
		pev->nextthink = pev->ltime + 99999;
	}
}

void CFuncRotating::SpinDown( void *funcArgs )
{
	pev->nextthink = pev->ltime + 0.1;

	pev->avelocity = pev->avelocity - ( pev->movedir * ( /*pev->speed*/ 10.f * m_fFriction ) ); // SDKTODO(SanyaSho)

	if ( pev->avelocity.x <= 0 && pev->avelocity.y <= 0 && pev->avelocity.z <= 0 )
	{
		pev->avelocity = g_vecZero;
		pev->nextthink = pev->ltime + 99999;
	}
}

void CFuncRotating::Use( void *funcArgs )
{
	char *szNoise;
	if ( FBitSet( pev->spawnflags, 0x10 ) )
	{
		if ( pev->avelocity != g_vecZero )
		{
			SetThink( &CFuncRotating::SpinDown );
			szNoise = STRING( pev->noise2 );
		}
		else
		{
			SetThink( &CFuncRotating::SpinUp );
			szNoise = STRING( pev->noise1 );
		}

		EMIT_SOUND_DYN2( edict(), 1, szNoise, 1.0, 0.8 );
		pev->nextthink = pev->ltime + 0.1;
	}
	else
	{
		if ( pev->avelocity != g_vecZero )
		{
			pev->avelocity = g_vecZero;
		}
		else
		{
			EMIT_SOUND_DYN2( edict(), 1, STRING( pev->noise3 ), 1.0, 0.8 );

			//pev->avelocity = pev->movedir * pev->speed; // SDKTODO(SanyaSho)
			pev->avelocity = pev->movedir * 10.f;
			pev->nextthink = gpGlobals->time + 99999;
		}
	}
}

void CFuncRotating::Blocked( void *funcArgs )
{
	CBaseMonster *pBlocker = GetClassPtr( (CBaseMonster *)ENT( gpGlobals->other ) );
	//pBlocker->TakeDamage( pev, pev, pev->dmg ); // SDKTODO(SanyaSho)
	pBlocker->TakeDamage( pev, pev, 10.f );
}

//
// func_pushable
//
class CPushable : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Touch( void *funcArgs );

private:
	float m_maxSpeed;
};

LINK_ENTITY_TO_CLASS( func_pushable, CPushable );

void CPushable::Spawn()
{
	pev->movetype = MOVETYPE_PUSHSTEP;
	pev->solid = SOLID_BBOX;

	SET_MODEL( edict(), STRING( pev->model ) );

	// SDKTODO(SanyaSho)
	//if ( pev->friction > 399.f )
	//	pev->friction = 399.f;

	//m_maxSpeed = pev->friction - 400;
	//pev->friction = 0;

	m_maxSpeed = 1000.f;
}

void CPushable::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "size" ) )
	{
		int nSize = (int)atof( pkvd->szValue );
		pkvd->fHandled = TRUE;

		switch ( nSize )
		{
		case 0:
			UTIL_SetSize( pev, Vector( -8, -8, -8 ), Vector( 8, 8, 8 ) );
			break;
		case 2:
			UTIL_SetSize( pev, Vector( -16, -16, -18 ) * 2, Vector( 16, 16, 18 ) * 2 );
			break;
		case 3:
			UTIL_SetSize( pev, Vector( -16, -16, -18 ), Vector( 16, 16, 18 ) );
			break;
		default:
		case 1:
			UTIL_SetSize( pev, Vector( -16, -16, -36 ), Vector( 16, 16, 36 ) );
			break;
		};
	}
}

void CPushable::Touch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	// Not a player
	if ( !FClassnameIs( pevToucher, "player" ) )
		return;

	// Don't move if the player is standing on us
	if ( FBitSet( pevToucher->flags, FL_ONGROUND ) && VARS( ENT( pevToucher->groundentity ) ) == pev )
		return;

	pev->velocity.x += pevToucher->velocity.x * 0.5;
	pev->velocity.y += pevToucher->velocity.y * 0.5;

	float flLength = sqrt( pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y );
	if ( flLength > m_maxSpeed )
	{
		pev->velocity.x = ( pev->velocity.x * m_maxSpeed / flLength );
		pev->velocity.y = ( pev->velocity.y * m_maxSpeed / flLength );
	}
}

//
// func_pendulum
//
class CPendulum : public CBaseEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Use( void *funcArgs );
	void Stop( void *funcArgs );
	void Swing( void *funcArgs );
	virtual void Touch( void *funcArgs );
	void RopeTouch( void *funcArgs );

private:
	float m_accel;
	float m_distance;
	float m_time;
	float m_damp;
	float m_maxSpeed;
	float m_dampSpeed;
	Vector m_center;
	Vector m_start;
};

LINK_ENTITY_TO_CLASS( func_pendulum, CPendulum );

void CPendulum::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "distance" ) )
	{
		m_distance = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "damp" ) )
	{
		m_damp = atof( pkvd->szValue ) * 0.001;
		pkvd->fHandled = TRUE;
	}
}

void CPendulum::Spawn()
{
	CBaseToggle::AxisDir( pev );

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );

	SET_MODEL( edict(), STRING( pev->model ) );

	if ( m_distance == 0 )
		return;

	// SDKTODO(SanyaSho)
	//if ( pev->speed == 0 )
	//	pev->speed = 100;

	//m_accel = ( pev->speed * pev->speed ) / ( m_distance * 2.0 );
	//m_maxSpeed = pev->speed;
	m_start = pev->angles;
	m_center = pev->movedir * ( m_distance * 0.5 ) + pev->angles;

	if ( FBitSet( pev->spawnflags, 1 ) )
	{
		SetThink( &CPendulum::SUB_CallUseToggle );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	//pev->speed = 0;

	if ( FBitSet( pev->spawnflags, 2 ) )
	{
		SetTouch( &CPendulum::RopeTouch );
	}
}

void CPendulum::Use( void *funcArgs )
{
	// SDKTODO(SanyaSho)
	//if ( pev->speed == 0 )
	//{
	//	pev->nextthink = pev->ltime + 0.1;
	//	m_time = pev->ltime;
	//	m_dampSpeed = m_maxSpeed;
	//	SetThink( &CPendulum::Think );
	//	return;
	//}

	if ( FBitSet( pev->spawnflags, 0x10 ) )
	{
		float dt = CBaseToggle::AxisDelta( pev->spawnflags, pev->angles, m_start );

		pev->avelocity = pev->movedir * m_maxSpeed;

		pev->nextthink = pev->ltime + ( dt / m_maxSpeed );
		SetThink( &CPendulum::Stop );
	}
	else
	{
		//pev->speed = 0; // SDKTODO(SanyaSho)
		SetThink( NULL );
		pev->avelocity = g_vecZero;
	}
}

void CPendulum::Stop( void *funcArgs )
{
	pev->angles = m_start;
	//pev->speed = 0; // SDKTODO(SanyaSho)

	SetThink( NULL );
	pev->avelocity = g_vecZero;
}

void CPendulum::Swing( void *funcArgs )
{
	float delta = CBaseToggle ::AxisDelta( pev->spawnflags, pev->angles, m_center );
	float dt = gpGlobals->time - m_time;
	m_time = gpGlobals->time;

	// SDKTODO(SanyaSho)
	//if ( delta > 0 && m_accel > 0 )
	//	pev->speed -= m_accel * dt;
	//else
	//	pev->speed += m_accel * dt;

	//if ( pev->speed > m_maxSpeed )
	//	pev->speed = m_maxSpeed;
	//else if ( pev->speed < -m_maxSpeed )
	//	pev->speed = -m_maxSpeed;

	//pev->avelocity = pev->speed * pev->movedir;

	pev->nextthink = pev->ltime + 0.1;

	if ( m_damp )
	{
		m_dampSpeed -= m_damp * m_dampSpeed * dt;
		if ( m_dampSpeed < 30.0 )
		{
			pev->angles = m_center;
			//pev->speed = 0; // SDKTODO(SanyaSho)

			SetThink( NULL );
			pev->avelocity = Vector( 0, 0, 0 );
		}
		// SDKTODO(SanyaSho)
		//else if ( pev->speed > m_dampSpeed )
		//	pev->speed = m_dampSpeed;
		//else if ( pev->speed < -m_dampSpeed )
		//	pev->speed = -m_dampSpeed;
	}
}

void CPendulum::Touch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );

	// SDKTODO(SanyaSho)
	//if ( pev->dmg <= 0 )
	//	return;

	if ( pevToucher->takedamage == DAMAGE_NO )
		return;

	// SDKTODO(SanyaSho)
	//float flDamage = pev->speed * pev->dmg * 0.01;
	float flDamage = 10.f;
	if ( flDamage < 0 )
		flDamage = -flDamage;

	CBaseMonster *pMonster = GetClassPtr( (CBaseMonster *)pevToucher );
	pMonster->TakeDamage( pev, pev, flDamage );

	pevToucher->velocity = ( pevToucher->origin - VecBModelOrigin( pev ) ).Normalize() * flDamage;
}

void CPendulum::RopeTouch( void *funcArgs )
{
	entvars_t *pevToucher = VARS( ENT( gpGlobals->other ) );
	
	// Must be a client
	if ( !FBitSet( pevToucher->flags, FL_CLIENT ) )
	{
		ALERT( at_log, "Not a client\n" );
		return;
	}

	// Keep this player with us
	if ( pev->enemy != gpGlobals->other )
	{
		pev->enemy = gpGlobals->other;

		pevToucher->velocity = g_vecZero;
		pevToucher->movetype = MOVETYPE_NONE;
	}
}
