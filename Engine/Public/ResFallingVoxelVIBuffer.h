#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResFallingVoxelVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResFallingVoxelVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		std::pair<StringID, StringID> textureId;
		//uint32_t resourceIdx{};
		uint32_t texIndices[ETOUI(FACE_DIR::END)]{};
	} DESC;

private:
	explicit CResFallingVoxelVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResFallingVoxelVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResFallingVoxelVIBuffer> Create();
};

NS_END