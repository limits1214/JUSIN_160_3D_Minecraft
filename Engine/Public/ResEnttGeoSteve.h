#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoSteve final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoSteve, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoSteve(const _string& sPath);
	~CResEnttGeoSteve() override;

public:
	static SPtr<CResEnttGeoSteve> Create();
};

NS_END
