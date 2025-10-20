/*

===== util.cpp ========================================================

  Utility code.  Really not optional after all.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

void UTIL_SetSize( entvars_t *pent, float minx, float miny, float minz, float maxx, float maxy, float maxz )
{
	vec3_t min, max;

	min[0] = minx;
	min[1] = miny;
	min[2] = minz;

	max[0] = maxx;
	max[1] = maxy;
	max[2] = maxz;

	SET_SIZE( ENT( pent ), min, max );
}

void UTIL_VecToYaw( float x, float y, float z )
{
	vec3_t vec;

	vec[0] = x;
	vec[1] = y;
	vec[2] = z;

	VEC_TO_YAW( vec );
}

//UTIL_VecToAngles()

void UTIL_MoveToOrigin( edict_t *pent, float x, float y, float z, float flDist, int iMoveType )
{
	vec3_t vecGoal;

	vecGoal[0] = x;
	vecGoal[1] = y;
	vecGoal[2] = z;

	MOVE_TO_ORIGIN( pent, vecGoal, flDist, iMoveType );
}

edict_t *UTIL_FindRadius( float x, float y, float z, float flRadius )
{
	vec3_t vecOrigin;

	vecOrigin[0] = x;
	vecOrigin[1] = y;
	vecOrigin[2] = z;

	return FIND_ENTITY_IN_RADIUS( vecOrigin, flRadius );
}

void UTIL_MakeVectors( float pitch, float yaw, float roll )
{
	vec3_t vecAngles;

	vecAngles[0] = pitch;
	vecAngles[1] = yaw;
	vecAngles[2] = roll;

	MAKE_VECTORS( vecAngles );
}

void UTIL_SetOrigin( entvars_t *pev, float x, float y, float z )
{
	vec3_t vecOrigin;

	vecOrigin[0] = x;
	vecOrigin[1] = y;
	vecOrigin[2] = z;

	SET_ORIGIN( ENT( pev ), vecOrigin );
}

void UTIL_EmitAmbientSound( float x, float y, float z, char *samp, float vol, float attenuation )
{
	vec3_t pos;

	pos[0] = x;
	pos[1] = y;
	pos[2] = z;

	EMIT_AMBIENT_SOUND( pos, samp, vol, attenuation );
}

void UTIL_TraceLine( float x1, float y1, float z1, float x2, float y2, float z2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr )
{
	vec3_t vec1, vec2;

	vec1[0] = x1;
	vec1[1] = y1;
	vec1[2] = z1;

	vec2[0] = x2;
	vec2[1] = y2;
	vec2[2] = z2;

	TRACE_LINE( vec1, vec2, fNoMonsters == 0, pentToSkip, ptr );
}

//UTIL_GetAimVector

void UTIL_ParticleEffect( float orgx, float orgy, float orgz, float dirx, float diry, float dirz, unsigned int ulColor, unsigned int ulCount )
{
	vec3_t vecOrigin, vecDirection;

	vecOrigin[0] = orgx;
	vecOrigin[1] = orgy;
	vecOrigin[2] = orgz;

	vecDirection[0] = dirx;
	vecDirection[1] = diry;
	vecDirection[2] = dirz;

	PARTICLE_EFFECT( vecOrigin, vecDirection, (float)ulColor, (float)ulCount );
}

float UTIL_RandomFloat( float flMin, float flMax )
{
	return (double)( rand() & 0x7FFF ) * 0.000030518509 * ( flMax - flMin ) + flMin; // TODO(SanyaSho)
}

int UTIL_RandomLong( int nMin, int nMax )
{
	return nMin + rand() % ( nMax - nMin + 1 ); // TODO(SanyaSho)
}

int UTIL_PointContents( float x, float y, float z )
{
	vec3_t vec;

	vec[0] = x;
	vec[1] = y;
	vec[2] = z;

	return POINT_CONTENTS( vec );
}

