#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoEnderMan final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoEnderMan, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoEnderMan(const _string& sPath);
	~CResEnttGeoEnderMan() override;

public:
	static SPtr<CResEnttGeoEnderMan> Create();
};

NS_END
