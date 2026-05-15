#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoPlayerArmor final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoPlayerArmor, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoPlayerArmor(const _string& sPath);
	~CResEnttGeoPlayerArmor() override;

public:
	static SPtr<CResEnttGeoPlayerArmor> Create();
};

NS_END
