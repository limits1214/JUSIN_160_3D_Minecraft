#include "pch.h"
#include "WorldManager.h"
#include "FurnaceStorage.h"
#include "ChestStorage.h"
#include "GameInstance.h"
#include "CameraObject.h"

#include "PigEntity.h"
#include "ZombieEntity.h"
#include "CreeperEntity.h"
#include "SkeletonEntity.h"
#include "ChickenEntity.h"
#include "CowEntity.h"

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

	if (ImGui::Button("RandSpawn"))
	{
		if (auto pObj = CGameInstance::Get().GetGameObjectByHandle(m_hPlayer))
		{
			auto pos = pObj->GetTransform().GetPosition();
			if (FAILED(WorldRandomMonsterGeneration(pos, 20, 1)))
			{
				MSG_BOX("FAIL");
			}
		}
	}
	ImGui::End();
}

void CWorldManager::Update(_float fTimeDelta)
{
	//m_fElapsedTime += fTimeDelta;

	UpdateSunShadowCamera(fTimeDelta);

	m_pFurnaceStorage->Update(fTimeDelta);

	m_TimerMobSpawn.AppendCurrTime(fTimeDelta);
	if (m_TimerMobSpawn.Get_Finished())
	{
		if (auto pObj = CGameInstance::Get().GetGameObjectByHandle(m_hPlayer))
		{
			auto pos = pObj->GetTransform().GetPosition();
			if (FAILED(WorldRandomMonsterGeneration(pos, 32.f * 1.5f, 1)))
			{
				//MSG_BOX("FAIL");
			}
		}
	}

}

HRESULT CWorldManager::Initialize()
{
	m_pFurnaceStorage = CFurnaceStorage::Create();
	m_pChestStorage = CChestStorage::Create();

	m_TimerMobSpawn.Set_GoalTime(1.f);
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

//HRESULT WWorldRandomMonsterGeneration(_float3 vCenterPos, _float fRadius, uint32_t iCnt)
//{
//	// 이거 이상은 안됨
//	uint32_t iTotalEntityCnt = 100;
//
//	// 만약 주어진센터 포스 주변을 넘어간 애들이 있다면 디스폰해준다.
//	_float fDespawnLimit = 32.f * 5.f;
//
//	// 모든 엔티티는 여기에 들어간다.
//	auto layerName = "02_ENTITY_SPAWNER";
//	auto pLayer = CGameInstance::Get().GetGameObjectLayer(layerName);
//
//	if (pLayer)
//	{
//		for (auto& h : *pLayer)
//		{
//			if (auto pObj = CGameInstance::Get().GetGameObjectByHandle(h))
//			{
//				_float3 vObjPos = pObj->GetTransform().GetPosition();
//				_float fDistSq = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&vObjPos) - XMLoadFloat3(&vCenterPos)));
//
//				if (fDistSq > (fDespawnLimit * fDespawnLimit))
//				{
//					pObj->SetPendingDestroyCascade();
//				}
//			}
//		}
//	}
//	
//
//
//	// 주어진 라디우스 기반 스폰 위치 결정
//	auto vCopyCenterPos = vCenterPos;
//	vCopyCenterPos.x += Randf(-fRadius, fRadius);
//	vCopyCenterPos.y += Randf(-fRadius, fRadius);
//	vCopyCenterPos.z += Randf(-fRadius, fRadius);
//
//	// xyz
//	// 02_ENTITY_SPAWNER 순회돌면서 위치 저장
//	int32_t blockX = static_cast<int32_t>(std::floor(vCopyCenterPos.x));
//	int32_t blockY = static_cast<int32_t>(std::floor(vCopyCenterPos.y));
//	int32_t blockZ = static_cast<int32_t>(std::floor(vCopyCenterPos.z));
//	//auto optBlock = CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ);
//	//if (optBlock->GetType() == CBlock3::TYPE::AIR)
//	//{
//
//	//}
//
//	// 동굴도 있다.
//	// 1.y를 기준으로 위로 스캔  
//	// 2.y를 기준으로 아래로 스캔 
//	// 땅(!=AIR) 위 AIR2칸 있어야한다.
//	// 
//	// 찾기 시도 횟수 제한
//	// 전부 못하면 FAIL
//	// 기존 몬스터 동일 위치에 스폰하면 안됨
//
//	int32_t nScanRange = 10;
//	bool bFound = false;
//	_float3 vSpawnPos = vCopyCenterPos;
//
//	for (int32_t yOffset = -nScanRange; yOffset <= nScanRange; ++yOffset)
//	{
//		int32_t curY = blockY + yOffset;
//
//		// 바닥이 고체 블록(AIR가 아님)인지 확인
//		auto pGround = CGameInstance::Get().GetVoxelBlock(blockX, curY, blockZ);
//		// 그 위의 두 칸이 AIR인지 확인
//		auto pAir1 = CGameInstance::Get().GetVoxelBlock(blockX, curY + 1, blockZ);
//		auto pAir2 = CGameInstance::Get().GetVoxelBlock(blockX, curY + 2, blockZ);
//
//
//		if (pGround->GetType() != CBlock3::TYPE::AIR &&
//			pGround->GetType() != CBlock3::TYPE::WATER_FLOW &&
//			pGround->GetType() != CBlock3::TYPE::WATER_STILL &&
//			pAir1->GetType() == CBlock3::TYPE::AIR &&
//			pAir1->GetType() != CBlock3::TYPE::WATER_FLOW &&
//			pAir1->GetType() != CBlock3::TYPE::WATER_STILL &&
//			pAir2->GetType() != CBlock3::TYPE::WATER_FLOW &&
//			pAir2->GetType() != CBlock3::TYPE::WATER_STILL &&
//			pAir2->GetType() == CBlock3::TYPE::AIR)
//		{
//			vSpawnPos.y = static_cast<_float>(curY + 1); // 발 위치
//			bFound = true;
//			break;
//		}
//	}
//
//	// 몬스터 스폰 일단 돼지 해보자 나중에 확률로 다양하게 할 예정
//	E::CPigEntity::DESC Desc{};
//	Desc.sObjectTag = "Pig";
//	if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PigEntity",
//		layerName, &Desc))
//	{
//		if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CPigEntity>(handle.value()))
//		{
//			pObj->GetTransform().SetPosition(vSpawnPos);
//			//pObj->SetPlayer(m_hPlayer);
//		}
//	}
//
//	return S_OK;
//}

HRESULT CWorldManager::WorldRandomMonsterGeneration(_float3 vCenterPos, _float fRadius, uint32_t iCnt)
{
	// 1. 엔티티 최대 개수 제한
	uint32_t iTotalEntityCnt = 100;

	// 2. 중심(플레이어) 기준 디스폰 반경 설정
	_float fDespawnLimit = fRadius;
	auto layerName = "02_ENTITY_SPAWNER";
	auto pLayer = CGameInstance::Get().GetGameObjectLayer(layerName);

	// 3. 기존 엔티티 순회 및 디스폰 처리
	if (pLayer)
	{
		uint32_t iSize{};
		for (auto& h : *pLayer)
		{
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandle(h))
			{
				++iSize;
				_float3 vObjPos = pObj->GetTransform().GetPosition();
				_float fDistSq = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&vObjPos) - XMLoadFloat3(&vCenterPos)));

				if (fDistSq > (fDespawnLimit * fDespawnLimit))
				{
					pObj->SetPendingDestroyCascade();
				}
			}
		}

		// 만약 현재 레이어의 엔티티 수가 최대 제한을 넘었다면 추가 스폰 중단
		if (iSize >= iTotalEntityCnt)
			return E_FAIL;

	}

	// 4. 주어진 반경 내 스폰 X, Z 좌표 랜덤 결정
	auto vCopyCenterPos = vCenterPos;
	vCopyCenterPos.x += Randf(-fRadius, fRadius);
	vCopyCenterPos.z += Randf(-fRadius, fRadius);
	// Y축은 지형에 맞출 것이므로 랜덤에서 제외하거나, 기준점으로만 둡니다.

	int32_t blockX = static_cast<int32_t>(std::floor(vCopyCenterPos.x));
	int32_t blockY = static_cast<int32_t>(std::floor(vCopyCenterPos.y));
	int32_t blockZ = static_cast<int32_t>(std::floor(vCopyCenterPos.z));

	int32_t nScanRange = 10;
	bool bFound = false;
	_float3 vSpawnPos = {};

	// 5. Y축 스캔 (바닥 및 충돌체 검사)
	for (int32_t yOffset = -nScanRange; yOffset <= nScanRange; ++yOffset)
	{
		int32_t curY = blockY + yOffset;

		auto pGround = CGameInstance::Get().GetVoxelBlock(blockX, curY, blockZ);
		auto pAir1 = CGameInstance::Get().GetVoxelBlock(blockX, curY + 1, blockZ);
		auto pAir2 = CGameInstance::Get().GetVoxelBlock(blockX, curY + 2, blockZ);
		if (!pGround || !pAir1 || !pAir2)
		{
			continue;
		}

		// 조건 1: 바닥이 고체 블록(AIR가 아님)이고, 위 두 칸이 AIR인지 확인
		if (pGround->GetType() != CBlock3::TYPE::AIR &&
			pGround->GetType() != CBlock3::TYPE::WATER_FLOW &&
			pGround->GetType() != CBlock3::TYPE::WATER_STILL &&
			pAir1->GetType() == CBlock3::TYPE::AIR &&
			pAir2->GetType() == CBlock3::TYPE::AIR)
		{
			// 잠재적 스폰 위치 계산 (블록의 중앙을 맞추기 위해 x, z에 0.5를 더함)
			_float3 potentialPos = {
				static_cast<_float>(blockX) + 0.5f,
				static_cast<_float>(curY + 1), // 발 위치
				static_cast<_float>(blockZ) + 0.5f
			};

			// 조건 2: 스폰될 공간에 다른 엔티티가 없는지 AABB 충돌 검사
			_float3 overlapPos = potentialPos;
			overlapPos.y += 0.5f; // AABB 중심점을 엔티티의 대략적인 중앙으로 올림
			_float3 halfExt = { 0.4f, 0.4f, 0.4f }; // 몬스터(돼지)의 대략적인 AABB 반경

			if (!CGameInstance::Get().VoxelAABBOverlap(overlapPos, halfExt))
			{
				vSpawnPos = potentialPos;
				bFound = true;
				break; // 완벽한 위치를 찾았으므로 스캔 종료
			}
		}
	}

	// 6. 위치를 찾지 못했다면 스폰 취소
	if (!bFound)
	{
		return E_FAIL; // 적절한 스폰 위치를 못 찾았으므로 다음 프레임/주기를 기약
	}

	// 7. 몬스터(돼지) 스폰

	auto randIdx = RandInt(0, 5);
	if (randIdx == 0)
	{
		E::CPigEntity::DESC Desc{};
		Desc.sObjectTag = "Pig";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PigEntity", layerName, &Desc))
		{
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CPigEntity>(handle.value()))
			{
				pObj->GetTransform().SetPosition(vSpawnPos);
				pObj->SetPlayer(m_hPlayer);
			}
		}
	}
	else if (randIdx == 1)
	{
		E::CCowEntity::DESC Desc{};
		Desc.sObjectTag = "Cow";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_CowEntity", layerName, &Desc))
		{
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CCowEntity>(handle.value()))
			{
				pObj->GetTransform().SetPosition(vSpawnPos);
				pObj->SetPlayer(m_hPlayer);
			}
		}
	}
	else if (randIdx == 2)
	{
		E::CChickenEntity::DESC Desc{};
		Desc.sObjectTag = "Chicken";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ChickenEntity", layerName, &Desc))
		{
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CChickenEntity>(handle.value()))
			{
				pObj->GetTransform().SetPosition(vSpawnPos);
				pObj->SetPlayer(m_hPlayer);
			}
		}
	}
	else if (randIdx == 3)
	{
		E::CZombieEntity::DESC Desc{};
		Desc.sObjectTag = "Zombie";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ZombieEntity", layerName, &Desc))
		{
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CZombieEntity>(handle.value()))
			{
				pObj->GetTransform().SetPosition(vSpawnPos);
				pObj->SetPlayer(m_hPlayer);
			}
		}
	}
	else if (randIdx == 4)
	{
		E::CSkeletonEntity::DESC Desc{};
		Desc.sObjectTag = "Skeleton";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_SkeletonEntity", layerName, &Desc))
		{
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CSkeletonEntity>(handle.value()))
			{
				pObj->GetTransform().SetPosition(vSpawnPos);
				pObj->SetPlayer(m_hPlayer);
			}
		}
	}
	else if (randIdx == 5)
	{
		E::CCreeperEntity::DESC Desc{};
		Desc.sObjectTag = "Creeper";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_CreeperEntity", layerName, &Desc))
		{
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CCreeperEntity>(handle.value()))
			{
				pObj->GetTransform().SetPosition(vSpawnPos);
				pObj->SetPlayer(m_hPlayer);
			}
		}
	}
	

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