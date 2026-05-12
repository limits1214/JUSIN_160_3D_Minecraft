#pragma once

#include "ItemPickAxe.h"
NS_BEGIN(Engine)

class ENGINE_DLL CItemWoodPickaxe : public CItemPickaxe
{
public:
	DECLARE_DERIVED_TYPE(CItemWoodPickaxe, CItemPickaxe)

protected:
	CItemWoodPickaxe();
	~CItemWoodPickaxe() override;

};

NS_END
