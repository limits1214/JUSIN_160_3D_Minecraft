#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoSpider final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoSpider, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoSpider(const _string& sPath);
	~CResEnttGeoSpider() override;

public:
	static SPtr<CResEnttGeoSpider> Create();
};

NS_END
