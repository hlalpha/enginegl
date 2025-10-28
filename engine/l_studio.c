#include "quakedef.h"

extern model_t	*loadmodel;
extern char		loadname[32];	// for hunk tags

/*
==================
Mod_LoadStudioModel
==================
*/
void Mod_LoadStudioModel (model_t *mod, void *buffer)
{
	studiohdr_t *pinmodel;
	int start, end, total;
	int version, i;
	studiohdr_t *pbuf;
	mstudiotexture_t *ptexture;
	byte *data, *pPal;
	char identifier[256];

	start = Hunk_LowMark();

	pinmodel = (studiohdr_t *)(byte *)buffer;

	version = LittleLong (pinmodel->version);
	if (version != STUDIO_VERSION)
	{
		memset (pinmodel, 0, sizeof(studiohdr_t));
		strcpy (pinmodel->name, "bogus");
		pinmodel->length = sizeof(studiohdr_t);
	}

	mod->type = mod_studio;
	mod->flags = 0;

	pbuf = Hunk_AllocName (pinmodel->length, loadname);
	memcpy (pbuf, buffer, pinmodel->length);

	ptexture = (mstudiotexture_t *)((byte *)pbuf + pinmodel->textureindex);
	for ( i = 0; i < pinmodel->numtextures; i++, ptexture++ )
	{
		data = (byte *)pbuf + ptexture->index;
		pPal = &data[ptexture->width * ptexture->height];

		strcpy (identifier, mod->name);
		strcat (identifier, ptexture->name);

		ptexture->index = GL_LoadTexture (identifier, ptexture->width, ptexture->height, data, false, 0, pPal);
	}

	end = Hunk_LowMark();
	total = end - start;

	Cache_Alloc (&mod->cache, total, loadname);
	if (!mod->cache.data)
		return;
	memcpy (mod->cache.data, pbuf, total);

	Hunk_FreeToLowMark (start);
}
