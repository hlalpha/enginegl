#if !defined( PLAYER_H )
#define PLAYER_H

struct SDK_SaveRestore
{
	int items;
	int unk;
	int unk2;
	float health;
	float armorvalue;
	float ammo_1;
	float ammo_2;
	float ammo_3;
	float ammo_4;
	float weapon;
	float armortype;
	byte b1;
	byte buttons;
	byte b3;
	byte b4;
	BOOL level_transition;
	char target[20];
	Vector vecOrigin;
	Vector vecVelocity;
	Vector vecAngles;
	Vector vecVAngle;
};

enum PLAYER_ANIM
{
	PLAYER_WALK = 0,
	PLAYER_WALKRIGHT,
	PLAYER_WALKSHOOT,
	PLAYER_SHOOT,
	PLAYER_RUN,
	PLAYER_WALKJUMP,
	PLAYER_DIE1
};

class CBasePlayer : public CBaseMonster
{
public:
	virtual void Spawn();

	virtual int Classify();
	virtual void SetActivity( int iActivity );
	virtual void Die();

	virtual void Jump();
	virtual void Duck();
	virtual void PreThink();
	virtual void PostThink();

	void nullsub_13();
	void nullsub_14();
	void CheckWaterJump();
	void WaterMove();
	void DeathThink( void *funcArgs );
	void PlayerUse();

	void ImpulseCommands();
	void GiveDefaultWeapons();

	void FireMP5();
	void FireGlock();
	void FireCrowbar();

	void UpdateWeaponAnim();
	void WeaponThink();

private:
	float m_flNextSecondaryAttack;
	float m_flNextUseUpdateTime;
	float m_flNextWaterMoveSound;
	int m_flNextAirUpdateTime;
	edict_t *m_pentSndLast;
	float m_flSndRoomtype;
	float m_flSndRange;
	float m_flNextStepSoundTime;
	float m_flNextWeaponAnimUpdateTime;
	char gap2[68];
	int m_nUnknownInt;
	char gap3[8];
};

#endif // PLAYER_H