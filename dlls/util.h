//
// Misc utility code
//
#ifndef ENGINECALLBACK_H
#include "enginecallback.h"
#endif


// Use this to mark stub entity position in world
#define MARK_STUB_ENTITY                                                 \
	{                                                                    \
		PRECACHE_MODEL( "progs/s_explod.spr" );                          \
		SET_MODEL( ENT(pev), "progs/s_explod.spr" );                     \
		ALERT( at_log, "Stub %s spawned!\n", STRING( pev->classname ) ); \
	}


#define ARRAYSIZE( p )					( sizeof( p ) / sizeof( p[0] ) )


// More explicit than "int"
typedef int EOFFSET;


// Keeps clutter down a bit, when using a float as a bit-vector
#define SetBits(flBitVector, bits)		((flBitVector) = (int)(flBitVector) | (bits))
#define ClearBits(flBitVector, bits)	((flBitVector) = (int)(flBitVector) & ~(bits))
#define FBitSet(flBitVector, bit)		((int)(flBitVector) & (bit))


// Makes these more explicit, and easier to find
#define FILE_GLOBAL static
#define DLL_GLOBAL


// Keeps clutter down a bit, when declaring external entity/global method prototypes
#define DECLARE_GLOBAL_METHOD(MethodName)		extern __declspec(dllexport)void MethodName( void )
#define GLOBAL_METHOD(funcname)					__declspec(dllexport) void funcname(void)


// This is the glue that hooks .MAP entity class names to our CPP classes
// The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress()
// The function is used to intialize / allocate the object for the entity
#define LINK_ENTITY_TO_CLASS(mapClassName,DLLClassName) \
	extern "C" __declspec(dllexport) void mapClassName( entvars_t *pev, void *funcArgs ); \
	void mapClassName( entvars_t *pev, void *funcArgs ) { GetClassPtr( (DLLClassName *)pev ); }



//
// Conversion among the three types of "entity", including identity-conversions.
//
#ifdef DEBUG
extern edict_t *DBG_EntOfVars(const entvars_t *pev);
inline edict_t *ENT(const entvars_t *pev)	{ return DBG_EntOfVars(pev); }
#else
inline edict_t *ENT(const entvars_t *pev)	{ return (edict_t *)pev->pContainingEntity; }
#endif
inline edict_t *ENT(edict_t *pent)			{ return pent; }
inline edict_t *ENT(EOFFSET eoffset)		{ return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset); }
inline EOFFSET OFFSET(EOFFSET eoffset)		{ return eoffset; }
inline EOFFSET OFFSET(edict_t *pent)	
{ 
#if _DEBUG
	if ( !pent )
		ALERT( at_error, "Bad ent in OFFSET()\n" );
#endif
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent); 
}
inline EOFFSET OFFSET(entvars_t *pev)				
{ 
#if _DEBUG
	if ( !pev )
		ALERT( at_error, "Bad pev in OFFSET()\n" );
#endif
	return OFFSET(ENT(pev)); 
}
inline entvars_t *VARS(entvars_t *pev)	{ return pev; }

inline entvars_t *VARS(edict_t *pent)			
{ 
	if ( !pent )
		return NULL;

	return &pent->v; 
}

inline entvars_t* VARS(EOFFSET eoffset)	{ return VARS(ENT(eoffset)); }
inline int ENTINDEX(int ent) { return (*g_engfuncs.pfnIndexOfEdict)(ent); }

// Testing the three types of "entity" for nullity
#define eoNullEntity 0
inline BOOL FNullEnt(EOFFSET eoffset)	{ return eoffset == 0; }
inline BOOL FNullEnt(edict_t* pent)		{ return pent == NULL || FNullEnt(OFFSET(pent)); }
inline BOOL FNullEnt(entvars_t* pev)	{ return pev == NULL || FNullEnt(OFFSET(pev)); }


// Testing strings for nullity
#define iStringNull 0
inline BOOL FStringNull(int iString)			{ return iString == iStringNull; }


#define cchMapNameMost 32


// Misc useful
inline BOOL FStrEq(const char*sz1, const char*sz2)
	{ return (strcmp(sz1, sz2) == 0); }
inline BOOL FClassnameIs(edict_t* pent, const char* szClassname)
	{ return FStrEq(STRING(VARS(pent)->classname), szClassname); }
inline BOOL FClassnameIs(entvars_t* pev, const char* szClassname)
	{ return FStrEq(STRING(pev->classname), szClassname); }


#define gpGlobals						( (globalvars_t *)pev->pSystemGlobals )


#define SF_LIGHT_START_OFF				( 1 << 0 )


#define SF_AMBIENT_SOUND_EVERYWHERE		( 1 << 0 )
#define SF_AMBIENT_SOUND_START_SILENT	( 1 << 1 )


extern DLL_GLOBAL Vector g_vecZero;


extern void UTIL_SetSize( entvars_t *pent, Vector &vecMin, Vector &vecMax );
extern float UTIL_VecToYaw( Vector &vec );
extern Vector UTIL_VecToAngles( Vector &vec );
extern void UTIL_MoveToOrigin( edict_t *pent, float x, float y, float z, float flDist, int iMoveType );
extern edict_t *UTIL_FindRadius( Vector &vecOrigin, float flRadius );
extern void UTIL_MakeVectors( Vector &vecAngles );
extern void UTIL_SetOrigin( entvars_t *pev, Vector &vecOrigin );
extern void UTIL_EmitAmbientSound( Vector &pos, char *samp, float vol, float attenuation );
extern void UTIL_TraceLine( Vector &vec1, Vector &vec2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr );
extern Vector UTIL_GetAimVector( edict_t *pent, float flSpeed );
extern void UTIL_ParticleEffect( Vector &org, Vector &dir, unsigned int ulColor, unsigned int ulCount );
extern float UTIL_RandomFloat( float flMin, float flMax );
extern int UTIL_RandomLong( int nMin, int nMax );
extern int UTIL_PointContents( Vector &vec );
