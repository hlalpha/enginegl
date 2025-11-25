// basemonster.h
#if !defined( BASEMONSTER_H )
#define BASEMONSTER_H

class CBaseMonster : public CBaseToggle
{
public:
	virtual int Classify() { return 0; }
	virtual void SetActivity( int iActivity ) {}
	virtual void SetState( int iState ) {}

	virtual void Alert() {};
	virtual void Pain( float flDamage ) {};
	virtual void Die() { SetDeathType( 0 ); };
	virtual void Idle() {};

	virtual int sub_10001940() { return 0; }
	virtual void TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage );
	virtual void sub_10001950( int a2 ) { unk56 = 1; }

	void SetDeathType( int iDeathType );
	void ChangeStateThink( void *funcArgs );

	void StudioFrameAdvance( float flInterval );
	void ResetSequenceInfo( float flInterval );

private:

protected:
	int m_nActivity;
	int unk26;
	int unk27;
	int unk28;
	int unk29;
	int unk30;
	int unk31;
	int unk32;
	int unk33;
	int unk34;
	int unk35;
	int unk36;
	int unk37;
	int unk38;
	int unk39;
	int unk40;
	int unk41;
	int unk42;
	int unk43;
	int unk44;
	Vector m_vecUnknown;
	int unk48;
	int unk49;
	int unk50;
	Vector m_vecAttackDir;
	entvars_t *m_pLastAttacker;
	byte unk55;
	int unk56;
	float unk57;
	float unk58;
	float m_flFrameRate;
	float m_flGroundSpeed;
	float m_flNextWeaponThink;
	int unk62;
	float m_flNextAlertTime;
	int unk64;
	int unk65;
	int unk66;
	BYTE unk67;
	BOOL m_fSequenceFinished;
	int m_nSquadSize;
	int unk70;
	int unk71;
	entvars_t *m_pOwner;
	entvars_t *m_pSquadOwner;
	int unk74;
	int unk75;
	int unk76;
};

#endif // BASEMONSTER_H