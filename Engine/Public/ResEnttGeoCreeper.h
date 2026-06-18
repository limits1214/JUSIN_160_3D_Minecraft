#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoCreeper final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoCreeper, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoCreeper(const _string& sPath);
	~CResEnttGeoCreeper() override;

public:
	static SPtr<CResEnttGeoCreeper> Create();
};

NS_END
