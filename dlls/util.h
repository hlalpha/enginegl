//
// Misc utility code
//
#ifndef ENGINECALLBACK_H
#include "enginecallback.h"
#endif


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


#define VARS( ed )	(*g_engfuncs.pfnGetVarsOfEnt)( ed )
#define ENT( ed )	( ed )


#define SF_LIGHT_START_OFF				1

