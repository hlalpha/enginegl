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


#define VARS( ed )						(*g_engfuncs.pfnGetVarsOfEnt)( ed )
#define ENT( ed )						( (edict_t *)ed->pContainingEntity )


#define FStringNull( string )			( string == 0 )
#define FStrEq( a, b )					( !strcmp( a, b ) )


#define gpGlobals						( (globalvars_t *)pev->pSystemGlobals )


#define SF_LIGHT_START_OFF				( 1 << 0 )


#define SF_AMBIENT_SOUND_EVERYWHERE		( 1 << 0 )
#define SF_AMBIENT_SOUND_START_SILENT	( 1 << 1 )


extern void UTIL_SetSize( entvars_t *pent, Vector &vecMin, Vector &vecMax );
extern void UTIL_VecToYaw( Vector &vec );
//extern UTIL_VecToAngles();
extern void UTIL_MoveToOrigin( edict_t *pent, float x, float y, float z, float flDist, int iMoveType );
extern edict_t *UTIL_FindRadius( Vector &vecOrigin, float flRadius );
extern void UTIL_MakeVectors( Vector &vecAngles );
extern void UTIL_SetOrigin( entvars_t *pev, Vector &vecOrigin );
extern void UTIL_EmitAmbientSound( Vector &pos, char *samp, float vol, float attenuation );
extern void UTIL_TraceLine( Vector &vec1, Vector &vec2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr );
//extern UTIL_GetAimVector;
extern void UTIL_ParticleEffect( Vector &org, Vector &dir, unsigned int ulColor, unsigned int ulCount );
extern float UTIL_RandomFloat( float flMin, float flMax );
extern int UTIL_RandomLong( int nMin, int nMax );
extern int UTIL_PointContents( Vector &vec );
