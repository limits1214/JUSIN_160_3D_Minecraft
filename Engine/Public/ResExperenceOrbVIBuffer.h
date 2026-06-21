#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResExperenceOrbVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResExperenceOrbVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		//std::pair<StringID, StringID> textureId;
		//uint32_t resourceIdx{};
		//uint32_t texIndex{};
	} DESC;

private:
	explicit CResExperenceOrbVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResExperenceOrbVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResExperenceOrbVIBuffer> Create();
};

NS_END