#if !defined( CONST_H )
#define CONST_H

// edict->flags
#define	FL_FLY					1
#define	FL_SWIM					2
//#define	FL_GLIMPSE				4
#define	FL_CONVEYOR				4
#define	FL_CLIENT				8
#define	FL_INWATER				16
#define	FL_MONSTER				32
#define	FL_GODMODE				64
#define	FL_NOTARGET				128
#define	FL_ITEM					256
#define	FL_ONGROUND				512
#define	FL_PARTIALGROUND		1024	// not all corners are valid
#define	FL_WATERJUMP			2048	// player jumping out of water
#define	FL_JUMPRELEASED			4096	// for jump debouncing
#ifdef QUAKE2
#define FL_FLASHLIGHT			8192
#define FL_ARCHIVE_OVERRIDE		1048576
#endif

// edict->movetype values
#define	MOVETYPE_NONE			0		// never moves
#define	MOVETYPE_ANGLENOCLIP	1
#define	MOVETYPE_ANGLECLIP		2
#define	MOVETYPE_WALK			3		// gravity
#define	MOVETYPE_STEP			4		// gravity, special edge handling
#define	MOVETYPE_FLY			5
#define	MOVETYPE_TOSS			6		// gravity
#define	MOVETYPE_PUSH			7		// no clip to world, push and crush
#define	MOVETYPE_NOCLIP			8
#define	MOVETYPE_FLYMISSILE		9		// extra size to monsters
#define	MOVETYPE_BOUNCE			10
#ifdef QUAKE2
#define MOVETYPE_BOUNCEMISSILE	11		// bounce w/o gravity
#define MOVETYPE_FOLLOW			12		// track movement of aiment
#endif

// edict->solid values
#define	SOLID_NOT				0		// no interaction with other objects
#define	SOLID_TRIGGER			1		// touch on edge, but not blocking
#define	SOLID_BBOX				2		// touch on edge, block
#define	SOLID_SLIDEBOX			3		// touch on edge, but not an onground
#define	SOLID_BSP				4		// bsp clip, touch on edge, block

// edict->deadflag values
#define	DEAD_NO					0
#define	DEAD_DYING				1
#define	DEAD_DEAD				2

#define	DAMAGE_NO				0
#define	DAMAGE_YES				1
#define	DAMAGE_AIM				2

// entity effects
#define	EF_BRIGHTFIELD			1
#define	EF_MUZZLEFLASH 			2
#define	EF_BRIGHTLIGHT 			4
#define	EF_DIMLIGHT 			8
#ifdef QUAKE2
#define EF_DARKLIGHT			16
#define EF_DARKFIELD			32
#define EF_LIGHT				64
#define EF_NODRAW				128
#endif

#define	SPAWNFLAG_NOT_EASY			256
#define	SPAWNFLAG_NOT_MEDIUM		512
#define	SPAWNFLAG_NOT_HARD			1024
#define	SPAWNFLAG_NOT_DEATHMATCH	2048

#ifdef QUAKE2
// server flags
#define	SFL_EPISODE_1		1
#define	SFL_EPISODE_2		2
#define	SFL_EPISODE_3		4
#define	SFL_EPISODE_4		8
#define	SFL_NEW_UNIT		16
#define	SFL_NEW_EPISODE		32
#define	SFL_CROSS_TRIGGERS	65280
#endif

//
// temp entity events
//
#define	TE_SPIKE			0
#define	TE_SUPERSPIKE		1
#define	TE_GUNSHOT			2
#define	TE_EXPLOSION		3
#define	TE_TAREXPLOSION		4
#define	TE_LIGHTNING1		5
#define	TE_LIGHTNING2		6
#define	TE_WIZSPIKE			7
#define	TE_KNIGHTSPIKE		8
#define	TE_LIGHTNING3		9
#define	TE_LAVASPLASH		10
#define	TE_TELEPORT			11
#define TE_EXPLOSION2		12

// PGM 01/21/97 
#define TE_BEAM				13
// PGM 01/21/97 

#ifdef QUAKE2
#define TE_IMPLOSION		14
#define TE_RAILTRAIL		15
#endif

#define	MSG_BROADCAST	0		// unreliable to all
#define	MSG_ONE			1		// reliable to one (msg_entity)
#define	MSG_ALL			2		// reliable to all
#define	MSG_INIT		3		// write to the init string

typedef enum
{
	kRenderModeNormal,
	kRenderModeColor,
	kRenderModeTexture,
	kRenderModeBlur,
	kRenderModeSolid,
	kRenderModeAdditive
} eRenderMode; // Scrapped from FGD

typedef enum
{
	kRenderFXNormal = 0,
	kRenderFXSlowPulse,
	kRenderFXFastPulse,
	kRenderFXSlowWidePulse,
	kRenderFXFastWidePulse,
	kRenderFXSlowFadeAway,
	kRenderFXFastFadeAway,
	kRenderFXSlowBecomeSolid,
	kRenderFXFastBecomeSolid,
	kRenderFXSlowStrobe,
	kRenderFXFastStrobe,
	kRenderFXFasterStrobe,
	kRenderFXSlowFlicker,
	kRenderFXFastFlicker
} eRenderFX; // Scrapped from FGD

#if !defined BYTE_DEFINED
typedef unsigned char 		byte;
#define BYTE_DEFINED 1
#endif

#if __cplusplus
#define FALSE 0
#define TRUE (!FALSE)

typedef int qboolean;
#else
#undef true
#undef false

typedef enum {false, true}	qboolean;
#endif // __cplusplus

typedef struct
{
	unsigned int r, g, b;
} colorVec;

typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;

#endif // CONST_H
