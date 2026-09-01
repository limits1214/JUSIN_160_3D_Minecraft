#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoPlayerArmorLeggings final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoPlayerArmorLeggings, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoPlayerArmorLeggings(const _string& sPath);
	~CResEnttGeoPlayerArmorLeggings() override;

public:
	static SPtr<CResEnttGeoPlayerArmorLeggings> Create();
};

NS_END
