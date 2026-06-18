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
	
	ImGui::Text("%f", fDay);
	ImGui::DragFloat("Quat", (float*)&m_fElapsedTime);
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
	static float m_fShadowTimer = 0.f;

	m_fShadowTimer += fTimeDelta;

	if (m_fShadowTimer < 1.f)
		return;

	m_fShadowTimer = 0.f;


	auto pShadowCamera =
		CGameInstance::Get().GetGameCamera("Shadow");

	if (!pShadowCamera)
		return;


	_matrix matSkyRotation =
		XMMatrixRotationX(
			CGameInstance::Get().GetWorldSkyRotation());


	_vector vSunDir =
		XMVector3Normalize(
			XMVector3TransformNormal(
				XMVectorSet(0.f, 1.f, 0.f, 0.f),
				matSkyRotation));



	auto pMainCam =
		CGameInstance::Get().GetGameCamera("Player");

	if (!pMainCam)
		return;


	_vector vCenter =
		pMainCam->GetTransform()
		.GetLoadedPostion();



	constexpr float fShadowDist = 150.f;


	_vector vEye =
		vCenter + vSunDir * fShadowDist;



	// texel snap

	constexpr float ShadowSize = 128.f;
	constexpr float ShadowMap = 4096.f;

	float texel =
		ShadowSize / ShadowMap;


	_float3 eye;

	XMStoreFloat3(&eye, vEye);


	eye.x =
		floorf(eye.x / texel + 0.5f) * texel;

	eye.y =
		floorf(eye.y / texel + 0.5f) * texel;

	eye.z =
		floorf(eye.z / texel + 0.5f) * texel;


	vEye = XMLoadFloat3(&eye);



	// 중요
	// 플레이어가 아니라 태양 방향으로 바라봄

	_vector vAt =
		vEye - vSunDir * fShadowDist;



	// up 고정

	_vector vUp =
		XMVectorSet(0.f, 1.f, 0.f, 0.f);



	float d =
		XMVectorGetX(
			XMVector3Dot(
				XMVector3Normalize(vAt - vEye),
				vUp));


	if (fabsf(d) > 0.99f)
	{
		vUp =
			XMVectorSet(0.f, 0.f, 1.f, 0.f);
	}



	_float3 eye3;
	_float3 at3;


	XMStoreFloat3(&eye3, vEye);
	XMStoreFloat3(&at3, vAt);



	auto& tr =
		pShadowCamera->GetTransform();


	tr.SetPosition(eye3);


	tr.LookAt(
		XMLoadFloat3(&at3));


	pShadowCamera->UpdateViewMatrix();
}


void UpdateSunShadowCamera2(_float fTimeDelta)
{
	static float m_fShadowTimer = 0.f;
	m_fShadowTimer += fTimeDelta;

	if (m_fShadowTimer < 1.f)
		return;

	m_fShadowTimer = 0.f;


	auto pShadowCamera =
		CGameInstance::Get().GetGameCamera("Shadow");

	if (!pShadowCamera)
		return;


	_matrix matSkyRotation =
		XMMatrixRotationX(
			CGameInstance::Get().GetWorldSkyRotation());


	_vector vSunDir =
		XMVector3Normalize(
			XMVector3TransformNormal(
				XMVectorSet(0.f, 1.f, 0.f, 0.f),
				matSkyRotation));


	auto pMainCam =
		CGameInstance::Get().GetGameCamera("Player");

	if (!pMainCam)
		return;


	_vector vCenter =
		pMainCam->GetTransform()
		.GetLoadedPostion();



	constexpr float fShadowDist = 150.f;


	_vector vEye =
		vCenter + vSunDir * fShadowDist;


	// texel snap
	constexpr float ShadowSize = 128.f;
	constexpr float ShadowMap = 2048.f;

	float texel =
		ShadowSize / ShadowMap;


	_float3 eye;
	XMStoreFloat3(&eye, vEye);


	eye.x =
		floorf(eye.x / texel + 0.5f) * texel;

	eye.y =
		floorf(eye.y / texel + 0.5f) * texel;

	eye.z =
		floorf(eye.z / texel + 0.5f) * texel;


	vEye = XMLoadFloat3(&eye);



	_vector vAt =
		vCenter;


	_float3 eye3;
	_float3 at3;


	XMStoreFloat3(&eye3, vEye);
	XMStoreFloat3(&at3, vAt);



	auto& tr =
		pShadowCamera->GetTransform();


	tr.SetPosition(eye3);

	tr.LookAt(
		XMLoadFloat3(&at3));

	pShadowCamera->UpdateViewMatrix();
}

//void CWorldManager::UpdateSunShadowCamera(_float fTimeDelta)
//{
//	if (auto pShadowCamera = CGameInstance::Get().GetGameCamera("Shadow"))
//	{
//		_matrix matSkyRotation = XMMatrixRotationX(CGameInstance::Get().GetWorldSkyRotation());
//
//		_vector vSunDirLocal = XMVectorSet(0.f, 1.f, 0.f, 0.f);
//		_vector vSunDirWorld = XMVector3Normalize(
//			XMVector3TransformNormal(vSunDirLocal, matSkyRotation));
//
//		// 메인 카메라 위치를 그림자 중심으로 사용
//		if (auto pMainCam = CGameInstance::Get().GetGameCamera("Player"))
//		{
//			_vector vSceneCenter = pMainCam->GetTransform().GetLoadedPostion();
//
//			constexpr _float fShadowDist = 150.f;
//			_vector vLightEye = vSceneCenter + vSunDirWorld * fShadowDist;
//
//			_float3 vEye, vAt;
//			XMStoreFloat3(&vEye, vLightEye);
//			XMStoreFloat3(&vAt, vSceneCenter);
//
//			pShadowCamera->GetTransform().SetPosition(vEye);
//			pShadowCamera->GetTransform().LookAt(XMLoadFloat3(&vAt));
//		}
//		
//	}
//}

UPtr<CWorldManager> CWorldManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = ToUPtr(new CWorldManager{ pDevice, pContext });
	if (FAILED(pInstance->Initialize()))
	{
		return nullptr;
	}
	return pInstance;
}