#if !defined( EDICT_H )
#define EDICT_H

typedef struct
{
	vec3_t	origin;
	vec3_t	angles;
	int		modelindex;
	int		sequence;
	int		frame;
	int		colormap;
	int		skin;
	int		effects;
	int		rendermode;
	int		renderamt;
	byte	rendercolor[4];
	int		renderfx;
} entity_state_t;

#endif // EDICT_H
