#pragma once
#include "ResEnttGeo.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeoPlayer final : public CResEnttGeo
{
public:
	DECLARE_DERIVED_TYPE(CResEnttGeoPlayer, CResEnttGeo)

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {}) override;

private:
	explicit CResEnttGeoPlayer(const _string& sPath);
	~CResEnttGeoPlayer() override;

public:
	static SPtr<CResEnttGeoPlayer> Create();
};

NS_END
