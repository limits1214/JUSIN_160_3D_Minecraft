#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResCubeItemVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResCubeItemVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		std::pair<StringID, StringID> textureId;
		//uint32_t resourceIdx{};
		//uint32_t texIndices[ETOUI(FACE_DIR::END)]{};
	} DESC;

private:
	explicit CResCubeItemVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResCubeItemVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResCubeItemVIBuffer> Create();
};

NS_END