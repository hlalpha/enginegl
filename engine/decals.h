#if !defined( DECALS_H )
#define DECALS_H

#define	MAX_DECALS		4096	// absolute limit of decals on the map
#define	MAX_WAD_DECALS	256		// limit of decal textures in the WAD
#define MAX_DECALSURFS	500		// max count of decal surfaces

typedef struct
{
	char name[64];
	cache_user_t cache;
} cacheentry_t;

typedef struct cachewad_s cachewad_t;

typedef void (*PFNCACHE)(cachewad_t *,byte *);

typedef struct cachewad_s
{
	char *name;
	cacheentry_t *cache;
	int cacheCount;
	int cacheMax;
	lumpinfo_t *lumps;
	int lumpCount;
	int cacheExtra;
	PFNCACHE pfnCacheBuild;
	int numpaths;
} cachewad_t;

extern char decal_names[MAX_DECALS][16];

extern cachewad_t decals_wad;

typedef struct decalname_s // why does this struct exist?
{
	char name[16];
} decalname_t;

// sv_main.c
extern int sv_decalnamecount;
extern decalname_t sv_decalnames[512];

// decals.c
void Draw_CacheWadInit (char *name, int cacheMax, cachewad_t *wad);
void Draw_CacheWadHandler (cachewad_t *wad, PFNCACHE fn, int extraDataSize);
void Draw_DecalSetName (int decal, char *name);
int Draw_DecalIndex (int id);
int Draw_CacheIndex (cachewad_t *wad, char *path);
texture_t *Draw_DecalTexture (int index);
void *Draw_CacheGet (cachewad_t *wad, int index);

// gl_draw.c
void Draw_MiptexTexture (cachewad_t *wad,byte *data);

// gl_rsurf.c
void R_DecalInit (void);
decal_t *R_DecalAlloc (decal_t *pdecal);
void R_DecalNode (mnode_t *node);
void R_DecalShoot (int textureIndex, int entity, vec_t *position, int flags);
decal_t *R_DecalIntersect (msurface_t *psurf, int *pcount, float x, float y);
void R_DecalCreate (msurface_t *psurf, int textureIndex, float scale, float x, float y);
void R_DrawDecals (void);

#endif // DECALS_H
