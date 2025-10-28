#include "quakedef.h"

int			numTransObjs = 0;

transObjRef	transObjects[MAX_TRANS_OBJECTS];

float		r_blend;

void AddTentity( entity_t *ent )
{
	if ( numTransObjs == MAX_TRANS_OBJECTS )
		Sys_Error( "AddTentity: Too many objects" );

	vec3_t in;

	VectorAdd( ent->model->maxs, ent->model->mins, in );

	VectorScale( in, 0.5f, in );

	VectorAdd( ent->origin, in, in );

	VectorSubtract( r_origin, in, in );

	float flDistSq = DotProduct( in, in );

	int i = numTransObjs;
	while ( i > 0 && transObjects[i - 1].distance < flDistSq )
	{
		transObjects[i] = transObjects[i - 1];
		i--;
	}

	transObjects[i].pEnt = ent;
	transObjects[i].distance = flDistSq;

	numTransObjs++;
}

extern void R_DrawSpriteModel (entity_t *e);
extern void R_DrawAliasModel (entity_t *e);
extern void R_DrawBrushModel (entity_t *e);

void R_DrawTEntitiesOnList( void )
{
	int				i;
	transObjRef		*ref;
	alight_t		light;
	vec3_t			lvec;

	if ( !r_drawentities.value )
		return;

	for ( i = 0, ref = transObjects; i < numTransObjs; i++, ref++ )
	{
		currententity = ref->pEnt;

		r_blend = CL_FxBlend( currententity ) / 256.0;

		if ( !r_blend )
			continue;

		switch ( currententity->model->type )
		{
		case mod_sprite:
			R_DrawSpriteModel( currententity );
			break;

		case mod_alias:
			R_DrawAliasModel( currententity );
			break;

		case mod_studio:
			if (R_StudioShouldDraw())
			{
				light.plightvec = lvec;
				R_StudioDynamicLight (currententity, &light);
				R_StudioRenderFinal (currententity, &light);
			}
			break;

		case mod_brush:
			R_DrawBrushModel( currententity );
			break;

		default:
			break;
		};
	}

	numTransObjs = 0;
	r_blend = 1.f;
}

