//#include "pch.h"
#include "WorldManager.h"
#include "FurnaceStorage.h"
#include "ChestStorage.h"
#include "GameInstance.h"
#include "CameraObject.h"

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
	ImGui::Begin("WorldManager");

	auto fDay = GetDayFactor();
	ImGui::InputFloat("Quat", (float*)&fDay, ImGuiInputTextFlags_ReadOnly);
	ImGui::End();
}

void CWorldManager::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	UpdateSunShadowCamera(fTimeDelta);

	m_pFurnaceStorage->Update(fTimeDelta);
}

HRESULT CWorldManager::Initialize()
{
	m_pFurnaceStorage = CFurnaceStorage::Create();
	m_pChestStorage = CChestStorage::Create();
	return S_OK;
}

void CWorldManager::UpdateSunShadowCamera(_float fTimeDelta)
{
	if (auto pShadowCamera = CGameInstance::Get().GetGameCamera("Shadow"))
	{
		_matrix matSkyRotation = XMMatrixRotationX(CGameInstance::Get().GetWorldSkyRotation());

		_vector vSunDirLocal = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vSunDirWorld = XMVector3Normalize(
			XMVector3TransformNormal(vSunDirLocal, matSkyRotation));

		// 메인 카메라 위치를 그림자 중심으로 사용
		if (auto pMainCam = CGameInstance::Get().GetGameCamera("Player"))
		{
			_vector vSceneCenter = pMainCam->GetTransform().GetLoadedPostion();

			constexpr _float fShadowDist = 150.f;
			_vector vLightEye = vSceneCenter + vSunDirWorld * fShadowDist;

			_float3 vEye, vAt;
			XMStoreFloat3(&vEye, vLightEye);
			XMStoreFloat3(&vAt, vSceneCenter);

			pShadowCamera->GetTransform().SetPosition(vEye);
			pShadowCamera->GetTransform().LookAt(XMLoadFloat3(&vAt));
		}
		
	}
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