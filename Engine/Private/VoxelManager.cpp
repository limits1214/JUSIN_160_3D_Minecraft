#include "VoxelManager.h"

NS_USING(Engine)

CVoxelManager::CVoxelManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{pDevice}
    , m_pContext{pContext}
{
}

CVoxelManager::~CVoxelManager()
{
}

void CVoxelManager::UpdateGUI()
{
}

UPtr<CVoxelManager> CVoxelManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return ToUPtr(new CVoxelManager{ pDevice , pContext });
}
