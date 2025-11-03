#if !defined( CBASE_H )
#define CBASE_H

// TODO(SanyaSho): Move this somewhere!
#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}
#define VectorLerp(a, b, c, t) {c[0] = a[0] + (b[0] - a[0]) * t;c[1] = a[1] + (b[1] - a[1]) * t;c[2] = a[2] + (b[2] - a[2]) * t;}

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

	void *operator new( size_t stAllocateBlock, entvars_t *pev ) { return (void *)ALLOC_PRIVATE( (edict_t *)pev->pContainingEntity, stAllocateBlock ); }
	void operator delete( void *pMem, entvars_t *pev ) {}

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

class CBaseDelay : public CBaseEntity
{
public:
	float m_flDelay;
	char *m_iszKillTarget;

	void SUB_UseTargets( void *funcArgs );
	void DelayThink( void *funcArgs );
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

