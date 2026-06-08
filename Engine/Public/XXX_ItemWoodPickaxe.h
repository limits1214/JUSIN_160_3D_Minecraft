#pragma once

#include "XXX_ItemPickAxe.h"
NS_BEGIN(Engine)

class ENGINE_DLL XXX_CItemWoodPickaxe : public XXX_CItemPickaxe
{
public:
	DECLARE_DERIVED_TYPE(XXX_CItemWoodPickaxe, XXX_CItemPickaxe)

protected:
	XXX_CItemWoodPickaxe();
	~XXX_CItemWoodPickaxe() override;

};

NS_END
