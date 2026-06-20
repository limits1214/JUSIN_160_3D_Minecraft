#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoPlayerArmorBoots final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoPlayerArmorBoots, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoPlayerArmorBoots(const _string& sPath);
	~CResEnttGeoPlayerArmorBoots() override;

public:
	static SPtr<CResEnttGeoPlayerArmorBoots> Create();
};

NS_END
