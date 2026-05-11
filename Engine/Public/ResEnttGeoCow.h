#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoCow final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoCow, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoCow(const _string& sPath);
	~CResEnttGeoCow() override;

public:
	static SPtr<CResEnttGeoCow> Create();
};

NS_END
