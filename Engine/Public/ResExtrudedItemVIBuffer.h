#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResExtrudedItemVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResExtrudedItemVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		std::pair<StringID, StringID> textureId;
		uint32_t resourceIdx{};
		uint32_t texIndex{};
	} DESC;

private:
	explicit CResExtrudedItemVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResExtrudedItemVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResExtrudedItemVIBuffer> Create();
};

NS_END