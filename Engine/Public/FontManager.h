#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CFontManager final : public CEngineBase
{
private:
	explicit CFontManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CFontManager() override;

public:
	void UpdateGUI();

public:
	void Draw(const StringID& fontName, const _tchar* pText, const _float2& vPosition, float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin);

private:
	HRESULT Initialize();

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};

	std::unique_ptr<SpriteBatch> m_pBatch = { };

public:
	static UPtr<CFontManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END