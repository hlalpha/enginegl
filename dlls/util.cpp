/*

===== util.cpp ========================================================

  Utility code.  Really not optional after all.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

void UTIL_SetSize( entvars_t *pent, Vector &vecMin, Vector &vecMax )
{
	SET_SIZE( ENT( pent ), vecMin, vecMax );
}

void UTIL_VecToYaw( Vector &vec )
{
	VEC_TO_YAW( vec );
}

Vector UTIL_VecToAngles( Vector &vec )
{
	float rgflVecOut[3];
	VEC_TO_ANGLES( vec, rgflVecOut );
	return rgflVecOut;
}

void UTIL_MoveToOrigin( edict_t *pent, float x, float y, float z, float flDist, int iMoveType )
{
	vec3_t vecGoal;

	vecGoal[0] = x;
	vecGoal[1] = y;
	vecGoal[2] = z;

	MOVE_TO_ORIGIN( pent, vecGoal, flDist, iMoveType );
}

edict_t *UTIL_FindRadius( Vector &vecOrigin, float flRadius )
{
	return FIND_ENTITY_IN_RADIUS( vecOrigin, flRadius );
}

void UTIL_MakeVectors( Vector &vecAngles )
{
	MAKE_VECTORS( vecAngles );
}

void UTIL_SetOrigin( entvars_t *pev, Vector &vecOrigin )
{
	SET_ORIGIN( ENT( pev ), vecOrigin );
}

void UTIL_EmitAmbientSound( Vector &pos, char *samp, float vol, float attenuation )
{
	EMIT_AMBIENT_SOUND( pos, samp, vol, attenuation );
}

void UTIL_TraceLine( Vector &vec1, Vector &vec2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr )
{
	TRACE_LINE( vec1, vec2, fNoMonsters == 0, pentToSkip, ptr );
}

Vector UTIL_GetAimVector( edict_t *pent, float flSpeed )
{
	float tmp[3];
	GET_AIM_VECTOR( pent, flSpeed, tmp );
	return tmp;
}

void UTIL_ParticleEffect( Vector &org, Vector &dir, unsigned int ulColor, unsigned int ulCount )
{
	PARTICLE_EFFECT( org, dir, (float)ulColor, (float)ulCount );
}

float UTIL_RandomFloat( float flMin, float flMax )
{
	return (double)( rand() & 0x7FFF ) * 0.000030518509 * ( flMax - flMin ) + flMin; // TODO(SanyaSho)
}

int UTIL_RandomLong( int nMin, int nMax )
{
	return nMin + rand() % ( nMax - nMin + 1 ); // TODO(SanyaSho)
}

int UTIL_PointContents( Vector &vec )
{
	return POINT_CONTENTS( vec );
}

