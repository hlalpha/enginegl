#include "quakedef.h"
#include "sound.h"

//#define DSP_USE_RETAIL_PRESETS // Use this if you want to use DSP table from retail hw.dll

#define LIMIT( x ) ( x > 32767 ) ? 32767 : ( x < -32768 ) ? -32768 : x // FIXME(SanyaSho): must return -32767!
#define RAND(MIN, MAX) ((MIN) + rand() % ((MAX) - (MIN) + 1))

cvar_t sxdly_delay		= {	"room_delay",		"0"		};
cvar_t sxdly_feedback	= {	"room_feedback",	"0.2"	};
cvar_t sxdly_lp			= {	"room_dlylp",		"1.0"	};
cvar_t sxrvb_size		= {	"room_size",		"0"		};
cvar_t sxrvb_feedback	= {	"room_refl",		"0.7"	};
cvar_t sxrvb_lp			= {	"room_rvblp",		"1.0"	};
cvar_t sxste_delay		= {	"room_left",		"0"		};
cvar_t sxmod_lowpass	= {	"room_lp",			"0"		};
cvar_t sxmod_mod		= {	"room_mod",			"0"		};
cvar_t sxroom_type		= {	"room_type",		"0"		};
cvar_t sxroom_off		= {	"room_off",			"0"		};

dlyline_t rgsxdly[4];
int rgsxlp[10];

float sxdly_delayprev;
float sxrvb_sizeprev;
float sxste_delayprev;
float sxroom_typeprev;

int sxamodr;
int sxamodl;
int sxamodrt;
int sxamodlt;

int sxmod1;
int sxmod1cur;
int sxmod2;
int sxmod2cur;

sx_preset_t rgsxpre[] =
{
#if !defined( DSP_USE_RETAIL_PRESETS )
	// Original table
	/*	lp,		mod,	size,	refl,	rvblp,	delay,	fback,	dlylp,	left	*/
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.0		},	// 0  Normal (off)
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.08,	0.8,	2.0,	0.0		},	// 1  Generic

	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.02,	0.75,	0.0,	0.001	},	// 2  Metal Small
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.03,	0.78,	0.0,	0.002	},	// 3  Metal Medium
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.77,	0.0,	0.003	},	// 4  Metal Large

	{	0.0,	0.0,	0.05,	0.85,	1.0,	0.008,	0.96,	2.0,	0.01	},	// 5  Tunnel Small
	{	0.0,	0.0,	0.05,	0.88,	1.0,	0.01,	0.98,	2.0,	0.02	},	// 6  Tunnel Medium
	{	0.0,	0.0,	0.05,	0.92,	1.0,	0.015,	0.995,	2.0,	0.04	},	// 7  Tunnel Large

	{	0.0,	0.0,	0.05,	0.84,	1.0,	0.0,	0.0,	2.0,	0.003	},	// 8  Chamber Small
	{	0.0,	0.0,	0.05,	0.9,	1.0,	0.0,	0.0,	2.0,	0.002	},	// 9  Chamber Medium
	{	0.0,	0.0,	0.05,	0.95,	1.0,	0.0,	0.0,	2.0,	0.001	},	// 10 Chamber Large

	{	0.0,	0.0,	0.05,	0.7,	0.0,	0.0,	0.0,	2.0,	0.003	},	// 11 Bright Small
	{	0.0,	0.0,	0.055,	0.78,	0.0,	0.0,	0.0,	2.0,	0.002	},	// 12 Bright Medium
	{	0.0,	0.0,	0.05,	0.86,	0.0,	0.0,	0.0,	2.0,	0.001	},	// 13 Bright Large

	{	1.0,	1.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.01	},	// 14 Water 1
	{	1.0,	1.0,	0.0,	0.0,	1.0,	0.06,	0.85,	2.0,	0.02	},	// 15 Water 2
	{	1.0,	1.0,	0.0,	0.0,	1.0,	0.2,	0.6,	2.0,	0.05	},	// 16 Water 3

	{	0.0,	0.0,	0.05,	0.8,	1.0,	0.15,	0.48,	2.0,	0.008	},	// 17 Concrete Small
	{	0.0,	0.0,	0.06,	0.9,	1.0,	0.22,	0.52,	2.0,	0.005	},	// 18 Concrete Medium
	{	0.0,	0.0,	0.07,	0.94,	1.0,	0.3,	0.6,	2.0,	0.001	},	// 19 Concrete Large

	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.3,	0.42,	2.0,	0.0		},	// 20 Big 1
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.35,	0.48,	2.0,	0.0		},	// 21 Big 2
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.38,	0.6,	2.0,	0.0		},	// 22 Big 3

	{	0.0,	0.0,	0.05,	0.9,	1.0,	0.2,	0.28,	0.0,	0.0		},	// 23 Cavern Small
	{	0.0,	0.0,	0.07,	0.9,	1.0,	0.3,	0.4,	0.0,	0.0		},	// 24 Cavern Medium
	{	0.0,	0.0,	0.09,	0.9,	1.0,	0.35,	0.5,	0.0,	0.0		},	// 25 Cavern Large

	{	0.0,	1.0,	0.01,	0.9,	0.0,	0.0,	0.0,	2.0,	0.05	},	// 26 Weirdo 1
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.009,	0.999,	2.0,	0.04	},	// 27 Weirdo 2
	{	0.0,	0.0,	0.001,	0.999,	0.0,	0.2,	0.8,	2.0,	0.05	}	// 28 Weirdo 3
#else
	// Retail table
	/*	lp,		mod,	size,	refl,	rvblp,	delay,	fback,	dlylp,	left	*/
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.0		},	// 0  Normal (off)
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.065,	0.1,	0.0,	0.001	},	// 1  Generic

	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.02,	0.75,	0.0,	0.01	},	// 2  Metal Small
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.03,	0.78,	0.0,	0.02	},	// 3  Metal Medium
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.77,	0.0,	0.03	},	// 4  Metal Large

	{	0.0,	0.0,	0.05,	0.85,	1.0,	0.008,	0.96,	2.0,	0.01	},	// 5  Tunnel Small
	{	0.0,	0.0,	0.05,	0.88,	1.0,	0.01,	0.98,	2.0,	0.02	},	// 6  Tunnel Medium
	{	0.0,	0.0,	0.05,	0.92,	1.0,	0.015,	0.995,	2.0,	0.04	},	// 7  Tunnel Large

	{	0.0,	0.0,	0.05,	0.84,	1.0,	0.0,	0.0,	2.0,	0.012	},	// 8  Chamber Small
	{	0.0,	0.0,	0.05,	0.9,	1.0,	0.0,	0.0,	2.0,	0.008	},	// 9  Chamber Medium
	{	0.0,	0.0,	0.05,	0.95,	1.0,	0.0,	0.0,	2.0,	0.004	},	// 10 Chamber Large

	{	0.0,	0.0,	0.05,	0.7,	0.0,	0.0,	0.0,	2.0,	0.012	},	// 11 Bright Small
	{	0.0,	0.0,	0.055,	0.78,	0.0,	0.0,	0.0,	2.0,	0.008	},	// 12 Bright Medium
	{	0.0,	0.0,	0.05,	0.86,	0.0,	0.0,	0.0,	2.0,	0.002	},	// 13 Bright Large

	{	1.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.01	},	// 14 Water 1
	{	1.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.85,	2.0,	0.02	},	// 15 Water 2
	{	1.0,	0.0,	0.0,	0.0,	1.0,	0.2,	0.6,	2.0,	0.05	},	// 16 Water 3

	{	0.0,	0.0,	0.05,	0.8,	1.0,	0.0,	0.48,	2.0,	0.016	},	// 17 Concrete Small
	{	0.0,	0.0,	0.06,	0.9,	1.0,	0.0,	0.52,	2.0,	0.01	},	// 18 Concrete Medium
	{	0.0,	0.0,	0.07,	0.94,	1.0,	0.3,	0.6,	2.0,	0.008	},	// 19 Concrete Large

	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.3,	0.42,	2.0,	0.0		},	// 20 Big 1
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.35,	0.48,	2.0,	0.0		},	// 21 Big 2
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.38,	0.6,	2.0,	0.0		},	// 22 Big 3

	{	0.0,	0.0,	0.05,	0.9,	1.0,	0.2,	0.28,	0.0,	0.0		},	// 23 Cavern Small
	{	0.0,	0.0,	0.07,	0.9,	1.0,	0.3,	0.4,	0.0,	0.0		},	// 24 Cavern Medium
	{	0.0,	0.0,	0.09,	0.9,	1.0,	0.35,	0.5,	0.0,	0.0		},	// 25 Cavern Large

	{	0.0,	1.0,	0.001,	0.9,	0.0,	0.0,	0.0,	2.0,	0.05	},	// 26 Weirdo 1
	{	0.0,	0.0,	0.0,	0.0,	1.0,	0.009,	0.999,	2.0,	0.04	},	// 27 Weirdo 2
	{	0.0,	0.0,	0.001,	0.999,	0.0,	0.2,	0.8,	2.0,	0.05	},	// 28 Weirdo 3
#endif
};


/*
===============
SX_Init
===============
*/
void SX_Init (void)
{
	Q_memset (rgsxdly, 0, sizeof(rgsxdly));
	Q_memset (rgsxlp, 0, sizeof(rgsxlp));

	sxdly_delayprev = -1.f;
	sxrvb_sizeprev = -1.f;
	sxste_delayprev = -1.f;
	sxroom_typeprev = -1.f;

	sxamodr = 255;
	sxamodl = 255;
	sxamodrt = 255;
	sxamodlt = 255;

	sxmod1 = 350 * (shm->speed / SAMPLE_RATE_11k);
	sxmod1cur = 350 * (shm->speed / SAMPLE_RATE_11k);
	sxmod2 = 450 * (shm->speed / SAMPLE_RATE_11k);
	sxmod2cur = 450 * (shm->speed / SAMPLE_RATE_11k);

	Con_DPrintf( "\nFX Processor Initialization\n" );

	Cvar_RegisterVariable (&sxdly_delay);
	Cvar_RegisterVariable (&sxdly_feedback);
	Cvar_RegisterVariable (&sxdly_lp);
	Cvar_RegisterVariable (&sxrvb_size);
	Cvar_RegisterVariable (&sxrvb_feedback);
	Cvar_RegisterVariable (&sxrvb_lp);
	Cvar_RegisterVariable (&sxste_delay);
	Cvar_RegisterVariable (&sxmod_lowpass);
	Cvar_RegisterVariable (&sxmod_mod);
	Cvar_RegisterVariable (&sxroom_type);
	Cvar_RegisterVariable (&sxroom_off);
}


/*
===============
SX_Free
===============
*/
void SX_Free (void)
{
	int			i;
	SXDLY_Free (0);

	for (i=1 ; i<2 ; i++)
	{
		SXDLY_Free (i);
	}

	SXDLY_Free (3);
}


/*
===============
SXDLY_Init
===============
*/
qboolean SXDLY_Init (int idelay, float delay)
{
	HGLOBAL		hBuffer;
	void		*pLock;
	dlyline_t	*dly;

	dly = &rgsxdly[idelay];

	if ( delay > 0.4 )
		delay = 0.4;

	if (dly->lpdelayline)
	{
		GlobalUnlock (dly->hdelayline);
		GlobalFree (dly->hdelayline);
		dly->hdelayline = 0;
		dly->lpdelayline = 0;
	}

	if (delay == 0.0)
		return true;

	dly->cdelaysamplesmax = (int)(shm->speed * delay + 1.0);

	hBuffer = GlobalAlloc (GMEM_MOVEABLE|GMEM_SHARE, dly->cdelaysamplesmax * 2);
	if (!hBuffer)
	{
		Con_SafePrintf ("Sound FX: Out of memory.\n");
		return false;
	}

	pLock = GlobalLock (hBuffer);
	if (!pLock)
	{
		Con_SafePrintf ("Sound FX: Failed to lock.\n");
		GlobalFree (hBuffer);
		return false;
	}

	memset (pLock, 0, dly->cdelaysamplesmax * sizeof(sample_t));
	dly->hdelayline = hBuffer;
	dly->lpdelayline = (sample_t *)pLock;
	dly->idelayinput = 0;
	dly->idelayoutput = dly->cdelaysamplesmax - dly->delaysamples;
	dly->xfade = 0;
	dly->mod = dly->modcur = 0;
	dly->lp = 1;
	dly->lp0 = dly->lp1 = dly->lp2 = 0;

	return true;
}


/*
===============
SXDLY_Free
===============
*/
void SXDLY_Free (int idelay)
{
	dlyline_t *tmp = &rgsxdly[idelay];
	if (tmp->lpdelayline)
	{
		GlobalUnlock (tmp->hdelayline);
		GlobalFree (tmp->hdelayline);
		tmp->hdelayline = NULL;
		tmp->lpdelayline = NULL;
	}
}


/*
===============
SXDLY_CheckNewStereoDelayVal
===============
*/
void SXDLY_CheckNewStereoDelayVal (void)
{
	int			stespeed;
	dlyline_t	*dly;

	dly = &rgsxdly[3];

	if (sxste_delayprev == sxste_delay.value)
		return;

	if (!sxste_delay.value)
	{
		SXDLY_Free (3);
		sxste_delayprev = 0.0;
		return;
	}

	stespeed = min (sxste_delay.value, 0.1) * shm->speed;

	if (!dly->lpdelayline)
	{
		dly->delaysamples = stespeed;
		SXDLY_Init (3, 0.1);
	}

	if (dly->delaysamples != stespeed)
	{
		dly->idelayoutputxf = dly->idelayinput - stespeed;

		if (dly->idelayoutputxf < 0)
			dly->idelayoutputxf += dly->cdelaysamplesmax;

		dly->xfade = 128;
	}

	sxste_delayprev = sxste_delay.value;

	dly->mod = 5000 * (shm->speed / SAMPLE_RATE_11k);
	dly->modcur = dly->mod;

	if (!dly->delaysamples)
		SXDLY_Free (3);
}


/*
===============
SXDLY_DoStereoDelay
===============
*/
void SXDLY_DoStereoDelay (int count)
{
	portable_samplepair_t	*pBuf;
	int			left, nCount;
	dlyline_t	*dly;
	sample_t	sxf, sdly;

	dly = &rgsxdly[3];

	if (!dly->lpdelayline)
		return;

	pBuf = paintbuffer;
	nCount = count;

	while (nCount--)
	{
		if (--dly->modcur < 0)
			dly->modcur = dly->mod;

		sdly = dly->lpdelayline[dly->idelayoutput];
		left = pBuf->left;

		if (dly->xfade || sdly || left)
		{
			if (!dly->xfade && !dly->modcur)
			{
				dly->idelayoutputxf = dly->idelayinput + ((rand() * dly->delaysamples) / (0x7FFF * 2)) - dly->delaysamples;

				if (dly->idelayoutputxf < 0)
					dly->idelayoutputxf += dly->cdelaysamplesmax;

				dly->xfade = 128;
			}

			if (dly->xfade)
			{
				sxf = dly->lpdelayline[dly->idelayoutputxf++];
				sdly = (((128 - dly->xfade) * sxf) >> 7) + ((dly->xfade * sdly) >> 7);

				if (dly->idelayoutputxf >= dly->cdelaysamplesmax)
					dly->idelayoutputxf = 0;

				if (!--dly->xfade)
					dly->idelayoutput = dly->idelayoutputxf;
			}

			left = LIMIT (left);

			dly->lpdelayline[dly->idelayinput] = left;

			pBuf->left = sdly;
		}
		else
		{
			dly->lpdelayline[dly->idelayinput] = 0;
		}

		if (++dly->idelayinput >= dly->cdelaysamplesmax)
			dly->idelayinput = 0;

		if (++dly->idelayoutput >= dly->cdelaysamplesmax)
			dly->idelayoutput = 0;

		pBuf++;
	};
}


/*
===============
SXDLY_CheckNewDelayVal
===============
*/
void SXDLY_CheckNewDelayVal (void)
{
	dlyline_t	*dly;

	dly = &rgsxdly[0];

	if (sxdly_delay.value != sxdly_delayprev)
	{
		if (!sxdly_delay.value)
		{
			SXDLY_Free (0);
			sxdly_delayprev = sxdly_delay.value;
		}
		else
		{
			if (!dly->lpdelayline)
				SXDLY_Init (0, 0.4);

			dly->delaysamples = min(sxdly_delay.value, 0.4) * shm->speed;

			if (dly->lpdelayline)
			{
				Q_memset (dly->lpdelayline, 0, dly->cdelaysamplesmax * sizeof(sample_t));
				dly->lp0 = dly->lp1 = dly->lp2 = 0;
			}

			dly->idelayinput = 0;
			dly->idelayoutput = dly->cdelaysamplesmax - dly->delaysamples;

			sxdly_delayprev = sxdly_delay.value;

			if (!dly->delaysamples)
				SXDLY_Free (0);
		}
	}

	dly->lp = (int)(sxdly_lp.value);
	dly->delayfeed = (int)(sxdly_feedback.value * 255.0);
}


/*
===============
SXDLY_DoDelay
===============
*/
void SXDLY_DoDelay (int count)
{
	int			nCount, left, right, newleft, newright, val, valt;
	portable_samplepair_t	*pBuf;
	sample_t	sdly;
	dlyline_t	*dly;

	dly = &rgsxdly[0];

	if (!dly->lpdelayline)
		return;

	pBuf = paintbuffer;
	nCount = count;

	while (nCount--)
	{
		sdly = dly->lpdelayline[dly->idelayoutput];

		left = pBuf->left;
		right = pBuf->right;

		if (sdly || left || right)
		{
			val = ((left + right) >> 1) + ((dly->delayfeed * sdly) >> 8);
			val = LIMIT( val );

			if (dly->lp)
			{
				valt = (dly->lp0 + dly->lp1 + val) / 3;

				dly->lp0 = dly->lp1;
				dly->lp1 = val;
			}
			else
			{
				valt = val;
			}

			dly->lpdelayline[dly->idelayinput] = valt;

			newleft = (valt >> 2) + left;
			newright = (valt >> 2) + right;

			newleft = LIMIT( newleft );
			newright = LIMIT( newright );

			pBuf->left = newleft;
			pBuf->right = newright;
		}
		else
		{
			dly->lp0 = dly->lp1 = 0;
			dly->lpdelayline[dly->idelayinput] = 0;
		}

		if (++dly->idelayinput >= dly->cdelaysamplesmax)
			dly->idelayinput = 0;

		if (++dly->idelayoutput >= dly->cdelaysamplesmax)
			dly->idelayoutput = 0;

		pBuf++;
	};
}


/*
===============
SXRVB_CheckNewReverbVal
===============
*/
void SXRVB_CheckNewReverbVal (void)
{
	dlyline_t	*dly;
	int			i, rvbspeed, rvbmod;

	if (sxrvb_sizeprev != sxrvb_size.value)
	{
		sxrvb_sizeprev = sxrvb_size.value;

		if (!sxrvb_size.value)
		{
			SXDLY_Free (1);
			SXDLY_Free (2);
		}
		else
		{
			for (i=1 ; i<3 ; i++)
			{
				dly = &rgsxdly[i];

				switch (i)
				{
				case 1:
					rvbspeed = min (sxrvb_size.value, 0.1) * shm->speed;
					rvbmod = 500 * (shm->speed / SAMPLE_RATE_11k);
					break;
				case 2:
					rvbspeed = min (sxrvb_size.value * 0.71, 0.1) * shm->speed;
					rvbmod = 700 * (shm->speed / SAMPLE_RATE_11k);
					break;
				default:
					rvbspeed = 0;
					break;
				};

				dly->modcur = dly->mod = rvbmod; // FIXME(SanyaSho): rvbmod value will be overwritten by SXDLY_Init below!
												 // Also DSP modulation is broken for some reason, if you move this after SXDLY_Init call you will get some random shit coming from your headset.

				if (!dly->lpdelayline)
				{
					dly->delaysamples = rvbspeed;
					SXDLY_Init (i, 0.1);
				}

				if (rvbspeed != dly->delaysamples)
				{
					dly->idelayoutputxf = dly->idelayinput - rvbspeed;

					if (dly->idelayoutputxf < 0)
						dly->idelayoutputxf += dly->cdelaysamplesmax;

					dly->xfade = 32;
				}

				if (!dly->delaysamples)
				{
					SXDLY_Free (i);
				}
			}
		}
	}

	rgsxdly[1].delayfeed = (int)(sxrvb_feedback.value * 255.0);
	rgsxdly[1].lp = (int)(sxrvb_lp.value);

	rgsxdly[2].delayfeed = (int)(sxrvb_feedback.value * 255.0);
	rgsxdly[2].lp = (int)(sxrvb_lp.value);
}


/*
===============
SXRVB_DoReverb
===============
*/
void SXRVB_DoReverb (int count)
{
	int			left, right;
	int			val, valt;
	int			vlr, voutm;
	sample_t	sxf, sdly;
	portable_samplepair_t	*pBuf;
	int			nCount;
	dlyline_t	*dly1, *dly2;

	dly1 = &rgsxdly[1];
	dly2 = &rgsxdly[2];

	if (!dly1->lpdelayline)
		return;

	pBuf = paintbuffer;
	nCount = count;

	while (nCount--)
	{
		left = pBuf->left;
		right = pBuf->right;

		voutm = 0;
		vlr = (left + right) >> 1;


		// delay 1
		sdly = dly1->lpdelayline[dly1->idelayoutput];

		if (--dly1->modcur < 0)
			dly1->modcur = dly1->mod;

		if (!dly1->xfade)
		{
			if (!sdly && !left && !right)
			{
				dly1->lp0 = 0;
				dly1->lpdelayline[dly1->idelayinput] = 0;
				voutm = 0;
			}
			else
			{
				if (!dly1->mod)
				{
					dly1->idelayoutputxf = dly1->idelayinput + ((rand() * dly1->delaysamples) / -(0x7FFF * 2)) - dly1->delaysamples;

					if (dly1->idelayoutputxf < 0)
						dly1->idelayoutputxf += dly1->cdelaysamplesmax;

					dly1->xfade = 32;
				}
			}
		}

		if (dly1->xfade)
		{
			sxf = dly1->lpdelayline[dly1->idelayoutputxf++];
			sdly = (((32 - dly1->xfade) * sxf) >> 5) + ((dly1->xfade * sdly) >> 5);

			if (dly1->idelayoutputxf >= dly1->cdelaysamplesmax)
				dly1->idelayoutputxf = 0;

			if (!--dly1->xfade)
				dly1->idelayoutput = dly1->idelayoutputxf;
		}

		if (sdly)
		{
			val = vlr + ((dly1->delayfeed * sdly) >> 8);
			val = LIMIT( val );
		}
		else
		{
			val = vlr;
		}

		if (dly1->lp)
		{
			valt = (val + dly1->lp0) >> 1;
			dly1->lp0 = val;
		}
		else
		{
			valt = val;
		}

		dly1->lpdelayline[dly1->idelayinput] = valt;

		voutm = valt;

		if (++dly1->idelayinput >= dly1->cdelaysamplesmax)
			dly1->idelayinput = 0;

		if (++dly1->idelayoutput >= dly1->cdelaysamplesmax)
			dly1->idelayoutput = 0;


		// delay 2
		if (--dly2->modcur < 0)
			dly2->modcur = dly2->mod;

		if (dly2->lpdelayline)
		{
			if (!dly2->xfade)
			{
				if (!sdly && !left && !right)
				{
					dly2->lp0 = 0;
					dly2->lpdelayline[dly2->idelayinput] = 0;
					voutm = 0;
				}
				else
				{
					if (!dly2->mod)
					{
						dly2->idelayoutputxf = dly2->idelayinput + ((rand() * dly2->delaysamples) / -(0x7FFF * 2)) - dly2->delaysamples;

						if (dly2->idelayoutputxf < 0)
							dly2->idelayoutputxf += dly2->cdelaysamplesmax;

						dly2->xfade = 32;
					}
				}
			}

			if (dly2->xfade)
			{
				sxf = dly2->lpdelayline[dly2->idelayoutputxf++];
				sdly = (((32 - dly2->xfade) * sxf) >> 5) + ((dly2->xfade * sdly) >> 5);

				if (dly2->idelayoutputxf >= dly2->cdelaysamplesmax)
					dly2->idelayoutputxf = 0;

				if (!--dly2->xfade)
					dly2->idelayoutput = dly2->idelayoutputxf;
			}

			if (sdly)
			{
				val = vlr + ((dly2->delayfeed * sdly) >> 8);
				val = LIMIT (val);
			}
			else
			{
				val = vlr;
			}

			if (dly2->lp)
			{
				valt = (val + dly2->lp0) >> 1;
				dly2->lp0 = val;
			}
			else
			{
				valt = val;
			}

			dly2->lpdelayline[dly2->idelayinput] = valt;

			voutm = valt;

			if (++dly2->idelayinput >= dly2->cdelaysamplesmax)
				dly2->idelayinput = 0;

			if (++dly2->idelayoutput >= dly2->cdelaysamplesmax)
				dly2->idelayoutput = 0;
		}

		left = voutm / 6 + left;
		right = voutm / 6 + right;

		left = LIMIT (left);
		right = LIMIT (right);

		pBuf->left = left;
		pBuf->right = right;

		pBuf++;
	}
}


/*
===============
SXRVB_DoAMod
===============
*/
void SXRVB_DoAMod (int count)
{
	int			left, right, oldleft, oldright;
	portable_samplepair_t	*pBuf;
	int			nCount;
	qboolean	doLowpass, doMod;

	if (sxmod_lowpass.value != 0.0 || sxmod_mod.value != 0.0)
	{
		pBuf = paintbuffer;
		nCount = count;

		doLowpass = sxmod_lowpass.value != 0.f;
		doMod = sxmod_mod.value != 0.f;

		while (nCount--)
		{
			left = pBuf->left;
			right = pBuf->right;

			if (doLowpass)
			{
				oldleft = left;
				oldright = right;

				left = (left + rgsxlp[0] + rgsxlp[1] + rgsxlp[2] + rgsxlp[3] + rgsxlp[4]) / 4;
				right = (right + rgsxlp[5] + rgsxlp[6] + rgsxlp[7] + rgsxlp[8] + rgsxlp[9]) / 4;

				rgsxlp[4] = oldleft;
				rgsxlp[9] = oldright;

				rgsxlp[0] = rgsxlp[1];
				rgsxlp[1] = rgsxlp[2];
				rgsxlp[2] = rgsxlp[3];
				rgsxlp[3] = rgsxlp[4];
				rgsxlp[4] = rgsxlp[5];
				rgsxlp[5] = rgsxlp[6];
				rgsxlp[6] = rgsxlp[7];
				rgsxlp[7] = rgsxlp[8];
				rgsxlp[8] = rgsxlp[9];
			}

			if (doMod)
			{
				if (--sxmod1cur < 0)
					sxmod1cur = sxmod1;
				if (!sxmod1)
					sxamodlt = RAND (32, 255);

				if (--sxmod2cur < 0)
					sxmod2cur = sxmod2;
				if (!sxmod2)
					sxamodrt = RAND (32, 255);

				left = (sxamodl * left) >> 8;
				right = (sxamodr * right) >> 8;

				if (sxamodl < sxamodlt)
					sxamodl++;
				else if (sxamodl > sxamodlt)
					sxamodl--;

				if (sxamodr < sxamodrt)
					sxamodr++;
				else if (sxamodr > sxamodrt)
					sxamodl--;
			}

			pBuf->left = LIMIT (left);
			pBuf->right = LIMIT (right);

			pBuf++;
		}
	}
}


/*
===============
SX_RoomFX
===============
*/
void SX_RoomFX (int count)
{
	int			nPreset;

	if (sxroom_off.value)
		return;

	qboolean bRestart = false;

	if (sxroom_typeprev != sxroom_type.value)
	{
		nPreset = (int)sxroom_type.value;

		sxroom_typeprev = nPreset;

		if (nPreset <= ARRAYSIZE(rgsxpre)-1)
		{
			Cvar_SetValue (	"room_lp",			rgsxpre[nPreset].room_lp		);
			Cvar_SetValue (	"room_mod",			rgsxpre[nPreset].room_mod		);
			Cvar_SetValue (	"room_size",		rgsxpre[nPreset].room_size		);
			Cvar_SetValue (	"room_refl",		rgsxpre[nPreset].room_refl		);
			Cvar_SetValue (	"room_rvblp",		rgsxpre[nPreset].room_rvblp		);
			Cvar_SetValue (	"room_delay",		rgsxpre[nPreset].room_delay		);
			Cvar_SetValue (	"room_feedback",	rgsxpre[nPreset].room_feedback	);
			Cvar_SetValue (	"room_dlylp",		rgsxpre[nPreset].room_dlylp		);
			Cvar_SetValue (	"room_left",		rgsxpre[nPreset].room_left		);
		}

		SXRVB_CheckNewReverbVal ();
		SXDLY_CheckNewDelayVal ();
		SXDLY_CheckNewStereoDelayVal ();

		bRestart = true;
	}

	if (bRestart || sxroom_type.value)
	{
		SXRVB_CheckNewReverbVal ();
		SXDLY_CheckNewDelayVal ();
		SXDLY_CheckNewStereoDelayVal ();
		SXRVB_DoAMod (count);
		SXRVB_DoReverb (count);
		SXDLY_DoDelay (count);
		SXDLY_DoStereoDelay (count);
	}
}
