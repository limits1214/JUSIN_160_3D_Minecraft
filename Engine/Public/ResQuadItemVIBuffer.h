#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResQuadItemVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResQuadItemVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		std::pair<StringID, StringID> textureId;
		uint32_t resourceIdx{};
	} DESC;

private:
	explicit CResQuadItemVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResQuadItemVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResQuadItemVIBuffer> Create();
};

NS_END