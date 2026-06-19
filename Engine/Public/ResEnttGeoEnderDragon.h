#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoEnderDragon final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoEnderDragon, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoEnderDragon(const _string& sPath);
	~CResEnttGeoEnderDragon() override;

public:
	static SPtr<CResEnttGeoEnderDragon> Create();
};

NS_END
