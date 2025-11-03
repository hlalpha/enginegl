#if !defined( BASEMONSTER_H )
#define BASEMONSTER_H

class CBaseMonster : public CBaseEntity
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
	virtual int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage );
	virtual void sub_10001950( int a2 ) { unk56 = 1; }

	void SetDeathType( int iDeathType );
	void ChangeStateThink( void *funcArgs );

	void StudioFrameAdvance( float flInterval );
	void ResetSequenceInfo( float flInterval );

private:

protected:
	int unk0;
	int unk1;
	int unk2;
	int unk3;
	int unk4;
	int unk5;
	int unk6;
	int unk7;
	int unk8;
	int unk9;
	int unk10;
	int unk11;
	int unk12;
	int unk13;
	int unk14;
	int unk15;
	int unk16;
	int unk17;
	int unk18;
	int unk19;
	int unk20;
	int unk21;
	int unk22;
	int unk23;
	int unk24;
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
	int unk45;
	int unk46;
	int unk47;
	int unk48;
	int unk49;
	int unk50;
	Vector m_vecUnknown1;
	int unk54;
	int unk55;
	int unk56;
	int unk57;
	int unk58;
	float m_flFrameRate;
	float m_flGroundSpeed;
	int unk61;
	int unk62;
	int unk63;
	int unk64;
	int unk65;
	int unk66;
	int unk67;
	BOOL m_fSequenceFinished;
	int unk69;
	int unk70;
	int unk71;
	int unk72;
	int unk73;
	int unk74;
	int unk75;
	int unk76;
};

#endif // BASEMONSTER_H