#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoChicken final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoChicken, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoChicken(const _string& sPath);
	~CResEnttGeoChicken() override;

public:
	static SPtr<CResEnttGeoChicken> Create();
};

NS_END
