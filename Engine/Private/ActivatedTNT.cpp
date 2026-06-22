#include "ActivatedTNT.h"
#include "GameInstance.h"
#include "Resources.h"
//#include "Block3.h"
#include "VoxelManager3.h"
#include "CollSphere.h"
#include "PigEntity.h"
NS_USING(Engine)
CActivatedTNT::CActivatedTNT()
{
}

CActivatedTNT::CActivatedTNT(const CActivatedTNT& rhs)
	: CVoxelObject(rhs)
{
}

CActivatedTNT::~CActivatedTNT()
{
}

HRESULT CActivatedTNT::Initialize(void* pArg)
{
	auto pDesc = static_cast<DESC*>(pArg);

	if (auto res = CResFallingVoxelVIBuffer::Create())
	{
		uint32_t tmp[ETOUI(FACE_DIR::END)]{ PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)),PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) };
		for (uint32_t i = 0; i < ETOUI(FACE_DIR::END); ++i)
		{
			auto a = PackTexId(9, ETOUI(CBlock3::GetTexType(CBlock3::TYPE::TNT, static_cast<FACE_DIR>(i))));
			memcpy(&tmp[i], &a, sizeof(uint32_t));
		}
		CResFallingVoxelVIBuffer::DESC desc{};
		desc.textureId = { "VOXEL_MANAGER_TEX", "TEXTURES" };
		//desc.resourceIdx = 0;
		memcpy(desc.texIndices, tmp, sizeof(tmp));

		if (FAILED(res->Load(desc)))
		{
			return E_FAIL;
		}
		m_pResVIBuffer = res;
	}

	if (auto res = CResDynamicBuffer::Create())
	{
		//UINT ByteWidth;
		//D3D11_USAGE Usage;
		//UINT BindFlags;
		//UINT CPUAccessFlags;
		//UINT MiscFlags;
		//UINT StructureByteStride;
		CResDynamicBuffer::DESC Desc{};
		Desc.desc = {
			.ByteWidth = m_iElementStride * m_iNumElements,
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
	if (FAILED(CVoxelObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CActivatedTNT::PriorityUpdate(E::_float fTimeDelta)
{
}

void CActivatedTNT::Update(E::_float fTimeDelta)
{
	for (auto iter = m_vecActivatedTNT.begin(); iter != m_vecActivatedTNT.end(); )
	{
		auto& block = *iter;

		VelocityUpdate(fTimeDelta, block);

		block.fElapsedTime += fTimeDelta;
		block.fColorTimer += fTimeDelta;

		// 폭발 시간이 다가올수록 더 빨리
		float flashCycle = (block.fElapsedTime > 2.0f) ? 0.25f : 0.5f;

		if (block.fColorTimer > flashCycle)
		{
			block.fColorwhite = !block.fColorwhite;
			block.fColorTimer = 0.f;
		}

		block.fElapsedTime += fTimeDelta;

		if (block.fElapsedTime > block.fTargetFuseTime)
		{
			CGameInstance::Get().VoxelProcessExplodeBlock(block.vPos.x, block.vPos.y, block.vPos.z, 4.f);
			CGameInstance::Get().AddParticleRenderExplodeSmoke(block.vPos, 25);


			{
				if (auto pCollGroup = CGameInstance::Get().GetColliderGroup("Coll_PigCenter"))
				{
					for (auto& pColl : *pCollGroup)
					{
						auto tmpColl = CCollSphere::Create({ block.vPos.x, block.vPos.y, block.vPos.z }, 4);
						if (pColl->Intersect(*tmpColl))
						{
							if(auto pObj = Cast<CPigEntity>(pColl->GetInnerPointer()))
							{
								pObj->TakeDamage(10);
							}
						}
						
					}
				}
			}

			// tnt 밀어내기
			{
				_float fExplosionRadius = 3.f; // 밀어낼 최대 반경
				_float fMaxPushForce = 10.f;   // 최대 밀어내기 힘 
				XMVECTOR vExplosionCenter = XMLoadFloat3(&block.vPos);

				for (auto& otherTNT : m_vecActivatedTNT)
				{
					// 자기 자신은 밀어내지 않음
					if (&otherTNT == &block)
						continue;

					XMVECTOR vOtherPos = XMLoadFloat3(&otherTNT.vPos);
					XMVECTOR vDist = vOtherPos - vExplosionCenter;
					XMVECTOR vLengthSq = XMVector3LengthSq(vDist);

					_float fDistSq = XMVectorGetX(vLengthSq);

					if (fDistSq < (fExplosionRadius * fExplosionRadius) && fDistSq > 0.001f)
					{
						_float fDist = sqrtf(fDistSq);

						// 거리에 반비례하는 힘 계산 (가까울수록 1.0에 가깝고, 멀수록 0.0에 가까워짐)
						_float fPushRatio = (fExplosionRadius - fDist) / fExplosionRadius;

						// 바깥으로 밀어내는 방향 벡터
						XMVECTOR vPushDir = XMVector3Normalize(vDist);

						// 마인크래프트 특유의 포물선 궤적을 위해 위쪽(Y축)으로 튕겨 오르는 힘 추가
						vPushDir += XMVectorSet(0.f, 0.5f, 0.f, 0.f);
						vPushDir = XMVector3Normalize(vPushDir);

						// 기존 속도에 충격파 속도를 더함
						XMVECTOR vCurrentVel = XMLoadFloat3(&otherTNT.vVelocity);
						XMVECTOR vAddedVel = vPushDir * (fPushRatio * fMaxPushForce);

						XMStoreFloat3(&otherTNT.vVelocity, vCurrentVel + vAddedVel);

						// 강제로 공중에 떴으므로 땅에 닿음 상태 해제
						otherTNT.bOnGround = false;
					}
				}
			}

			iter = m_vecActivatedTNT.erase(iter); // TNT 제거
			continue;
		}

		++iter;
	}
}

void CActivatedTNT::LateUpdate(E::_float fTimeDelta)
{
	E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	GetTransform().Update();

	m_vecInstancedBlockTransform.clear();

	for (const auto& block : m_vecActivatedTNT)
	{
		INSTANCE_DATA instData{};

		if (block.fColorwhite)
		{
			instData.vColor = { 2.f, 2.f, 2.f, 1.f };
		}
		else
		{
			instData.vColor = { 0.8f, 0.8f, 0.8f, 1.f };
		}

		// 월드 변환 행렬 세팅
		XMStoreFloat4x4(&instData.matWorld, XMMatrixTranslationFromVector(XMLoadFloat3(&block.vPos)));
		m_vecInstancedBlockTransform.push_back(instData);
	}
}

HRESULT CActivatedTNT::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_ActivatedTNT");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_ActivatedTNT");
	//const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>("MC_ITEM_VIBuffer", "CubeItemDirt");
	const auto& viBuffer = m_pResVIBuffer;

	pContext->IASetInputLayout(vs->GetInputLayout().Get());
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

	ID3D11Buffer* vertexBuffers[] = {
			viBuffer->GetVertexBuffer().Get(),
			m_pResInstancedBuffer->GetBuffer().Get()
	};
	uint32_t strides[] = {
		viBuffer->GetVertexStride(),
		m_iElementStride,
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
			std::memcpy(mappedSubResource.pData, m_vecInstancedBlockTransform.data(), sizeof(INSTANCE_DATA) * m_vecInstancedBlockTransform.size());
			pContext->Unmap(pCbPerObject->GetBuffer().Get(), 0);
		}
	}

	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}

	pContext->DrawIndexedInstanced((UINT)viBuffer->GetNumIndices(), (UINT)m_vecInstancedBlockTransform.size(), 0, 0, 0);
	return S_OK;
}
void CActivatedTNT::VelocityUpdate(E::_float fTimeDelta, SActivatedTNTData& tntData)
{
	XMVECTOR vVel = XMLoadFloat3(&tntData.vVelocity);

	// 1. 마찰 및 중력 처리
	if (tntData.bOnGround)
	{
		float fFriction = powf(0.01f, fTimeDelta);
		vVel *= XMVectorSet(fFriction, 1.f, fFriction, 1.f);

		if (fabsf(XMVectorGetX(vVel)) < 0.01f) vVel = XMVectorSetX(vVel, 0.f);
		if (fabsf(XMVectorGetZ(vVel)) < 0.01f) vVel = XMVectorSetZ(vVel, 0.f);
	}
	else
	{
		vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);
	}

	// 2. AABB 충돌을 위한 중심점(Center) 보정
	// vPos는 메쉬의 좌하단이므로, AABB의 정중앙을 구하기 위해 X, Y, Z 모두 0.5f씩 더해줍니다.
	const XMFLOAT3 halfExtents = { 0.49f, 0.49f, 0.49f };
	XMFLOAT3 c = { tntData.vPos.x + 0.5f, tntData.vPos.y + 0.5f, tntData.vPos.z + 0.5f };

	// [Y 축 충돌 검사]
	float velY = XMVectorGetY(vVel);
	float prevY = c.y;
	c.y += velY * fTimeDelta;

	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		if (velY < 0.f) // 바닥으로 떨어지다 충돌
		{
			// 바닥 블록 윗면에 정확히 안착 (기존 돼지 엔티티와 동일한 로직)
			c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
			tntData.bOnGround = true;
		}
		else // 위로 튀어오르다 천장에 충돌
		{
			c.y = prevY;
		}
		vVel = XMVectorSetY(vVel, 0.f);
	}
	else
	{
		tntData.bOnGround = false;
	}

	// [X 축 충돌 검사]
	float px = c.x;
	c.x += XMVectorGetX(vVel) * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		c.x = px;
		vVel = XMVectorSetX(vVel, 0.f);
	}

	// [Z 축 충돌 검사]
	float pz = c.z;
	c.z += XMVectorGetZ(vVel) * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		c.z = pz;
		vVel = XMVectorSetZ(vVel, 0.f);
	}

	// 3. 최종 좌표 적용 (충돌용 중심점 c에서 다시 좌하단 렌더링용 기준점으로 원상 복구)
	tntData.vPos = { c.x - 0.5f, c.y - 0.5f, c.z - 0.5f };
	XMStoreFloat3(&tntData.vVelocity, vVel);
}

UPtr<CActivatedTNT> CActivatedTNT::Create()
{
	auto pInstance = ToUPtr(new CActivatedTNT{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CActivatedTNT");
		return nullptr;
	}

	return pInstance;
}
UPtr<CPrototype> CActivatedTNT::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CActivatedTNT{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CActivatedTNT");
		return nullptr;
	}

	return pInstance;
}
