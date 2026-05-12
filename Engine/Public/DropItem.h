#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CDropItem : public CItemObject
{
public:
	typedef struct tagDesc : CItemObject::DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CDropItem, CItemObject)

protected:
	explicit CDropItem();
	~CDropItem() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END