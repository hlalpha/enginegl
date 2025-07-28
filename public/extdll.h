#if !defined( EXTDLL_H )
#define EXTDLL_H

//
// !!! Make sure to include stdio.h/cstdio BEFORE including this header !!!
//

#if defined( __cplusplus ) // no need to have these engine-side
typedef int qboolean;
typedef float vec_t;
#endif

//#define QUIVER_TESTS

/*
	pfnAlertMessage:
		atype:
			0 - INFO
			1 - CONSOLE LOG
			2 - WARN
			3 - ERROR
*/

typedef struct KeyValueData_s
{
	char	*szClassName;	// in: entity classname
	char	*szKeyName;		// in: name of key
	char	*szValue;		// in: value of key
	int		fHandled;		// out: DLL sets to true if key-value pair was understood
} KeyValueData;

typedef struct enginefuncs_s
{
	int					(*pfnPFPrecacheModel)			( char *s );
	int					(*pfnPFPrecacheSound)			( char *s );
	void				(*pfnPFSetModel)				( struct edict_s *e, char *m );
	int					(*pfnPFModelIndex)				( char *m );
	void				(*pfnPFSetSize)					( struct edict_s *e, float *min, float *max );

	void				(*pfnPFChangeLevel)				( char *s1, char *s2 );

	void				(*pfnPFSetSpawnParams)			( struct edict_s *ent );
	void				(*pfnSaveSpawnParams)			( struct edict_s *e );

	float				(*pfnPFVecToYaw)				( float *in );
	void				(*pfnPFVecToAngles)				( float *in, float *out );
	void				(*pfnPFMoveToGoal)				( struct edict_s *ent, float dist );
	void				(*pfnSVMoveToOrigin)			( struct edict_s *ent, float *goal, float dist, int strafe );
	void				(*pfnPFChangeYaw)				( struct edict_s *ent );
	void				(*pfnPFChangePitch)				( struct edict_s *ent );
	struct edict_s*		(*pfnFindEntityByString)		( struct edict_s *e, char *key, char *value );
	int					(*pfnGetEntityIllum)			( struct edict_s *pEnt );
	struct edict_s*		(*pfnPFFindRadius)				( float *org, float rad );
	struct edict_s*		(*pfnPFCheckClient)				( void );
	void				(*pfnPFMakeVectors)				( float *angles );
	struct edict_s*		(*pfnPFSpawn)					( void );
	void				(*pfnPFRemove)					( struct edict_s *ed );
	void				(*pfnPFMakeStatic)				( struct edict_s *ent );
	qboolean			(*pfnPFCheckBottom)				( struct edict_s *ent );
	float				(*pfnPFDropToFloor)				( struct edict_s *ent ); // float?
	float				(*pfnPFWalkMove)				( struct edict_s *ent, float yaw, float dist ); // float?
	void				(*pfnPFSetOrigin)				( struct edict_s *e, float *org );
	void				(*pfnPFSound)					( struct edict_s *entity, int channel, char *sample, float volume, float attenuation );
	void				(*pfnPFAmbientSound)			( float *pos, char *samp, float vol, float attenuation );
	void				(*pfnPF_UNK_TraceLine)			( float *v1, float *v2, int nomonsters, struct edict_s *ent, struct trace_t *ptr ); // TODO
	void				(*pfnPF_UNK_TraceToss)			( struct edict_s *ent, struct edict_s *ignore, struct trace_t *ptr ); // TODO
	void				(*pfnPFAim)						( struct edict_s *ent, float speed, vec_t *out );
	void				(*pfnPFLocalCmd)				( char *str );
	void				(*pfnPFStuffCmd)				( struct edict_s *e, char *format, ... );
	void				(*pfnPFParticle)				( float *org, float *dir, float color, float count );
	void				(*pfnLightStyle)				( int style, char *val );
	int					(*pfnPFDecalIndex)				( int idx, char *name );
	int					(*pfnPFPointContents)			( float *v );

	void				(*pfnPFWriteByte)				( int dest, int c );
	void				(*pfnPFWriteChar)				( int dest, int c );
	void				(*pfnPFWriteShort)				( int dest, int c );
	void				(*pfnPFWriteLong)				( int dest, int c );
	void				(*pfnPFWriteAngle)				( int dest, float f );
	void				(*pfnPFWriteCoord)				( int dest, float f );
	void				(*pfnPFWriteString)				( int dest, char *s );
	void				(*pfnPFWriteEntity)				( int dest, int c );

	float				(*pfnCVarGetFloat)				( char *name );
	char*				(*pfnCVarGetString)				( char *name );
	void				(*pfnCVarSetFloat)				( char *name, float value );
	void				(*pfnCVarSetString)				( char *name, char *value );

	void				(*pfnAlertMessage)				( int atype, char *szFmt, ... );
	int					(*pfnEngineFprintf)				( FILE *f, char *format, ... );

	void*				(*pfnAllocatePrivateData)		( struct edict_s *pEdict, int size );
	void*				(*pfnGetPrivateData)			( struct edict_s *pEdict );
	void				(*pfnFreePrivateData)			( struct edict_s *pEdict );

	void*				(*pfnDispatchFuncById)			( struct edict_s *ent, int dllFunc );
	void				(*pfn_UNK_ChangeMethod)			( void ); // TODO

	char*				(*pfnSzFromIndex)				( int iString );
	int					(*pfnAllocEngineString)			( char *string );
	struct entvars_s*	(*pfnGetVarsOfEnt)				( struct edict_s *pEdict );
	struct edict_s*		(*pfnPEntityOfEntOffset)		( int iEntOffset );
	int					(*pfnEntOffsetOfPEntity)		( struct edict_s *pEdict );
	int					(*pfnIndexOfEdict)				( int ent ); // TODO
	struct edict_s*		(*pfnPEntityOfEntIndex)			( int iEntIndex ); // TODO

	void*				(*pfnGetModelPtr)				( struct edict_s *pEdict );

#if defined( QUIVER_TESTS )
	// EXTENDED API FOR TESTS
	void				(*pfnRegisterCvar)				( struct cvar_s *cvar );
#endif
} enginefuncs_t;

#endif // !defined( EXTDLL_H )
