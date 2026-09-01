#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResCloudVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResCloudVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		//std::pair<StringID, StringID> textureId;
		//uint32_t resourceIdx{};
		//uint32_t texIndices[ETOUI(FACE_DIR::END)]{};
	} DESC;

private:
	explicit CResCloudVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResCloudVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResCloudVIBuffer> Create();
};

NS_END