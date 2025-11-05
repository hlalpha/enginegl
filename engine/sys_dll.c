#include "quakedef.h"

#include "eiface.h"

// sys_dll.c
edict_t *PEntityOfEntOffset (int iEntOffset);
int EntOffsetOfPEntity (edict_t *pEdict);
int IndexOfEdict (int ent);
char *SzFromIndex (string_t iString);
entvars_t *GetVarsOfEnt (edict_t *ent);
edict_t *FindEntityByVars (entvars_t *pEntVars);

void *GetDispatchFuncById_I (edict_t *ent, int dllFunc);
void ChangeMethod (void);

float CVarGetFloat (char *name);
char *CVarGetString (char *name);
void CVarSetFloat (char *name, float value);
void CVarSetString (char *name, char *value);

int AllocEngineString (char *string);
void SaveSpawnParams (edict_t *e);
void *GetModelPtr (edict_t *pEdict);

// sv_move.c
void SV_MoveToGoal_I (edict_t *ent, float dist);
float SV_MoveToOrigin_I (edict_t *ent, const float *pflGoal, float dist, int iStrafe);

#if defined( QUIVER_TESTS )
void pfnRegisterCvar (cvar_t *cvar)
{
	Con_Printf ("TESTS: Registering a cvar from DLL: %s\n", cvar->name);

	Cvar_RegisterVariable (cvar);
}
#endif

enginefuncs_t g_engfuncsExportedToDlls =
{
	PF_precache_model_I,
	PF_precache_sound_I,
	PF_setmodel_I,
	PF_modelindex_I,
	PF_setsize_I,

	PF_changelevel_I,

	PF_setspawnparms_I,
	SaveSpawnParams,

	PF_vectoyaw_I,
	PF_vectoangles_I,
	SV_MoveToGoal_I,
	SV_MoveToOrigin_I,
	PF_changeyaw_I,
#if defined( QUAKE2 )
	PF_changepitch_I,
#else
	NULL,
#endif
	FindEntityByString,
	GetEntityIllum,
	PF_findradius_I,
	PF_checkclient_I,
	PF_makevectors_I,
	PF_Spawn_I,
	PF_Remove_I,
	PF_makestatic_I,
	PF_checkbottom_I,
	PF_droptofloor_I,
	PF_walkmove_I,
	PF_setorigin_I,
	PF_sound_I,
	PF_ambientsound_I,
	PF_traceline_DLL,
#if defined( QUAKE2 )
	PF_TraceToss_DLL,
#else
	NULL,
#endif // QUAKE2
	PF_aim_I,
	PF_localcmd_I,
	PF_stuffcmd_I,
	PF_particle_I,
	PF_lightstyle_I,
	PF_DecalIndex_I,
	PF_pointcontents_I,

	PF_WriteByte_I,
	PF_WriteChar_I,
	PF_WriteShort_I,
	PF_WriteLong_I,
	PF_WriteAngle_I,
	PF_WriteCoord_I,
	PF_WriteString_I,
	PF_WriteEntity_I,

	CVarGetFloat,
	CVarGetString,
	CVarSetFloat,
	CVarSetString,

	AlertMessage,
	EngineFprintf,

	ED_AllocatePrivateData,
	ED_GetPrivateData,
	ED_FreePrivateData,

	GetDispatchFuncById_I,
	ChangeMethod,

	SzFromIndex,
	AllocEngineString,
	GetVarsOfEnt,
	PEntityOfEntOffset,
	EntOffsetOfPEntity,
	IndexOfEdict,
	FindEntityByVars,

	GetModelPtr,

#if defined( QUIVER_TESTS )
	// EXTENDED API FOR TESTS
	pfnRegisterCvar,
#endif
};


/*
===========
GetDispatchFuncById

Get exported QC func from game DLL
===========
*/
DISPATCHFUNC GetDispatchFuncById (edict_t *ent, int dllFunc)
{
	switch (dllFunc)
	{
	case 0: return g_pfnDispatchSpawn;
	case 1: return g_pfnDispatchThink;
	case 2: return g_pfnDispatchTouch;
	case 3: return g_pfnDispatchUse;
	case 4: return g_pfnDispatchBlocked;
	case 5: return g_pfnDispatchKeyValue;
	case 6: return g_pfnDispatchSave;
	//case 7: return g_pfnDispatchRestore ?
	};
	// don't return null here, OG enginegl.exe returns some random memory
}


#if defined( QUIVER_QUAKE_COMPAT )
// pr_edict.c
dfunction_t *ED_FindFunction (char *name);
#endif


/*
===========
CallDispatchFunc

Call an exported QC func from game DLL
===========
*/
void CallDispatchFunc (edict_t *ent, int dllFunc, void *funcArg)
{
	DISPATCHFUNC func = GetDispatchFuncById (ent, dllFunc);
	if (func)
		func (&ent->v, funcArg);
	else if (dllFunc == 0)
		Con_Printf ("ASSERT FAILURE: entity method (spawn) is null");

#if defined( QUIVER_QUAKE_COMPAT )
	switch (dllFunc)
	{
	case 0: // Spawn:
	{
		// look for the spawn function
		dfunction_t *spawnfunc = ED_FindFunction( pr_strings + ent->v.classname );

		if (spawnfunc)
			PR_ExecuteProgram (spawnfunc - pr_functions);
		break;
	}
	case 1: // Think:
	{
		if (ent->v.think)
			PR_ExecuteProgram (ent->v.think);
		break;
	}
	case 2: // Touch:
	{
		if (ent->v.touch)
			PR_ExecuteProgram (ent->v.touch);
		break;
	}
	//case 3: // Use: not used anywhere
	//{
	//	break;
	//}
	case 4: // Blocked:
	{
		if (ent->v.blocked)
			PR_ExecuteProgram (ent->v.blocked);
		break;
	}
	//case 5: // KeyValue: used only by extDLL
	//{
	//	break;
	//}
	//case 6: // Save: used only by extDLL
	//{
	//	break;
	//}
	//case 7: // Restore?: in theory must be a Restore function for the saverestore system
	//{
	//	break;
	//}
	};
#endif
}


/*
===========
ED_FreePrivateData
===========
*/
// ED_FreePrivateData ??


/*
===========
ED_SetGameDLLVars

Set custom progdefs for entity
===========
*/
void ED_SetGameDLLVars (edict_t *ent) // TODO(SanyaSho): Better name?
{
	ent->v.pContainingEntity = ent;
	ent->v.pSystemGlobals = pr_global_struct;
}


/*
===========
ED_AllocatePrivateData

Allocate a entity private data
===========
*/
void *ED_AllocatePrivateData (edict_t *pEdict, int size)
{
	void		*pData;

	ED_FreePrivateData (pEdict);

	if (size <= 0)
		return NULL;

	pData = calloc (1, size);
	pEdict->pvPrivateData = pData;

	return pData;
}


/*
===========
ED_GetPrivateData

Get engine-allocated entity private data
===========
*/
void *ED_GetPrivateData (edict_t *pEdict)
{
	return pEdict->pvPrivateData;
}


/*
===========
ED_FreePrivateData

Free engine-allocated entity private data
===========
*/
void ED_FreePrivateData (edict_t *pEdict)
{
	if (pEdict->pvPrivateData)
		free (pEdict->pvPrivateData);

	pEdict->pvPrivateData = 0;
}


/*
===========
PEntityOfEntOffset

Get entity edict_t from offset
===========
*/
edict_t *PEntityOfEntOffset (int iEntOffset)
{
	return PROG_TO_EDICT (iEntOffset);
}


/*
===========
EntOffsetOfPEntity

Get global entity offset from edict_t
===========
*/
int EntOffsetOfPEntity( edict_t *pEdict )
{
	return EDICT_TO_PROG( pEdict );
}


/*
===========
IndexOfEdict

Weird version of NUM_FOR_EDICT
===========
*/
int IndexOfEdict (int ent)
{
	// TODO: Check if we're OOB?
	return ent / pr_edict_size;
}


/*
===========
SzFromIndex

Get engine-allocated string from the pool
===========
*/
char *SzFromIndex (string_t iString)
{
	return (char *)(pr_strings + iString);
}


/*
===========
GetVarsOfEnt

Get entvars_t from edict_t
===========
*/
entvars_t *GetVarsOfEnt (edict_t *ent)
{
	return &ent->v;
}


/*
===========
FindEntityByVars
===========
*/
edict_t *FindEntityByVars (entvars_t *pEntVars)
{
	int		i;

	// Nothing is watching
	if (sv.num_edicts <= 0)
		return NULL;

	for (i=0 ; i<sv.num_edicts ; i++)
	{
		edict_t *ed = EDICT_NUM (i);
		if (&ed->v == pEntVars) // NOTE: OG enginegl.exe is trying to subtract ed and entvars to check if it's equal -120 (offset to entvars)
			return ed;
	}

	return NULL;
}


/*
===========
GetDispatchFuncById_I

Wrapper for GetDispatchFuncById
===========
*/
void *GetDispatchFuncById_I (edict_t *ent, int dllFunc) // FIXME: I don't want to move DISPATCHFUNC to the extdll.h header
{
	return (void *(__cdecl *)(edict_t *, int))GetDispatchFuncById (ent, dllFunc);
}


/*
===========
ChangeMethod

Function had 3 arguments:
- edict_t *
- method ID
- function

Example usage:
CHANGE_METHOD(ENT(pev), em_use, light_use);
===========
*/
void ChangeMethod (void)
{
	Sys_Error ("CAN'T CHANGE METHODS HERE!");
}


/*
===========
CVarGetFloat

Wrapper for Cvar_VariableValue
===========
*/
float CVarGetFloat (char *name)
{
	return Cvar_VariableValue (name);
}


/*
===========
CVarGetString

Wrapper for Cvar_VariableString
===========
*/
char *CVarGetString (char *name)
{
	return Cvar_VariableString (name);
}


/*
===========
CVarSetFloat

Wrapper for Cvar_SetValue
===========
*/
void CVarSetFloat (char *name, float value)
{
	Cvar_SetValue (name, value);
}


/*
===========
CVarSetString

Wrapper for Cvar_Set
===========
*/
void CVarSetString (char *name, char *value)
{
	Cvar_Set (name, value);
}


/*
===========
AllocEngineString
===========
*/
int AllocEngineString (char *string)
{
	return ED_NewString (string) - pr_strings;
}


/*
===========
SaveSpawnParams
===========
*/
void SaveSpawnParams (edict_t *e)
{
	int ent;
	client_t *client;
	int i;
	
	ent = NUM_FOR_EDICT (e);
	if(ent < 1 || svs.maxclients < ent)
		PR_RunError( "Entity is not a client" );

	client = svs.clients + ent;

	for (i=0 ; i<NUM_SPAWN_PARMS ; i++)
		client->spawn_parms[i] = (&pr_global_struct->parm1)[i];
}


/*
===========
GetModelPtr

Wrapper for Mod_Extradata
===========
*/
void *GetModelPtr (edict_t *pEdict)
{
	return Mod_Extradata (sv.models[(int)pEdict->v.modelindex]);
}
