#if !defined( EIFACE_H )
#define EIFACE_H

/*
	pfnAlertMessage:
		atype:
			0 - INFO
			1 - CONSOLE LOG
			2 - WARN
			3 - ERROR
*/
typedef enum alert_s
{
	at_info = 0,
	at_log,
	at_warn,
	at_error,
} alert_t;

typedef struct KeyValueData_s
{
	char	*szClassName;	// in: entity classname
	char	*szKeyName;		// in: name of key
	char	*szValue;		// in: value of key
	int		fHandled;		// out: DLL sets to true if key-value pair was understood
} KeyValueData;

typedef struct TraceResult_s
{
	int			fAllSolid;		// if true, plane is not valid
	int			fStartSolid;	// if true, the initial point was in a solid area
	int			fInOpen;
	int			fInWater;
	float		flFraction;		// time completed, 1.0 = didn't hit anything
	vec3_t		vecEndPos;		// final position
	float		flPlaneDist;
	vec3_t		vecPlaneNormal;	// surface normal at impact
	int			fHit;			// entity the surface is on
} TraceResult;

typedef struct enginefuncs_s
{
	int					(*pfnPrecacheModel)				( char *s );
	int					(*pfnPrecacheSound)				( char *s );
	void				(*pfnSetModel)					( struct edict_s *e, char *m );
	int					(*pfnModelIndex)				( char *m );
	void				(*pfnSetSize)					( struct edict_s *e, float *min, float *max );

	void				(*pfnChangeLevel)				( char *s1, char *s2 );

	void				(*pfnSetSpawnParms)				( struct edict_s *ent );
	void				(*pfnSaveSpawnParms)			( struct edict_s *e );

	float				(*pfnVecToYaw)					( float *in );
	void				(*pfnVecToAngles)				( float *in, float *out );
	void				(*pfnMoveToGoal)				( struct edict_s *ent, float dist );
	void				(*pfnMoveToOrigin)				( struct edict_s *ent, float *goal, float dist, int strafe );
	void				(*pfnChangeYaw)					( struct edict_s *ent );
	void				(*pfnChangePitch)				( struct edict_s *ent );
	struct edict_s*		(*pfnFindEntityByString)		( struct edict_s *e, char *key, char *value );
	int					(*pfnGetEntityIllum)			( struct edict_s *pEnt );
	struct edict_s*		(*pfnFindRadius)				( float *org, float rad );
	struct edict_s*		(*pfnCheckClient)				( void );
	void				(*pfnMakeVectors)				( float *angles );
	struct edict_s*		(*pfnCreateEntity)				( void );
	void				(*pfnRemoveEntity)				( struct edict_s *ed );
	void				(*pfnMakeStatic)				( struct edict_s *ent );
	float				(*pfnEntIsOnFloor)				( struct edict_s *ent ); // float?
	float				(*pfnDropToFloor)				( struct edict_s *ent ); // float?
	float				(*pfnWalkMove)					( struct edict_s *ent, float yaw, float dist ); // float?
	void				(*pfnSetOrigin)					( struct edict_s *e, float *org );
	void				(*pfnEmitSound)					( struct edict_s *entity, int channel, char *sample, float volume, float attenuation );
	void				(*pfnEmitAmbientSound)			( float *pos, char *samp, float vol, float attenuation );
	void				(*pfnTraceLine)					( float *v1, float *v2, int fNoMonsters, struct edict_s *pentToSkip, TraceResult *ptr );
	void				(*pfnTraceToss)					( struct edict_s *ent, struct edict_s *ignore, TraceResult *ptr );
	void				(*pfnGetAimVector)				( struct edict_s *ent, float speed, vec_t *out );
	void				(*pfnServerCommand)				( char *str );
	void				(*pfnServerExecute)				( struct edict_s *e, char *format, ... );
	void				(*pfnParticleEffect)			( float *org, float *dir, float color, float count );
	void				(*pfnLightStyle)				( int style, char *val );
	int					(*pfnDecalIndex)				( int idx, char *name );
	int					(*pfnPointContents)				( float *v );

	void				(*pfnWriteByte)					( int dest, int c );
	void				(*pfnWriteChar)					( int dest, int c );
	void				(*pfnWriteShort)				( int dest, int c );
	void				(*pfnWriteLong)					( int dest, int c );
	void				(*pfnWriteAngle)				( int dest, float f );
	void				(*pfnWriteCoord)				( int dest, float f );
	void				(*pfnWriteString)				( int dest, char *s );
	void				(*pfnWriteEntity)				( int dest, int c );

	float				(*pfnCVarGetFloat)				( char *name );
	char*				(*pfnCVarGetString)				( char *name );
	void				(*pfnCVarSetFloat)				( char *name, float value );
	void				(*pfnCVarSetString)				( char *name, char *value );

	void				(*pfnAlertMessage)				( int atype, char *szFmt, ... );
	int					(*pfnEngineFprintf)				( FILE *f, char *format, ... );

	void*				(*pfnPvAllocEntPrivateData)		( struct edict_s *pEdict, int size );
	void*				(*pfnPvEntPrivateData)			( struct edict_s *pEdict );
	void				(*pfnFreeEntPrivateData)		( struct edict_s *pEdict );

	void*				(*pfnDispatchFuncById)			( struct edict_s *ent, int dllFunc );
	void				(*pfnChangeMethods)				( void ); // TODO

	char*				(*pfnSzFromIndex)				( string_t iString );
	int					(*pfnAllocString)				( char *string );

	struct entvars_s*	(*pfnGetVarsOfEnt)				( struct edict_s *pEdict );
	struct edict_s*		(*pfnPEntityOfEntOffset)		( int iEntOffset );
	int					(*pfnEntOffsetOfPEntity)		( struct edict_s *pEdict );
	int					(*pfnIndexOfEdict)				( int ent ); // TODO
	struct edict_s*		(*pfnFindEntityByVars)			( struct entvars_s *pEntVars );

	void*				(*pfnGetModelPtr)				( struct edict_s *pEdict );

#if defined( QUIVER_TESTS )
	// EXTENDED API FOR TESTS
	void				(*pfnRegisterCvar)				( struct cvar_s *cvar );
#endif
} enginefuncs_t;

#endif // !defined( EIFACE_H )
