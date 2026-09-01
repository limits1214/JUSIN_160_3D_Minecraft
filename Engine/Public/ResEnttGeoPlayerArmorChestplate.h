#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoPlayerArmorChestplate final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoPlayerArmorChestplate, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoPlayerArmorChestplate(const _string& sPath);
	~CResEnttGeoPlayerArmorChestplate() override;

public:
	static SPtr<CResEnttGeoPlayerArmorChestplate> Create();
};

NS_END
