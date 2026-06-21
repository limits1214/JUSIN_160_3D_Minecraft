#include "ExperienceOrb.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CameraObject.h"
#include "CollBox.h"
#include "ComConstantBuffer.h"
NS_USING(Engine)

CExperienceOrb::CExperienceOrb()
{
}

CExperienceOrb::~CExperienceOrb()
{
}

HRESULT CExperienceOrb::Initialize(void* pArg)
{
	auto pDesc = static_cast<DESC*>(pArg);
	m_hPlayer = pDesc->hPlayer;
	if (FAILED(CEntityObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

	if (auto res = CResDynamicBuffer::Create())
	{
		CResDynamicBuffer::DESC Desc{};
		Desc.desc = {
			.ByteWidth = ((uint32_t)sizeof(VTX_EXP_ORB_INSTANCED_DATA) * m_iNumElements),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0,
		};

		if (FAILED(res->Load(Desc)))
		{
			return E_FAIL;
		}
		m_pResInstancedBuffer = res;
	}

	{
		CComConstantBuffer::DESC Desc{};
		Desc.cBufferId = { TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT };
		if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_ConstantBuffer", "ComCBufferPerObject", &Desc, &m_pComCBufferPerObject)))
		{
			return E_FAIL;
		};
	}

	return S_OK;
}

void CExperienceOrb::PriorityUpdate(E::_float fTimeDelta)
{
}

void CExperienceOrb::Update(E::_float fTimeDelta)
{
	CGameObject* pPlayerObj = CGameInstance::Get().GetGameObjectByHandle(m_hPlayer);

	//  발밑이 아닌 가슴/몸통 쪽으로 날아오도록 Y축을 살짝 올려줍니다.
	_float3 playerPos = pPlayerObj->GetTransform().GetPosition();
	playerPos.y += 1.0f;
	XMVECTOR vTargetPos = XMLoadFloat3(&playerPos);

	m_vecInstancedData.clear();

	auto pGameCam = E::CGameInstance::Get().GetActiveGameCamera();
	_matrix matCamWorld = pGameCam->GetTransform().GetLoadedWorldMatrix();

	XMVECTOR vCamRight = XMVector3Normalize(matCamWorld.r[0]);
	XMVECTOR vCamUp = XMVector3Normalize(matCamWorld.r[1]);
	XMVECTOR vCamLook = XMVector3Normalize(matCamWorld.r[2]);

	_float3 scale = GetTransform().GetScale();
	float frameSize = 0.25f;

	for (auto& item : m_listOrbs)
	{
		XMVECTOR vOrbPos = XMLoadFloat3(&item.vPos);
		XMVECTOR vToPlayer = vTargetPos - vOrbPos;
		float fDist = XMVectorGetX(XMVector3Length(vToPlayer));

		// 자석 거리 판정 (예: 6블록 이내면 끌려감)
		_bool bMagnet = (fDist < 6.0f);

		// 3. 물리 및 이동 업데이트 (다가가는 로직 실행)
		VelocityUpdate(item, fTimeDelta, bMagnet, vToPlayer);

		// 개별 오브 타이머 진행
		item.fBobTime += fTimeDelta;

		// 빌보드 행렬 조립
		_matrix matBillboard = XMMatrixIdentity();
		matBillboard.r[0] = XMVectorSetW(vCamRight * scale.x, 0.f);
		matBillboard.r[1] = XMVectorSetW(vCamUp * scale.y, 0.f);
		matBillboard.r[2] = XMVectorSetW(vCamLook * scale.z, 0.f);

		//_matrix matBob = XMMatrixTranslation(0.f, item.fBobYOffset, 0.f);
		_matrix matTranslation = XMMatrixTranslation(item.vPos.x, item.vPos.y, item.vPos.z);

		XMStoreFloat4x4(&item.matWorld, matBillboard *  matTranslation);

		item.boxCollider->Transform(XMMatrixTranslation(item.vPos.x, item.vPos.y, item.vPos.z));
		E::CGameInstance::Get().AddColliderGroup("Coll_ExpOrb", item.boxCollider.get());

		// --- 인스턴스 데이터 세팅 ---
		VTX_EXP_ORB_INSTANCED_DATA inst{};
		inst.matWorld = item.matWorld;

		uint32_t safeType = item.iType % 16;
		int col = safeType % 4;
		int row = safeType / 4;
		inst.uvOffset = { col * frameSize, row * frameSize };

		float fSine = sinf(item.fBobTime * 12.0f) * 0.5f + 0.5f;
		XMVECTOR vColorA = XMVectorSet(0.0f, 1.0f, 0.1f, 1.0f);
		XMVECTOR vColorB = XMVectorSet(0.8f, 1.0f, 0.0f, 1.0f);
		XMVECTOR vFinalColor = XMVectorLerp(vColorA, vColorB, fSine);
		XMStoreFloat4(&inst.vColor, vFinalColor);

		// 라이트 처리
		int32_t blockX = static_cast<int32_t>(std::floor(item.vPos.x));
		int32_t blockY = static_cast<int32_t>(std::floor(item.vPos.y));
		int32_t blockZ = static_cast<int32_t>(std::floor(item.vPos.z));
		if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
		{
			inst.light = optCurrBlock->GetLight();
		}

		m_vecInstancedData.push_back(inst);
	}
}

void CExperienceOrb::LateUpdate(E::_float fTimeDelta)
{
	E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	GetTransform().Update();

}

HRESULT CExperienceOrb::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_ExperienceOrb");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_ExperienceOrb");
	const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResExperenceOrbVIBuffer>("MC_ITEM_VIBuffer", "ExperienceOrb");

	pContext->IASetInputLayout(vs->GetInputLayout().Get());
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

	ID3D11Buffer* vertexBuffers[] = {
				viBuffer->GetVertexBuffer().Get(),
				m_pResInstancedBuffer->GetBuffer().Get()
	};
	uint32_t strides[] = {
		viBuffer->GetVertexStride(),
		(uint32_t)sizeof(VTX_EXP_ORB_INSTANCED_DATA),
	};
	uint32_t offsets[] = {
		0,
		0,
	};
	pContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
	pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

	{
		auto pCbPerObject = m_pResInstancedBuffer;
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;

		if (SUCCEEDED(pContext->Map(pCbPerObject->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
			std::memcpy(mappedSubResource.pData, m_vecInstancedData.data(), sizeof(VTX_EXP_ORB_INSTANCED_DATA) * m_vecInstancedData.size());
			pContext->Unmap(pCbPerObject->GetBuffer().Get(), 0);
		}
	}

	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}

	//pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	pContext->DrawIndexedInstanced((UINT)viBuffer->GetNumIndices(), (UINT)m_vecInstancedData.size(), 0, 0, 0);
	return S_OK;
}

void CExperienceOrb::AddOrb(const _float3& vPos, const _float3& vVelocity, uint32_t iType)
{
	InstancedExpOrbDesc Desc{};
	Desc.vPos = vPos;
	Desc.vVelocity = vVelocity;
	Desc.iType = iType;
	Desc.boxCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.25f, 0.25f });
	Desc.boxCollider->SetInnerPointer(this);
	Desc.boxCollider->SetInnerHint("CDropBlock");
	m_listOrbs.push_back(Desc);
	CollHint hint{};
	hint.iter = std::prev(m_listOrbs.end());
	Desc.boxCollider->SetInnerHint2(&hint, sizeof(hint));
}

void CExperienceOrb::VelocityUpdate(InstancedExpOrbDesc& item, E::_float fTimeDelta, _bool bMagnet, XMVECTOR vToPlayer)
{
	XMVECTOR vVel = XMLoadFloat3(&item.vVelocity);

	if (bMagnet)
	{
		//  자석 모드: 기존 물리(중력/마찰) 무시하고 플레이어를 향해 가속
		XMVECTOR vDir = XMVector3Normalize(vToPlayer);

		// 점점 빨라지도록 가속도 부여 (수치는 취향껏 조절, 현재 초당 35.0f 가속)
		vVel += vDir * 35.0f * fTimeDelta;

		// 너무 총알처럼 날아가지 않게 최대 속도 제한
		float fSpeed = XMVectorGetX(XMVector3Length(vVel));
		if (fSpeed > 15.0f)
		{
			vVel = vDir * 15.0f;
		}
		item.bOnGround = false; // 끌려갈 때는 공중에 뜸
	}
	else
	{
		//  일반 물리 로직 (자석 범위 밖일 때)
		XMVECTOR vWishDir = XMVectorZero();

		// 가속
		float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
		float fAddSpeed = m_fSpeed - fCurrSpeed;
		if (fAddSpeed > 0.f)
		{
			float fAccelSpeed = std::min(18.f * m_fSpeed * fTimeDelta, fAddSpeed);
			vVel += vWishDir * fAccelSpeed;
		}

		// 수평 속도 제한
		XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
		float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
		if (fHorizSpeed > m_fSpeed)
		{
			vHoriz = XMVector3Normalize(vHoriz) * m_fSpeed;
			vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
		}

		// 마찰
		if (item.bOnGround)
		{
			float fSpeed = XMVectorGetX(XMVector3Length(XMVectorSetY(vVel, 0.f)));
			if (fSpeed > 0.f)
			{
				float fNewSpeed = std::max(fSpeed - fSpeed * 15.f * fTimeDelta, 0.f);
				float vy = XMVectorGetY(vVel);
				vVel = XMVectorSetY(vVel * (fNewSpeed / fSpeed), vy);
			}
		}

		// 중력
		if (!item.bOnGround)
			vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);
	}

	// ==========================================
	// AABB 복셀 벽/바닥 충돌 처리
	// ==========================================
	const XMFLOAT3 halfExtents = { 0.25f, 0.25f, 0.25f };
	XMFLOAT3 c = item.vPos;

	// Y
	float velY = XMVectorGetY(vVel);
	float prevY = c.y;
	c.y += velY * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		if (velY < 0.f)
		{
			c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
			item.bOnGround = true;
		}
		else c.y = prevY;

		//  자석 모드일 땐 벽에 막혀도 위아래로 비비면서 넘어갈 수 있도록 속도를 0으로 죽이지 않음
		if (!bMagnet) vVel = XMVectorSetY(vVel, 0.f);
	}
	else item.bOnGround = false;

	// X
	float px = c.x;
	c.x += XMVectorGetX(vVel) * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		c.x = px;
		if (!bMagnet) vVel = XMVectorSetX(vVel, 0.f);
	}

	// Z
	float pz = c.z;
	c.z += XMVectorGetZ(vVel) * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		c.z = pz;
		if (!bMagnet) vVel = XMVectorSetZ(vVel, 0.f);
	}

	item.vPos = c;
	XMStoreFloat3(&item.vVelocity, vVel);
}

//
//void CExperienceOrb::VelocityUpdate(InstancedExpOrbDesc& item, E::_float fTimeDelta)
//{
//	XMVECTOR vVel = XMLoadFloat3(&item.vVelocity);
//	XMVECTOR vWishDir = XMVectorZero();
//
//	// 가속
//	float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
//	float fAddSpeed = m_fSpeed - fCurrSpeed;
//	if (fAddSpeed > 0.f)
//	{
//		float fAccelSpeed = std::min(18.f * m_fSpeed * fTimeDelta, fAddSpeed);
//		vVel += vWishDir * fAccelSpeed;
//	}
//
//	// 수평 속도 제한
//	XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
//	float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
//	if (fHorizSpeed > m_fSpeed)
//	{
//		vHoriz = XMVector3Normalize(vHoriz) * m_fSpeed;
//		vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
//	}
//
//	// 마찰
//	if (item.bOnGround)
//	{
//		float fSpeed = XMVectorGetX(XMVector3Length(XMVectorSetY(vVel, 0.f)));
//		if (fSpeed > 0.f)
//		{
//			float fNewSpeed = std::max(fSpeed - fSpeed * 15.f * fTimeDelta, 0.f);
//			float vy = XMVectorGetY(vVel);
//			vVel = XMVectorSetY(vVel * (fNewSpeed / fSpeed), vy);
//		}
//	}
//
//	// 중력
//	if (!item.bOnGround)
//		vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);
//
//	// AABB 충돌
//	const XMFLOAT3 halfExtents = { 0.25f, 0.25f, 0.25f };
//	XMFLOAT3 c = item.vPos;
//
//	// Y
//	float velY = XMVectorGetY(vVel);
//	float prevY = c.y;
//	c.y += velY * fTimeDelta;
//	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
//	{
//		if (velY < 0.f)
//		{
//			c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
//			item.bOnGround = true;
//		}
//		else c.y = prevY;
//		vVel = XMVectorSetY(vVel, 0.f);
//	}
//	else item.bOnGround = false;
//
//	// X
//	float px = c.x;
//	c.x += XMVectorGetX(vVel) * fTimeDelta;
//	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
//	{
//		c.x = px;
//		vVel = XMVectorSetX(vVel, 0.f);
//	}
//
//	// Z
//	float pz = c.z;
//	c.z += XMVectorGetZ(vVel) * fTimeDelta;
//	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
//	{
//		c.z = pz;
//		vVel = XMVectorSetZ(vVel, 0.f);
//	}
//
//	item.vPos = c;
//	XMStoreFloat3(&item.vVelocity, vVel);
//}

UPtr<CExperienceOrb> CExperienceOrb::Create()
{
	auto pInstance = ToUPtr(new CExperienceOrb{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CExperienceOrb");
		return nullptr;
	}

	return pInstance;
}

UPtr<CPrototype> CExperienceOrb::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CExperienceOrb{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CExperienceOrb");
		return nullptr;
	}

	return pInstance;
}
