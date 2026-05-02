#include "LightManager.h"

NS_USING(Engine)

CLightManager::CLightManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{pDevice}
    , m_pContext{pContext}
{
}

CLightManager::~CLightManager()
{
}

void CLightManager::UpdateGUI()
{
}

std::optional< DIRECTIONAL_LIGHT> CLightManager::GetDirectionalLight(const StringID& iStr)
{
    auto iter = m_mapDirectionalLights.find(iStr);

    if (iter == m_mapDirectionalLights.end())
    {
        return std::nullopt;
    }

    return iter->second;
}

HRESULT CLightManager::SetDirectionalLight(const StringID& iStr, const std::optional< DIRECTIONAL_LIGHT>& light)
{
    if (light.has_value())
    {
        m_mapDirectionalLights.insert({ iStr, light.value() });
    }
    else
    {
        m_mapDirectionalLights.erase(iStr);
    }
    return S_OK;
}

UPtr<CLightManager> CLightManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return ToUPtr(new CLightManager{ pDevice , pContext });
}
