#pragma once

#include "XXX_Item.h"
NS_BEGIN(Engine)

class ENGINE_DLL XXX_CItemPickaxe : public XXX_CItem
{
public:
	DECLARE_DERIVED_TYPE(XXX_CItemPickaxe, XXX_CItem)

protected:
	XXX_CItemPickaxe();
	~XXX_CItemPickaxe() override;

};

NS_END
