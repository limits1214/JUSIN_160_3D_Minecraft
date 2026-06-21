#pragma once

#include "ResVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttVIBuffer final : public CResVIBuffer
{
public:
	DECLARE_DERIVED_TYPE(CResEnttVIBuffer, CResVIBuffer)

public:
	typedef struct tagDesc
	{
		std::pair<StringID, StringID> geometryId{};
		std::optional<uint32_t> baseTexId{};
		std::unordered_map<_string, uint32_t> specificCubeTexIds{};
		_bool bUseFlatQuad{ false };
	} DESC;

private:
	explicit CResEnttVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CResEnttVIBuffer() override;

public:

public:
	HRESULT Load(const std::any& arg = {}) override;
	HRESULT Unload(const std::any& arg = {})  override;

public:
	static SPtr<CResEnttVIBuffer> Create();
};

NS_END