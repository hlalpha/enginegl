#include "quakedef.h"

char decal_names[MAX_DECALS][16];

cachewad_t decals_wad;


/*
===============
Draw_CacheWadInit
===============
*/
void Draw_CacheWadInit (char *name, int cacheMax, cachewad_t *wad)
{
	int len;
	int handle;
	int size;
	wadinfo_t header;
	int i;
	lumpinfo_t *lumps;
	
	len = COM_OpenFile (name, &handle);
	if ( handle == -1 )
		Sys_Error ("Draw_LoadWad: Couldn't open %s\n", name);

	Sys_FileRead (handle, &header, 12);

	if ( header.identification[0] != 'W' ||
		 header.identification[1] != 'A' ||
		 header.identification[2] != 'D' ||
		 header.identification[3] != '3'
	   )
	{
		Sys_Error ("Wad file %s doesn't have WAD3 id\n", name);
	}

	wad->lumps = malloc (len - header.infotableofs);

	Sys_FileSeek (handle, header.infotableofs);
	Sys_FileRead (handle, wad->lumps, len - header.infotableofs);

	COM_CloseFile (handle);

	lumps = wad->lumps;
	for (i = 0; i < header.numlumps; i++, lumps++)
	{
		W_CleanupName (lumps->name, lumps->name);
	}

	wad->lumpCount = header.numlumps;
	wad->cacheCount = 0;
	wad->cacheMax = cacheMax;
	wad->name = name; // TODO(SanyaSho): use strdup?
	size = sizeof(cacheentry_t) * cacheMax;
	wad->cache = malloc(size);
	memset (wad->cache, 0, size);
	wad->pfnCacheBuild = NULL;
	wad->cacheExtra = 0;
}


/*
===============
Draw_CacheWadHandler
===============
*/
void Draw_CacheWadHandler (cachewad_t *wad, PFNCACHE fn, int extraDataSize)
{
	wad->pfnCacheBuild = fn;
	wad->cacheExtra = extraDataSize;
}


/*
===============
Draw_DecalSetName
===============
*/
void Draw_DecalSetName (int decal, char *name)
{
	// Overflow check
	if ( decal > MAX_WAD_DECALS-1 )
		return;

	strncpy (decal_names[decal], name, sizeof(decal_names[decal])-1);
	decal_names[decal][sizeof(decal_names[decal])-1] = '\0';
}


/*
===============
Draw_DecalIndex
===============
*/
int Draw_DecalIndex (int id)
{
	if ( !decal_names[id][0] )
		Sys_Error ("Used decal #%d without no name\n", id);

	return Draw_CacheIndex (&decals_wad, decal_names[id]);
}


/*
===============
Draw_CacheIndex
===============
*/
int Draw_CacheIndex (cachewad_t *wad, char *path)
{
	int i;
	cacheentry_t *pCache;

	pCache = wad->cache;

	for (i = 0; i < wad->cacheCount; i++, pCache++)
	{
		if ( !strcmp(path, pCache->name) )
		{
			break;
		}
	}

	if (i == wad->cacheCount)
	{
		if (wad->cacheMax == wad->cacheCount)
		{
			Sys_Error ("Draw_CacheIndex: Cache wad (%s) out of %d entries", wad->name, wad->cacheMax);
		}

		wad->cacheCount++;
		strcpy (pCache->name, path);
	}

	return i;
}


/*
===============
Draw_DecalTexture
===============
*/
texture_t *Draw_DecalTexture (int index)
{
	return (texture_t *)Draw_CacheGet (&decals_wad, index);
}


/*
===============
Draw_CacheGet
===============
*/
void *Draw_CacheGet (cachewad_t *wad, int index)
{
	int i;
	cacheentry_t *pCache;
	cache_user_t *pCacheUser;
	int handle;
	byte *pCacheAlloc;
	void *pCacheCheck;
	lumpinfo_t *lumps;
	char szDest[16], szCleanDest[16];

	if (wad->cacheCount <= index)
	{
		Sys_Error ("Cache wad indexed before load %s: %d", wad->name, index);
	}

	pCache = &wad->cache[index];
	pCacheUser = &pCache->cache;
	
	// Don't alloc anything if the image was already in the cache
	pCacheCheck = Cache_Check (pCacheUser);
	if (pCacheCheck)
	{
		return pCacheCheck;
	}
	
	COM_FileBase (pCache->name, szDest);

	W_CleanupName (szDest, szCleanDest);

	lumps = wad->lumps;

	for (i = 0; i < wad->lumpCount; i++, lumps++)
	{
		if ( !Q_strcmp (szCleanDest, lumps->name) )
			break;
	}

	if (i == wad->lumpCount)
	{
		Sys_Error ("Draw_CacheGet: couldn't find %s in %s", pCache->name, wad->name);
	}

	COM_OpenFile (wad->name, &handle);

	if (handle == -1)
	{
		return NULL;
	}

	pCacheAlloc = Cache_Alloc (pCacheUser, lumps->size + wad->cacheExtra + 1, szCleanDest);
	if (!pCacheAlloc)
	{
		Sys_Error ("Draw_CacheGet: not enough space for %s in %s", pCache->name, wad->name);
	}

	pCacheAlloc[lumps->size + wad->cacheExtra] = 0;

	Sys_FileSeek (handle, lumps->filepos);
	Sys_FileRead (handle, pCacheAlloc + wad->cacheExtra, lumps->size);

	COM_CloseFile (handle);

	if (wad->pfnCacheBuild)
		wad->pfnCacheBuild (wad, pCacheAlloc);

	if (!pCacheUser->data)
		Sys_Error ("Draw_CacheGet: failed to load %s", pCache->name);

	return pCacheUser->data;
}
