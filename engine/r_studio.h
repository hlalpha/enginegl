#if !defined( R_STUDIO_H )
#define R_STUDIO_H

typedef struct
{
	float fv[3];
} auxvert_t;

typedef struct alight_s
{
	int ambientlight;
	int shadelight;
	vec3_t color;
	float *plightvec;
} alight_t;

#include "studio.h"

// l_studio.c
void Mod_LoadStudioModel (model_t *mod, void *buffer);

// r_studio.c
qboolean R_StudioCheckBBox (void);
int R_StudioBodyVariations (model_t *model);
void R_StudioCalcRotations (vec3_t *pos, vec4_t *q, int sequence, float f);
void R_StudioSetupBones (void);
void R_StudioTransformAuxVert (auxvert_t *av, int bone, vec_t *vert);
void R_StudioLighting (float *lv, int bone, int flags, vec_t *normal);
void R_StudioSetupLighting (alight_t *plighting);
void R_StudioSetupModel (int bodypart);
void R_StudioDynamicLight (entity_t *ent, alight_t *plight);
void R_StudioRenderFinal (entity_t *ent, alight_t *plight);
void R_GLStudioDrawPoints (entity_t *ent, alight_t *plight);
void GLR_StudioDrawShadow (void);

#endif // R_STUDIO_H