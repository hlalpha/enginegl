#if !defined( ITEMS_H )
#define ITEMS_H

#define MAX_ITEMS 32

class CItem : public CBaseEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	void ThrowShell( Vector &vecOrigin );
	virtual void Use( void *funcArgs );
	virtual void Touch( void *funcArgs );

private:
	int m_nItem;
};

#endif // ITEMS_H