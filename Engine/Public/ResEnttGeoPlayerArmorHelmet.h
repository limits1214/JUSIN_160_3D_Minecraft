#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoPlayerArmorHelmet final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoPlayerArmorHelmet, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoPlayerArmorHelmet(const _string& sPath);
	~CResEnttGeoPlayerArmorHelmet() override;

public:
	static SPtr<CResEnttGeoPlayerArmorHelmet> Create();
};

NS_END
