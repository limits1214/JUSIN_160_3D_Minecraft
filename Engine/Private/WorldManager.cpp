//#include "pch.h"
#include "WorldManager.h"
#include "FurnaceStorage.h"
#include "ChestStorage.h"

NS_USING(Engine)

CWorldManager::CWorldManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice {pDevice}
	, m_pContext{pContext}
{
}


CWorldManager::~CWorldManager()
{
}


void CWorldManager::UpdateGUI()
{
	
}

void CWorldManager::Update(_float fTimeDelta)
{
	m_pFurnaceStorage->Update(fTimeDelta);
}

HRESULT CWorldManager::Initialize()
{
	m_pFurnaceStorage = CFurnaceStorage::Create();
	m_pChestStorage = CChestStorage::Create();
	return S_OK;
}

UPtr<CWorldManager> CWorldManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = ToUPtr(new CWorldManager{ pDevice, pContext });
	if (FAILED(pInstance->Initialize()))
	{
		return nullptr;
	}
	return pInstance;
}