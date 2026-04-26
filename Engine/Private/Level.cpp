#include "pch.h"
#include "Level.h"

NS_USING(Engine)

CLevel::CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}

CLevel::~CLevel()
{

}

HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
    return S_OK;
}

void CLevel::FrameStart(_float fTimeDelta)
{
}

void CLevel::FrameEnd(_float fTimeDelta)
{
}

void CLevel::UpdateGUI()
{
}

