#pragma once
#include "Engine_Defines.h"

#include "ItemObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CChestStorage final: public CEngineBase
{
public:
	DECLARE_DERIVED_TYPE(CChestStorage, CEngineBase)

private:
	explicit CChestStorage();
	~CChestStorage() override;

public:
	void Update(E::_float fTimeDelta) ;

public:
	static UPtr<CChestStorage> Create();
};

NS_END