// pr_cmds.h -- QCVM funcs exported for Game DLL

//===========================================================================

// pr_cmds.c
void PF_makevectors_I (float *angles);
void PF_setorigin_I (edict_t *e, float *org);

void PF_setsize_I (edict_t *e, float *min, float *max);
void PF_setmodel_I (edict_t *e, char *m);
int PF_modelindex_I (char *m);

float PF_vectoyaw_I (float *value1);
void PF_vectoangles_I (float *value1, vec_t *out);

void PF_particle_I (float *org, float *dir, float color, float count);
void PF_ambientsound_I (float *pos, char *samp, float vol, float attenuation);
void PF_sound_I (edict_t *entity, int channel, char *sample, float volume, float attenuation);

edict_t *PF_checkclient_I (void);

void PF_stuffcmd_I (edict_t *e, char *format, ...);
void PF_localcmd_I (char *str);

edict_t *PF_findradius_I (float *org, float rad);

edict_t *PF_Spawn_I (void);
void PF_Remove_I (edict_t *ed);

int PF_precache_sound_I (char *s);
int PF_precache_model_I (char *s);

float PF_walkmove_I (edict_t *ent, float yaw, float dist);
float PF_droptofloor_I (edict_t *ent);
void PF_lightstyle_I (int style, char *val);

qboolean PF_checkbottom_I (edict_t *ent);
int PF_pointcontents_I (float *v);
void PF_aim_I (edict_t *ent, float speed, vec_t *out);

void PF_changeyaw_I (edict_t *ent);
void PF_changepitch_I (edict_t *ent);

edict_t *FindEntityByString (edict_t *e, char *key, char *value);

sizebuf_t *WriteDest_I (int dest);
void PF_WriteByte_I (int dest, int c);
void PF_WriteChar_I (int dest, int c);
void PF_WriteShort_I (int dest, int c);
void PF_WriteLong_I (int dest, int c);
void PF_WriteAngle_I (int dest, float f);
void PF_WriteCoord_I (int dest, float f);
void PF_WriteString_I (int dest, char *s);
void PF_WriteWord_I (int dest, int c);

void PF_makestatic_I (edict_t *ent);
void PF_setspawnparms_I (edict_t *ent);
void PF_changelevel_I (char *s1, char *s2);
