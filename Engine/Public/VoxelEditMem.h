#pragma once
#include "Engine_Defines.h"

#include "Block3.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVoxelEditMem final : public CEngineBase
{
public:
	DECLARE_DERIVED_TYPE(CVoxelEditMem, CEngineBase)

private:
	explicit CVoxelEditMem();
	~CVoxelEditMem() override;

public:
	void Update(_float fTimeDelta);

public:
	static UPtr<CVoxelEditMem> Create();
};

NS_END