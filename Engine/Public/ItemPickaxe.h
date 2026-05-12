#pragma once

#include "Item.h"
NS_BEGIN(Engine)

class ENGINE_DLL CItemPickaxe : public CItem
{
public:
	DECLARE_DERIVED_TYPE(CItemPickaxe, CItem)

protected:
	CItemPickaxe();
	~CItemPickaxe() override;

};

NS_END
