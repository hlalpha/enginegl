// cbase.h
#if !defined( CBASE_H )
#define CBASE_H

class CBaseEntity;
class CBaseToggle;
class CBaseMonster;

#define SetThink( a )		m_pfnThink		= static_cast <void (CBaseEntity:: *)( void *funcArgs )>( a )
#define SetTouch( a )		m_pfnTouch		= static_cast <void (CBaseEntity:: *)( void *funcArgs )>( a )
#define SetUse( a )			m_pfnUse		= static_cast <void (CBaseEntity:: *)( void *funcArgs )>( a )
#define SetBlocked( a )		m_pfnBlocked	= static_cast <void (CBaseEntity:: *)( void *funcArgs )>( a )

class CBaseEntity
{
public:
	CBaseEntity();

	virtual void Spawn();
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void UNKNOWN( void *funcArgs );
	virtual void Save( void *funcArgs );
	virtual void Restore( void *funcArgs );
	virtual void Think( void *funcArgs );
	virtual void Touch( void *funcArgs );
	virtual void Use( void *funcArgs );
	virtual void Blocked( void *funcArgs );

	void SUB_Remove( void *funcArgs );
	void SUB_DoNothing( void *funcArgs );
	void SUB_CallUseToggle( void *funcArgs );
	void SUB_CallTouchToggle( void *funcArgs );

	void *operator new( size_t stAllocateBlock, entvars_t *pev ) { return (void *)ALLOC_PRIVATE( (edict_t *)pev->pContainingEntity, stAllocateBlock ); }
	void operator delete( void *pMem, entvars_t *pev ) {}

	static CBaseEntity *Instance( edict_t *pent ) { if ( !pent ) pent = ENT( 0 ); CBaseEntity *pEnt = (CBaseEntity *)GET_PRIVATE( pent ); return pEnt; }
	static CBaseEntity *Instance( entvars_t *pev ) { return Instance( ENT( pev ) ); }
	static CBaseEntity *Instance( EOFFSET eoffset ) { return Instance( ENT( eoffset ) ); }

	edict_t *edict() { return ENT( pev ); };
	EOFFSET eoffset() { return OFFSET( pev ); };

	entvars_t *pev;
	globalvars_t *globals;

	void (CBaseEntity:: *m_pfnThink)( void *funcArgs );
	void (CBaseEntity:: *m_pfnTouch)( void *funcArgs );
	void (CBaseEntity:: *m_pfnUse)( void *funcArgs );
	void (CBaseEntity:: *m_pfnBlocked)( void *funcArgs );
};

//
// Converts a entvars_t * to a class pointer
// It will allocate the class and entity if necessary
//
template <class T> T * GetClassPtr( T *a )
{
	entvars_t *pev = (entvars_t *)a;

	// allocate entity if necessary
	if (pev == NULL)
		pev = VARS(CREATE_ENTITY());

	// get the private data
	a = (T *)GET_PRIVATE(ENT(pev));

	if (a == NULL)
	{
		// allocate private data
		a = new(pev) T;
		a->pev = pev;
		a->globals = (globalvars_t *)pev->pSystemGlobals;
	}
	return a;
}

class CPointEntity : public CBaseEntity
{
public:
};

class CMultiSource : public CPointEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Use( void *funcArgs );
	void sub_10015810( void *funcArgs );
	void Register( void *funcArgs );

public: // private:
	int m_iUnknown; // SDKTODO
	int m_iTotal;
};

class CBaseDelay : public CBaseEntity
{
public:
	float m_flDelay;
	char *m_iszKillTarget;

	void SUB_UseTargets( void *funcArgs );
	void DelayThink( void *funcArgs );
};

enum TOGGLE_STATE
{
	TS_AT_TOP = 0,
	TS_AT_BOTTOM,
	TS_GOING_UP,
	TS_GOING_DOWN,
};

#define SetMoveDone( a )		m_pfnCallWhenMoveDone		= static_cast <void (CBaseToggle:: *)( void *funcArgs )>( a )

class CBaseToggle : public CBaseDelay
{
public:
	void LinearMove( Vector vecDest, float flSpeed );
	void LinearMoveDone( void *funcArgs );
	void AngularMove( Vector vecDestAngle, float flSpeed );
	void AngularMoveDone( void *funcArgs );

	static void AxisDir( entvars_t *pev );
	static float AxisDelta( int flags, Vector &angle1, Vector &angle2 );

public: // protected:
	TOGGLE_STATE m_toggle_state;

	float m_flActivateFinished;

	float m_flMoveDistance;
	float m_flWait;
	float m_flLip;

	float m_flTWidth;
	float m_flTLength;

	Vector m_vecPosition1;
	Vector m_vecPosition2;

	int m_cTriggersLeft;
	float m_flHeight;

	int m_eoActivator;

	void (CBaseToggle:: *m_pfnCallWhenMoveDone)( void *funcArgs );

	Vector m_vecFinalDest;
	Vector m_vecFinalAngle;
};

#include "basemonster.h"

char *ButtonSound( int sound );

class CBaseButton : public CBaseMonster
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Pain( float flDamage );
	virtual void Spawn();

	void ButtonSpark( void *funcArgs );
	void ButtonUse( void *funcArgs );
	void ButtonTouch( void *funcArgs );
	void ButtonActivate();
	void TriggerAndWait( void *funcArgs );
	void ButtonReturn( void *funcArgs );
	void ButtonBackHome( void *funcArgs );

protected:
	BOOL m_fStayPushed;
	BOOL m_fRotating;
	string_t m_iszMaster;
};

// this moved here from world.cpp, to allow classes to be derived from it
//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================
class CWorld : public CBaseEntity
{
public:
	void Spawn();
	void KeyValue( KeyValueData *pkvd );
};

#endif // CBASE_H

