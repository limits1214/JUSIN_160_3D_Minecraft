#include "GameManager.h"

NS_USING(Engine)

CGameManager::CGameManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
}

CGameManager::~CGameManager()
{
}

void CGameManager::UpdateGUI()
{
}

UPtr<CGameManager> CGameManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return UPtr<CGameManager>();
}
