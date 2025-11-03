// monsters.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "../engine/studio.h"

int CBaseMonster::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage )
{
	// SDKTODO(SanyaSho)
	return 0;
}

void CBaseMonster::SetDeathType( int iDeathType )
{
	switch ( iDeathType )
	{
	case 0:
		m_nActivity = 35;
		break;
	case 1:
		m_nActivity = 36;
		break;
	case 2:
		m_nActivity = 37;
		break;
	case 3:
		m_nActivity = 38;
		break;
	case 4:
		m_nActivity = 39;
		break;
	default:
		ALERT( at_log, "Unknown death type!\n" );
		break;
	}

	pev->ideal_yaw = pev->angles[1];

	SetActivity( m_nActivity );

	SetThink( &CBaseMonster::ChangeStateThink );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CBaseMonster::ChangeStateThink( void *funcArgs )
{
	// SDKTODO(SanyaSho)
}

void GetSequenceInfo( void *pmodel, entvars_t *pev, float *pflFrameRate, float *pflGroundSpeed )
{
	studiohdr_t			*pstudiohdr;
	mstudioseqdesc_t	*pseqdesc;

	pstudiohdr = (studiohdr_t *)pmodel;
	if ( !pstudiohdr )
		return;

	if ( pev->sequence >= pstudiohdr->numseq )
	{
		*pflFrameRate = 0.0;
		*pflGroundSpeed = 0.0;
		return;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	if ( pseqdesc->numframes > 1 )
	{
		*pflFrameRate = 256 * pseqdesc->fps / ( pseqdesc->numframes - 1 );
		*pflGroundSpeed = pseqdesc->linearmovement.Length();
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / ( pseqdesc->numframes - 1 );
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}

// SDKTODO(SanyaSho): Find a better place for these!
void CBaseMonster::StudioFrameAdvance( float flInterval )
{
	if ( pev->animtime )
		pev->frame = ( globals->time - pev->animtime ) * pev->framerate * m_flFrameRate + pev->frame;

	pev->animtime = globals->time; // set the animtime

	if ( pev->frame < 0.0 || pev->frame >= 256.0 )
		pev->frame += ( pev->frame / 256.0 ) * -256.0;

	m_fSequenceFinished = FALSE;

	float flSeqEnd = pev->frame + flInterval * m_flFrameRate * pev->framerate; // check if the sequence is finished
	if ( flSeqEnd >= 256 || flSeqEnd <= 0.0 )
		m_fSequenceFinished = TRUE;
}

// SDKTODO(SanyaSho): Find a better place for these!
void CBaseMonster::ResetSequenceInfo( float flInterval )
{
	void *pModel = GET_MODEL_PTR( ENT( pev ) );
	GetSequenceInfo( pModel, pev, &m_flFrameRate, &m_flGroundSpeed ); // grab the model anim info

	pev->animtime = globals->time;
	pev->framerate = 1;
	m_fSequenceFinished = FALSE;
	m_flGroundSpeed *= flInterval;
}