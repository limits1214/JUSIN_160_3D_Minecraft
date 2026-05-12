#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoSkeleton final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoSkeleton, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoSkeleton(const _string& sPath);
	~CResEnttGeoSkeleton() override;

public:
	static SPtr<CResEnttGeoSkeleton> Create();
};

NS_END
