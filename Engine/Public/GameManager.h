#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CGameManager final : public CEngineBase
{
private:
	explicit CGameManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CGameManager() override;

public:
	void UpdateGUI();

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};

public:
	static UPtr<CGameManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END