#if !defined( R_TRANS_H )
#define R_TRANS_H

#define	MAX_TRANS_OBJECTS	100

typedef struct
{
	entity_t *pEnt;
	float distance;
} transObjRef;

extern float		r_blend;

void AddTentity( entity_t *ent );
void R_DrawTEntitiesOnList( void );

#endif
