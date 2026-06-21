#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoArrow final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoArrow, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoArrow(const _string& sPath);
	~CResEnttGeoArrow() override;

public:
	static SPtr<CResEnttGeoArrow> Create();
};

NS_END
