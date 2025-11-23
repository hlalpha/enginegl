#if !defined( WEAPONS_H )
#define WEAPONS_H

#define MAX_WEAPONS 32

class CSprayCan : public CBaseEntity
{
public:
	void Spawn( entvars_t *pevOwner );
	void AdvanceSpriteFrame( void *funcArgs );

private:
	byte m_fFrame;
};

class CBloodSplat : public CBaseEntity
{
public:
	void Spawn( entvars_t *pevOwner );
	void AdvanceSpriteFrame( void *funcArgs );
};

#endif // !WEAPONS_H
