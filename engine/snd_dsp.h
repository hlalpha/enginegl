#if !defined( SND_DSP_H )
#define SND_DSP_H

#define	SAMPLE_RATE_11k	11025

typedef short sample_t;

typedef struct dlyline_s
{
	int cdelaysamplesmax;
	int lp;
	int idelayinput;
	int idelayoutput;
	int idelayoutputxf;
	int xfade;
	int delaysamples;
	int delayfeed;
	int lp0;
	int lp1;
	int lp2;
	int mod;
	int modcur;
	void *hdelayline;
	sample_t *lpdelayline;
} dlyline_t;

typedef struct sx_preset_s
{
	float room_lp;
	float room_mod;
	float room_size;
	float room_refl;
	float room_rvblp;
	float room_delay;
	float room_feedback;
	float room_dlylp;
	float room_left;
} sx_preset_t;

void SX_Init();
void SX_Free();
qboolean SXDLY_Init( int idelay,float delay );
void SXDLY_Free( int idelay );
void SXDLY_CheckNewStereoDelayVal();
void SXDLY_DoStereoDelay( int count );
void SXDLY_CheckNewDelayVal();
void SXDLY_DoDelay( int count );
void SXRVB_CheckNewReverbVal();
void SXRVB_DoReverb( int count );
void SXRVB_DoAMod( int count );
void SX_RoomFX( int count );

#endif // SND_DSP_H
