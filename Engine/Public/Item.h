#pragma once

#include "GameObject.h"
NS_BEGIN(Engine)

class ENGINE_DLL CItem : public CEngineBase
{
public:
	DECLARE_DERIVED_TYPE(CItem, CEngineBase)

protected:
	CItem();
	~CItem() override;

};

NS_END
