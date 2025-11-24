// func_break.h
#if !defined( FUNC_BREAK_H )
#define FUNC_BREAK_H

enum Explosions
{
	expRandom,
	expDirected
};

enum Materials
{
	matGlass = 0,
	matWood,
	matMetal,
	matFlesh,
	matCinderBlock,
	matCeilingTile,
	matLastMaterial
};

#define MAX_SHARDS 6

class CBreakable : public CBaseMonster
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Spawn();
	virtual void Touch();
	virtual void Use();
	virtual void Die();

public:
	Materials m_Material;
	Explosions m_Explosion;
	int m_iShards;
	int m_idShard;
	float m_angle;
};

#endif // FUNC_BREAK_H