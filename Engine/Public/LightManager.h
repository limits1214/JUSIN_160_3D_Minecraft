#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CLightManager final: public CEngineBase
{
private:
	explicit CLightManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CLightManager() override;

public:
	void UpdateGUI();

public:
	std::optional< DIRECTIONAL_LIGHT> GetDirectionalLight(const StringID& iStr);
	HRESULT SetDirectionalLight(const StringID& iStr, const std::optional< DIRECTIONAL_LIGHT>& light);

private:
	std::unordered_map<StringID, DIRECTIONAL_LIGHT> m_mapDirectionalLights{};

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};

public:
	static UPtr<CLightManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END