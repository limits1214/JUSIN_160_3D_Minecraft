#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoPig final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoPig, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoPig(const _string& sPath);
	~CResEnttGeoPig() override;

public:
	static SPtr<CResEnttGeoPig> Create();
};

NS_END
