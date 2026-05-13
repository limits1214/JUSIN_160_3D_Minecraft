#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResItemVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResItemVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		std::pair<StringID, StringID> textureId;
		uint32_t resourceIdx{};
	} DESC;

private:
	explicit CResItemVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResItemVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResItemVIBuffer> Create();
};

NS_END