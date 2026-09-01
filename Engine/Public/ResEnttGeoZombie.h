
#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoZombie final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoZombie, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoZombie(const _string& sPath);
	~CResEnttGeoZombie() override;

public:
	static SPtr<CResEnttGeoZombie> Create();
};

NS_END
