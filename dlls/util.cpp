/*

===== util.cpp ========================================================

  Utility code.  Really not optional after all.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

void UTIL_EmitAmbientSound( float x, float y, float z, char *samp, float vol, float attenuation )
{
	vec3_t pos;

	pos[0] = x;
	pos[1] = y;
	pos[2] = z;

	EMIT_AMBIENT_SOUND( pos, samp, vol, attenuation );
}

float UTIL_RandomFloat( float flMin, float flMax )
{
	return (double)( rand() & 0x7FFF ) * 0.000030518509 * ( flMax - flMin ) + flMin; // TODO(SanyaSho)
}

void _VectorCopy( vec_t *to, vec_t *from )
{
	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
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

