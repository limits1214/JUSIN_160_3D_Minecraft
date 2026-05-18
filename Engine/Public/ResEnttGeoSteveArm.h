#pragma once
#include "ResEnttGeoSteve.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoSteveArm final : public CResEnttGeoSteve
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoSteveArm, CResEnttGeoSteve)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoSteveArm(const _string& sPath);
	~CResEnttGeoSteveArm() override;

public:
	static SPtr<CResEnttGeoSteveArm> Create();
};

NS_END
