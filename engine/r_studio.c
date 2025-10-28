#include "quakedef.h"
#include "mathlib.h"

extern void R_RotateForEntity (entity_t *e);
extern	cvar_t	gl_ztrick;
extern vec3_t			lightspot;

float bonetransform[MAXSTUDIOBONES][3][4];
float lighttransform[MAXSTUDIOBONES][3][4];

int g_chrome[2048][2];

int r_ambientlight;
vec3_t r_plightvec;
float r_shadelight;
colorVec r_icolormix;

studiohdr_t *pstudiohdr;
mstudiobodyparts_t *pbodypart;
mstudiomodel_t *psubmodel;
mstudiomesh_t *pmesh;

int smodels_total;
int psubmodel_verts;

auxvert_t auxverts[MAXSTUDIOVERTS];
vec3_t lightvalues[MAXSTUDIOVERTS];

auxvert_t *pauxverts;
vec3_t *pvlightvalues;

vec3_t shadevector;

qboolean R_StudioShouldDraw (void)
{
	return true;
}

int R_StudioBodyVariations (model_t *model)
{
	studiohdr_t			*phdr;
	mstudiobodyparts_t	*pbodypart;

	if ( model->type != mod_studio )
		return 0;

	phdr = (studiohdr_t *)Mod_Extradata( model );
	if ( !phdr )
		return 0;

	pbodypart = (mstudiobodyparts_t *)((byte *)phdr + phdr->bodypartindex);

	return pbodypart->nummodels;
}

void AngleQuaternion( const vec3_t angles, vec4_t quaternion )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = (angles[2] * 0.0174532925199433) * 0.5;
	sy = sin(angle);
	cy = cos(angle);
	angle = (angles[1] * 0.0174532925199433) * 0.5;
	sp = sin(angle);
	cp = cos(angle);
	angle = (angles[0] * 0.0174532925199433) * 0.5;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr*cp*cy-cr*sp*sy; // X
	quaternion[1] = cr*sp*cy+sr*cp*sy; // Y
	quaternion[2] = cr*cp*sy-sr*sp*cy; // Z
	quaternion[3] = cr*cp*cy+sr*sp*sy; // W
}

void QuaternionMatrix( const vec4_t quaternion, float (*matrix)[4] )
{
	matrix[0][0] = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
}

void QuaternionSlerp( const vec4_t p, vec4_t q, float t, vec4_t qt )
{
	int i;
	static float omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;
	for (i = 0; i < 4; i++) {
		a += (p[i]-q[i])*(p[i]-q[i]);
		b += (p[i]+q[i])*(p[i]+q[i]);
	}
	if (a > b) {
		for (i = 0; i < 4; i++) {
			q[i] = -q[i];
		}
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ((1.0 + cosom) > 0.00000001) {
		if ((1.0 - cosom) > 0.00000001) {
			omega = acos( cosom );
			sinom = sin( omega );
			sclp = sin( (1.0 - t)*omega) / sinom;
			sclq = sin( t*omega ) / sinom;
		}
		else {
			sclp = 1.0 - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else {
		qt[0] = -p[1];
		qt[1] = p[0];
		qt[2] = -p[3];
		qt[3] = p[2];
		sclp = sin( (1.0 - t) * 0.5 * M_PI);
		sclq = sin( t * 0.5 * M_PI);
		for (i = 0; i < 3; i++) {
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

void R_StudioCalcRotations (vec3_t *pos, vec4_t *q, int sequence, float f)
{
	mstudioseqdesc_t		*pseqdesc;
	mstudioanim_t			*panim;
	mstudiobonecontroller_t	*pbonecontroller;
	mstudiobonerot_t		*pbonerot;
	mstudiobonepos_t		*pbonepos;
	float					dadt, value;
	int						i, j, k;
	byte					pcontroller1, pcontroller2;
	vec3_t					adj;
	int						frame, t;
	vec3_t					angle1, angle2;
	vec4_t					q1, q2;

	frame = (int)f;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + sequence;
	panim = (mstudioanim_t *)((byte *)pstudiohdr + pseqdesc->animindex);

	{
		dadt = 1.0;

		if (currententity->animtime >= currententity->prevanimtime + 0.01)
		{
			dadt = (cl.time - currententity->animtime) / 0.1;
		}

		if (dadt > 2.0)
		{
			dadt = 2.0;
		}
	}

	{
		pbonecontroller = (mstudiobonecontroller_t *)((byte *)pstudiohdr + pstudiohdr->bonecontrollerindex);
		for (i=0 ; i<pstudiohdr->numbones ; i++)
		{
			pbonerot = (mstudiobonerot_t *)((byte *)pstudiohdr + panim[i].rotindex);
			pbonepos = (mstudiobonepos_t *)((byte *)pstudiohdr + panim[i].posindex);

			// clear the vector
			VectorCopy (vec3_origin, adj);

			for (j=0 ; j<pstudiohdr->numbonecontrollers ; j++)
			{
				if (pbonecontroller[j].bone != i)
					continue;

				pcontroller1 = currententity->controller[j];
				pcontroller2 = currententity->prevcontroller[j];

				if (pbonecontroller[j].type & STUDIO_RLOOP)
				{
					if (abs(pcontroller1 - pcontroller2) > 128)
					{
						int a, b;
						a = (pcontroller1 + 128) % 256;
						b = (pcontroller2 + 128) % 256;
						value = ((a * dadt) + (b * (1 - dadt)) - 128) * (360.0 / 256.0) + pbonecontroller[j].start;
					}
					else
					{
						value = (pcontroller1 * dadt + pcontroller2 * (1 - dadt)) * (360.0 / 256.0) + pbonecontroller[j].start;
					}
				}
				else
				{
					value = (pcontroller1 * dadt + pcontroller2 * (1.0 - dadt)) / 255.0;
					if (value < 0.0) value = 0.0;
					if (value > 1.0) value = 1.0;
					value = (1 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
				}

				switch (pbonecontroller[j].type & STUDIO_TYPES)
				{
				case STUDIO_XR:
					adj[0] += value;
					break;
				case STUDIO_YR:
					adj[1] += value;
					break;
				case STUDIO_ZR:
					adj[2] += value;
					break;
				}
			}

			int numPosFrames = panim[i].numpos;

			int posFrame1 = 0;
			for (k=1 ; k<numPosFrames ; k++)
			{
				if (pbonepos[k].frame > frame)
					break;

				posFrame1 = k;
			}

			if (posFrame1 + 1 >= numPosFrames)
			{
				VectorCopy (pbonepos[posFrame1].pos, pos[i]);
			}
			else
			{
				t = (frame - pbonepos[posFrame1].frame) / (float)(pbonepos[posFrame1 + 1].frame - pbonepos[posFrame1].frame);
				VectorLerp(pbonepos[posFrame1].pos, pbonepos[posFrame1 + 1].pos, pos[i], t);
			}

			int numRotFrames = panim[i].numrot;

			int rotFrame1 = 0;
			for (k=1 ; k<numRotFrames ; k++)
			{
				if (pbonerot[k].frame > frame)
					break;

				rotFrame1 = k;
			}

			if (rotFrame1 + 1 >= numRotFrames)
			{
				angle1[0] = pbonerot[rotFrame1].angle[0] / 100.0f + adj[0];
				angle1[1] = pbonerot[rotFrame1].angle[1] / 100.0f + adj[1];
				angle1[2] = pbonerot[rotFrame1].angle[2] / 100.0f + adj[2];
				AngleQuaternion (angle1, q[i]);
			}
			else
			{
				angle1[0] = pbonerot[rotFrame1].angle[0] / 100.0f + adj[0];
				angle1[1] = pbonerot[rotFrame1].angle[1] / 100.0f + adj[1];
				angle1[2] = pbonerot[rotFrame1].angle[2] / 100.0f + adj[2];
				AngleQuaternion(angle1, q1);

				angle2[0] = pbonerot[rotFrame1 + 1].angle[0] / 100.0f + adj[0];
				angle2[1] = pbonerot[rotFrame1 + 1].angle[1] / 100.0f + adj[1];
				angle2[2] = pbonerot[rotFrame1 + 1].angle[2] / 100.0f + adj[2];
				AngleQuaternion (angle2, q2);

				t = (frame - pbonerot[rotFrame1].frame) / (float)(pbonerot[rotFrame1 + 1].frame - pbonerot[rotFrame1].frame);
				QuaternionSlerp (q1, q2, t, q[i]);
			}
		}
	}

	{
		for (j = 0; j < pstudiohdr->numbonecontrollers; j++)
		{
			value = (currententity->controller[j] / 255.0);
			value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;

			switch (pbonecontroller[j].type)
			{
			case STUDIO_X:
				pos[pbonecontroller[j].bone][0] += value;
				break;
			case STUDIO_Y:
				pos[pbonecontroller[j].bone][1] += value;
				break;
			case STUDIO_Z:
				pos[pbonecontroller[j].bone][2] += value;
				break;
			};
		}
	}

	if (pseqdesc->motiontype & STUDIO_X)
		pos[pseqdesc->motionbone][0] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Y)
		pos[pseqdesc->motionbone][1] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Z)
		pos[pseqdesc->motionbone][2] = 0.0;
}

void R_StudioSetupBones (void)
{
	mstudioseqdesc_t	*pseqdesc;
	mstudiobone_t		*pbones;
	double				dfdt, f;
	int					i;

	static vec3_t		pos[MAXSTUDIOBONES];
	static vec4_t		q[MAXSTUDIOBONES];
	float				bonematrix[3][4];

	if (currententity->sequence >= pstudiohdr->numseq)
		currententity->sequence = 0;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

	{
		// Estimate current frame
		dfdt = (cl.time - currententity->animtime) * currententity->framerate * pseqdesc->fps;

		f = currententity->frame * (pseqdesc->numframes - 1) / 256.0;
		f += dfdt;

		if (pseqdesc->flags & STUDIO_LOOPING)
		{
			if (pseqdesc->numframes > 1)
			{
				f += (int)(f / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
			}
		}
		else
		{
			if (f < 0.0)
			{
				f = 0.0;
			}

			if (f >= pseqdesc->numframes - 1.001)
			{
				f = pseqdesc->numframes - 1.001;
			}
		}
	}


	R_StudioCalcRotations (pos, q, currententity->sequence, f);


	{
		if (currententity->sequencetime && (currententity->sequencetime + 0.2 > cl.time) && (currententity->prevsequence < pstudiohdr->numseq))
		{
			// TODO
		}
		else
		{
			currententity->prevframe = f;
		}
	}


	pbones = (mstudiobone_t *)((byte *)pstudiohdr + pstudiohdr->boneindex);
	for (i=0 ; i < pstudiohdr->numbones ; i++)
	{
		QuaternionMatrix (q[i], bonematrix);

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1)
		{
			memcpy (bonetransform[i], bonematrix, sizeof(bonematrix));
			memcpy (lighttransform[i], bonematrix, sizeof(bonematrix));
		}
		else
		{
			R_ConcatTransforms (bonetransform[pbones[i].parent], bonematrix, bonetransform[i]);
			R_ConcatTransforms (lighttransform[pbones[i].parent], bonematrix, lighttransform[i]);
		}
	}
}

void R_StudioTransformAuxVert (auxvert_t *av, int bone, vec_t *vert)
{
	av->fv[0] = DotProduct (bonetransform[bone][0], vert) + bonetransform[bone][0][3];
	av->fv[1] = DotProduct (bonetransform[bone][1], vert) + bonetransform[bone][1][3];
	av->fv[2] = DotProduct (bonetransform[bone][2], vert) + bonetransform[bone][2][3];
}

void R_StudioLighting (float *lv, int bone, int flags, vec_t *normal)
{
	float		illum, lambert, lightcos;

	float lx = DotProduct (lighttransform[bone][0], normal);
	float ly = DotProduct (lighttransform[bone][1], normal);
	float lz = DotProduct (lighttransform[bone][2], normal);

	illum = r_ambientlight;

	// TODO
	if ((flags & STUDIO_NF_FLATSHADE) /*&& dword_48F068 != 1*/)
	{
		illum += r_shadelight * 0.5f;
	}
	else
	{
		lambert = v_lambert.value;
		if (lambert <= 1.0f)
			lambert = 1.0f;

		// TODO
		float dot = r_plightvec[0] * lx +
			r_plightvec[1] * ly +
			r_plightvec[2] * lz;

		lightcos = (dot - (lambert - 1.0f)) / lambert;

		if (lightcos < 0.0f)
			illum -= r_shadelight * lightcos;
	}

	// TODO
	/*if (flags & STUDIO_NF_CHROME)
	{
		int index = ((char*)lv - (char*)pvlightvalues) >> 2;

		float v = (vpn[0] * lx + vpn[1] * ly + vpn[2] * lz) * 2.0f;

		float rx = v * lx - vpn[0];
		float rz = v * lz - vpn[2];

		g_chrome[index][0] = (int)((rx + 1.0f) * 32.0f);
		g_chrome[index][1] = (int)((rz + 1.0f) * 32.0f);
	}*/

	if (illum > 255)
		illum = 255;

	*lv = lightgammatable[(int)illum * 4] / 1023.0;
}

void R_StudioSetupLighting (alight_t *plighting)
{
	r_ambientlight = plighting->ambientlight;
	r_shadelight = plighting->shadelight;

	VectorCopy (plighting->plightvec, r_plightvec);

	r_icolormix.r = (int)( (plighting->color[0] * 0xC0FF) ) & 0xFF00;
	r_icolormix.g = (int)( (plighting->color[1] * 0xC0FF) ) & 0xFF00;
	r_icolormix.b = (int)( (plighting->color[2] * 0xC0FF) ) & 0xFF00;
}

void R_StudioSetupModel (int bodypart)
{
	int index;

	if (pstudiohdr->numbodyparts < bodypart)
	{
		Con_DPrintf ("R_StudioSetupModel: no such bodypart %d\n", bodypart);
		bodypart = 0;
	}

	pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex) + bodypart;

	index = currententity->body / pbodypart->base;
	index = index % pbodypart->nummodels;

	psubmodel = (mstudiomodel_t *)((byte *)pstudiohdr + pbodypart->modelindex) + index;
}

void R_StudioDynamicLight (entity_t *ent, alight_t *plight)
{
	//if (r_fullbright.value)
	{
		plight->shadelight = 0;
		plight->ambientlight = 192;

		plight->color[0] = plight->color[1] = plight->color[2] = 1;

		plight->plightvec[0] = 0;
		plight->plightvec[1] = 0;
		plight->plightvec[2] = -1;

		return;
	}

	// TODO
}

void R_StudioRenderFinal (entity_t *ent, alight_t *plight)
{
	float		clr, shade;
	int			i;

	smodels_total++;

	pauxverts = auxverts;
	pvlightvalues = lightvalues;
	// TODO
	//dword_CD921C = // ?

	pstudiohdr = (studiohdr_t *)Mod_Extradata( currententity->model );

	shade = -(ent->angles[1] / (M_PI*180));
	shadevector[0] = cos (shade);
	shadevector[1] = sin (shade);
	shadevector[2] = 1.f;
	VectorNormalize (shadevector);

	R_StudioSetupLighting (plight);

	glPushMatrix ();

	R_RotateForEntity (ent);

	if (gl_smoothmodels.value)
		glShadeModel (GL_SMOOTH);

	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (gl_affinemodels.value)
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	R_StudioSetupBones();

	for (i = 0 ; i < pstudiohdr->numbodyparts ; i++)
	{
		R_StudioSetupModel (i);

		// TODO
		//if (r_dointerp)
		//	ent->trivial_accept = 0;

		if (ent->rendermode != kRenderModeNormal)
		{
			switch (currententity->rendermode)
			{
			case kRenderModeColor:
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ALPHA);
				break;
			case kRenderModeAdditive:
				glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE);
				glColor4f (1,1,1,r_blend);
				break;
			default:
				glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f (1,1,1,r_blend);
			};

			glEnable (GL_BLEND);
		}

		R_GLStudioDrawPoints (ent, plight);

		if (r_shadows.value && (ent->rendermode != kRenderModeAdditive))
		{
			clr = 1.0 - (r_blend*0.5);

			glShadeModel (GL_FLAT);

			glDisable (GL_TEXTURE_2D);

			glBlendFunc (GL_ZERO, GL_SRC_COLOR);

			glEnable (GL_BLEND);

			glColor4f (clr,clr,clr,0.5);

			glDepthMask (GL_ZERO);

			glEnable (GL_POLYGON_OFFSET_FILL);

			if (!gl_ztrick.value || gldepthmin < 0.5)
				glPolygonOffset (1,-4);
			else
				glPolygonOffset (1,4);

			GLR_StudioDrawShadow ();

			glDisable (GL_POLYGON_OFFSET_FILL);

			glEnable (GL_TEXTURE_2D);

			glDisable (GL_BLEND);

			glColor4f (1,1,1,1);

			glDepthMask (GL_ONE);

			if (gl_smoothmodels.value)
				glShadeModel (GL_SMOOTH);
		}
	}

#if 0
	// draw bones
	//if (g_viewerSettings.showBones)
	{
		mstudiobone_t* pbones = (mstudiobone_t*)((byte*)pstudiohdr + pstudiohdr->boneindex);
		glDisable(GL_DEPTH_TEST);

		for (i = 0; i < pstudiohdr->numbones; i++)
		{
			if (pbones[i].parent >= 0)
			{
				glPointSize(3.0f);
				glColor3f(1, 0.7f, 0);

				glBegin(GL_LINES);
				glVertex3f(bonetransform[pbones[i].parent][0][3], bonetransform[pbones[i].parent][1][3], bonetransform[pbones[i].parent][2][3]);
				glVertex3f(bonetransform[i][0][3], bonetransform[i][1][3], bonetransform[i][2][3]);
				glEnd();

				glColor3f(0, 0, 0.8f);

				glBegin(GL_POINTS);
				if (pbones[pbones[i].parent].parent != -1)
					glVertex3f(bonetransform[pbones[i].parent][0][3], bonetransform[pbones[i].parent][1][3], bonetransform[pbones[i].parent][2][3]);
				glVertex3f(bonetransform[i][0][3], bonetransform[i][1][3], bonetransform[i][2][3]);
				glEnd();
			}
			else
			{
				// draw parent bone node
				glPointSize(5.0f);
				glColor3f(0.8f, 0, 0);
				glBegin(GL_POINTS);
				glVertex3f(bonetransform[i][0][3], bonetransform[i][1][3], bonetransform[i][2][3]);
				glEnd();
			}
		}

		glPointSize(1.0f);
	}
#endif

	if (ent->rendermode != kRenderModeNormal)
		glDisable (GL_BLEND);

	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glShadeModel (GL_FLAT);

	if (gl_affinemodels.value)
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPopMatrix ();
}

void R_GLStudioDrawPoints (entity_t *ent, alight_t *plight)
{
	byte				*pvertbone, *pnormbone;
	mstudiomodeldata_t	*pmodeldata;
	vec3_t				*pstudioverts, *pstudionorms;
	mstudiotexture_t	*ptexture;
	mstudiotrivert_t	*ptricmds;
	auxvert_t			*paux;
	float				*lv;
	int					flags;
	float				ss, st;
	float				l;
	int					i, j, t;

	pvertbone = (byte *)pstudiohdr + psubmodel->vertinfoindex;
	pnormbone = (byte *)pstudiohdr + psubmodel->norminfoindex;

	pmodeldata = (mstudiomodeldata_t *)((byte *)pstudiohdr + psubmodel->modeldataindex);
	pstudioverts = (vec3_t *)((byte *)pstudiohdr + pmodeldata->vertindex);
	pstudionorms = (vec3_t *)((byte *)pstudiohdr + pmodeldata->normindex);

	ptexture = (mstudiotexture_t *)((byte *)pstudiohdr + pstudiohdr->textureindex);

	pmesh = (mstudiomesh_t *)((byte *)pstudiohdr + psubmodel->meshindex);

	paux = pauxverts;
	psubmodel_verts = psubmodel->numverts;

	for (i=0 ; i<psubmodel_verts ; i++)
	{
		R_StudioTransformAuxVert (&paux[i], pvertbone[i], pstudioverts[i]);
	}

	lv = (float*)pvlightvalues;
	for (i=0 ; i<psubmodel->nummesh ; i++)
	{
		flags = ptexture[pmesh[i].skinref].flags;

		for (j=0 ; j<pmesh[i].numnorms ; j++, lv++, pstudionorms++, pnormbone++)
		{
			R_StudioLighting (lv, *pnormbone, flags, (float *)pstudionorms);
		}
	}

	// TODO
	//dword_CD9238 = 1;

	for (i=0 ; i<psubmodel->nummesh ; i++)
	{
		pmesh = (mstudiomesh_t*)((byte*)pstudiohdr + psubmodel->meshindex + i * sizeof(mstudiomesh_t));
		ptricmds = (mstudiotrivert_t*)((byte*)pstudiohdr + pmesh->triindex);

		ss = 1.0f / (float)ptexture[pmesh->skinref].width;
		st = 1.0f / (float)ptexture[pmesh->skinref].height;

		GL_Bind (ptexture[pmesh->skinref].index);

		// TODO
		/*if (ptexture[pmesh->skinref].flags & STUDIO_NF_CHROME)
		{
			for (t = 0; t < pmesh->numtris; t++, ptricmds += 3)
			{
				glBegin (GL_TRIANGLES);


				/// vert1
				glTexCoord2f (
					(float)g_chrome[ptricmds[0].normindex][0] * ss,
					(float)g_chrome[ptricmds[0].normindex][1] * st
				);
				l = (*pvlightvalues)[ptricmds[0].normindex];
				glColor4f (
					plight->color[0] * l,
					plight->color[1] * l,
					plight->color[2] * l,
					r_blend
				);
				glVertex3f (
					pauxverts[ptricmds[0].vertindex].fv[0],
					pauxverts[ptricmds[0].vertindex].fv[1],
					pauxverts[ptricmds[0].vertindex].fv[2]
				);
				/// vert1


				/// vert2
				glTexCoord2f (
					(float)g_chrome[ptricmds[1].normindex][0] * ss,
					(float)g_chrome[ptricmds[1].normindex][1] * st
				);
				l = (*pvlightvalues)[ptricmds[1].normindex];
				glColor4f (
					plight->color[0] * l,
					plight->color[1] * l,
					plight->color[2] * l,
					r_blend
				);
				glVertex3f (
					pauxverts[ptricmds[1].vertindex].fv[0],
					pauxverts[ptricmds[1].vertindex].fv[1],
					pauxverts[ptricmds[1].vertindex].fv[2]
				);
				/// vert2


				/// vert3
				glTexCoord2f (
					(float)g_chrome[ptricmds[2].normindex][0] * ss,
					(float)g_chrome[ptricmds[2].normindex][1] * st
				);
				l = (*pvlightvalues)[ptricmds[2].normindex];
				glColor4f (
					plight->color[0] * l,
					plight->color[1] * l,
					plight->color[2] * l,
					r_blend
				);
				glVertex3f (
					pauxverts[ptricmds[2].vertindex].fv[0],
					pauxverts[ptricmds[2].vertindex].fv[1],
					pauxverts[ptricmds[2].vertindex].fv[2]
				);
				/// vert3


				glEnd ();
			}
		}
		else*/
		{
			for (t=0 ; t<pmesh->numtris ; t++)
			{
				glBegin (GL_TRIANGLES);


				/// vert1
				glTexCoord2f (
					(float)ptricmds[0].s * ss,
					(float)ptricmds[0].t * st
				);
				l = (*pvlightvalues)[ptricmds[0].normindex];
				glColor4f (
					plight->color[0] * l,
					plight->color[1] * l,
					plight->color[2] * l,
					r_blend
				);
				glVertex3f (
					pauxverts[ptricmds[0].vertindex].fv[0],
					pauxverts[ptricmds[0].vertindex].fv[1],
					pauxverts[ptricmds[0].vertindex].fv[2]
				);
				/// vert1


				/// vert2
				glTexCoord2f (
					(float)ptricmds[1].s * ss,
					(float)ptricmds[1].t * st
				);
				l = (*pvlightvalues)[ptricmds[1].normindex];
				glColor4f (
					plight->color[0] * l,
					plight->color[1] * l,
					plight->color[2] * l,
					r_blend
				);
				glVertex3f (
					pauxverts[ptricmds[1].vertindex].fv[0],
					pauxverts[ptricmds[1].vertindex].fv[1],
					pauxverts[ptricmds[1].vertindex].fv[2]
				);
				/// vert2


				/// vert3
				glTexCoord2f (
					(float)ptricmds[2].s * ss,
					(float)ptricmds[2].t * st
				);
				l = (*pvlightvalues)[ptricmds[2].normindex];
				glColor4f (
					plight->color[0] * l,
					plight->color[1] * l,
					plight->color[2] * l,
					r_blend
				);
				glVertex3f (
					pauxverts[ptricmds[2].vertindex].fv[0],
					pauxverts[ptricmds[2].vertindex].fv[1],
					pauxverts[ptricmds[2].vertindex].fv[2]
				);
				/// vert3


				glEnd();
				ptricmds += 3;
			}
		}
	}
}

void GLR_StudioDrawShadow()
{
	// TODO
	mstudiotrivert_t *ptricmds;

	float lheight = currententity->origin[2] - lightspot[2];

	for ( int i = 0; i < psubmodel->nummesh; i++ )
	{
		pmesh = (mstudiomesh_t *)((byte *)pstudiohdr + psubmodel->meshindex) + i;
		ptricmds = (mstudiotrivert_t *)((byte *)pstudiohdr + pmesh->triindex);

		for ( int j = 0; j < pmesh->numtris * 3; j++ )
		{
			glBegin( GL_TRIANGLES );

			for ( int k = 0; k < 3; k++ )
			{
				auxvert_t *pvert = &pauxverts[ptricmds[k].vertindex];

				vec3_t drawvert;
				drawvert[0] = pvert->fv[0];
				drawvert[1] = pvert->fv[1];

				float verz = pvert->fv[2];
				float shadowZ = 1.0f - lheight;
				drawvert[2] = shadowZ;

				float offset = lheight + verz;
				drawvert[0] -= offset * shadevector[0];
				drawvert[1] -= offset * shadevector[1];

				glVertex3fv( drawvert );
			}

			glEnd();

			ptricmds++;
		}
	}
}