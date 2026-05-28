#include "FallingVoxel.h"
#include "GameInstance.h"
#include "Resources.h"
//#include "Block3.h"
#include "VoxelManager3.h"
NS_USING(Engine)
CFallingVoxel::CFallingVoxel()
{
}

CFallingVoxel::CFallingVoxel(const CFallingVoxel& rhs)
	: CVoxelObject(rhs)
{
}

CFallingVoxel::~CFallingVoxel()
{
}

HRESULT CFallingVoxel::Initialize(void* pArg)
{
	auto pDesc = static_cast<DESC*>(pArg);

	//m_viBufferID = pDesc->viBufferId;

    if (auto res = CResFallingVoxelVIBuffer::Create())
    {
        uint32_t tmp[ETOUI(FACE_DIR::END)]{ PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)),PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) ,PackTexId(9, ETOUI(CBlock3::TEX_TYPE::SAND)) };

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

void CFallingVoxel::PriorityUpdate(E::_float fTimeDelta)
{
}

void CFallingVoxel::Update(E::_float fTimeDelta)
{
	// ----------------------------------------------------
	// 루프 2: [추가] 정착 대기 중인 캐시들 감시 및 정리
	// ----------------------------------------------------
	//auto settledIt = m_vecSettledBlocks.begin();
	//while (settledIt != m_vecSettledBlocks.end())
	//{
	//	if (settledIt->bDelete)
	//	{
	//		// 청크 메쉬가 완벽하게 배치를 인지했으므로 이제 캐시에서 완전히 제거합니다.
	//		settledIt = m_vecSettledBlocks.erase(settledIt);
	//		continue;

	//	}
	//	CBlock3 currentChunkBlock{};
	//	// 해당 좌표의 월드 청크 블록 상태를 가져옵니다
	//	// settledIt->iTargetPos.x, settledIt->iTargetPos.y
	//	//CGameInstance::Get().GetVoxelChunk();

	//	auto chunkCoord = CVoxelManager3::GetChunkCoordByWorldBlockCoord(settledIt->iTargetPos.x, settledIt->iTargetPos.y, settledIt->iTargetPos.z);
	//	auto chunkIdx = CVoxelManager3::encodeChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z);
	//	//auto chunkCoord = CVoxelManager3::GetChunkCoordByWorldBlockCoord(settledIt->iTargetPos.x, settledIt->iTargetPos.y, settledIt->iTargetPos.z);

	//	const auto& editShadow = CGameInstance::Get().GetVoxelEditShadow();
	//	auto chunkIter = editShadow.find(chunkIdx);
	//	if (chunkIter != editShadow.end())
	//	{
	//		auto cbx = settledIt->iTargetPos.x - chunkCoord.x * (int32_t)VOXEL_CHUNK_X_SIZE3;
	//		auto cby = settledIt->iTargetPos.y - chunkCoord.y * (int32_t)VOXEL_CHUNK_Y_SIZE3;
	//		auto cbz = settledIt->iTargetPos.z - chunkCoord.z * (int32_t)VOXEL_CHUNK_Z_SIZE3;


	//		auto editBlockIter = chunkIter->second.find(CChunk3::BlockIndexing(cbx, cby, cbz));
	//		if (editBlockIter == chunkIter->second.end())
	//		{
	//			if (auto pChunk = CGameInstance::Get().GetVoxelChunkByWorldBlockCoord(settledIt->iTargetPos.x, settledIt->iTargetPos.y, settledIt->iTargetPos.z))
	//			{

	//				if (!pChunk->GetMessingQueued() && pChunk->GetMessingState() != CChunk3::MESSING_STATE::ING && pChunk->GetBufferState() != CChunk3::BUFFER_STATE::ING)
	//				{

	//					if (auto currentChunkBlock = CGameInstance::Get().GetVoxelBlock(settledIt->iTargetPos.x, settledIt->iTargetPos.y, settledIt->iTargetPos.z))
	//					{
	//						// 💡 실제 청크에 내가 보낸 블록(SAND)이 렌더링 가능한 상태로 완전히 들어찼는지 체크
	//						// 청크 메쉬 리빌드가 완료되어 이제 눈에 보이기 시작하면 AIR가 아닌 내가 넣은 타입이 감지됩니다.
	//						if (currentChunkBlock.value().GetType() == CBlock3::TYPE::SAND)
	//						{
	//							// 청크 메쉬가 완벽하게 배치를 인지했으므로 이제 캐시에서 완전히 제거합니다.
	//							//settledIt = m_vecSettledBlocks.erase(settledIt);
	//							//continue;

	//							settledIt->bDelete = true;
	//						}
	//					}

	//				}
	//			}
	//		}
	//	}





	//	++settledIt;
	//}

	const XMFLOAT3 halfExtents = { 0.49f, 0.49f, 0.49f };

	// ----------------------------------------------------
	// 루프 1: 낙하 중인 블록들 업데이트 및 충돌 검사
	// ----------------------------------------------------
	auto it = m_vecFallingBlocks.begin();
	while (it != m_vecFallingBlocks.end())
	{
		XMFLOAT3 c = it->vPos;
		XMFLOAT3 centerForOverlap = { c.x + 0.5f, c.y + 0.5f, c.z + 0.5f };

		float velY = it->vVelocity.y;
		centerForOverlap.y += velY * fTimeDelta;

		if (CGameInstance::Get().VoxelAABBOverlap(centerForOverlap, halfExtents))
		{
			if (velY <= 0.f)
			{
				int32_t iX = (int32_t)floorf(c.x);
				int32_t iY = (int32_t)floorf(centerForOverlap.y - 0.5f);
				int32_t iZ = (int32_t)floorf(c.z);

				int32_t targetY = iY + 1;

				// 월드 청크 데이터 공간에 블록 배치 명령 (청크 리빌드 예약 켜짐)
				CBlock3 b{};
				b.SetType(CBlock3::TYPE::SAND);
				CGameInstance::Get().SetVoxelBlock(iX, targetY, iZ, b);

				// 💡 [수정] 바로 소멸시키지 않고 "정착 대기 캐시 리스트"로 토스!
				TSettledBlockData settledBlock{};
				settledBlock.iTargetPos = { iX, targetY, iZ };
				// 소수점 밀림 없는 완벽한 정수 그리드 좌표에 이쁘게 고정하여 그리도록 세팅
				settledBlock.vRenderPos = { (float)iX, (float)targetY, (float)iZ };

				if (auto pChunk = CGameInstance::Get().GetVoxelChunkByWorldBlockCoord(settledBlock.iTargetPos.x, settledBlock.iTargetPos.y, settledBlock.iTargetPos.z))
				{
					settledBlock.chunkBufferVersion = pChunk->GetBufferVersion();
				}
				

				m_vecSettledBlocks.push_back(settledBlock);

				// 낙하 리스트에서는 제외
				it = m_vecFallingBlocks.erase(it);
				continue;
			}
		}
		else
		{
			it->vPos.y += velY * fTimeDelta;
		}

		++it;
	}

	// ----------------------------------------------------
	// 루프 2: [추가] 정착 대기 중인 캐시들 감시 및 정리
	// ----------------------------------------------------
	auto settledIt = m_vecSettledBlocks.begin();
	while (settledIt != m_vecSettledBlocks.end())
	{
		// 해당 좌표의 월드 청크 블록 상태를 가져옵니다.
		if (auto currentChunkBlock = CGameInstance::Get().GetVoxelBlock(settledIt->iTargetPos.x, settledIt->iTargetPos.y, settledIt->iTargetPos.z))
		{
			
			if (auto pChunk = CGameInstance::Get().GetVoxelChunkByWorldBlockCoord(settledIt->iTargetPos.x, settledIt->iTargetPos.y, settledIt->iTargetPos.z))
			{
				;
				//  실제 청크에 내가 보낸 블록(SAND)이 렌더링 가능한 상태로 완전히 들어찼는지 체크
				// 청크 메쉬 리빌드가 완료되어 이제 눈에 보이기 시작하면 AIR가 아닌 내가 넣은 타입이 감지됩니다.
				if (currentChunkBlock.value().GetType() == CBlock3::TYPE::SAND)
				{
					if (pChunk->GetBufferVersion() > settledIt->chunkBufferVersion)
					{
						// 청크 메쉬가 완벽하게 배치를 인지했으므로 이제 캐시에서 완전히 제거합니다.
						settledIt = m_vecSettledBlocks.erase(settledIt);
						continue;
					}
					;
					
				}
			}
			else
			{
				settledIt = m_vecSettledBlocks.erase(settledIt);
				continue;
			}

			
		}

		++settledIt;
	}
}

void CFallingVoxel::LateUpdate(E::_float fTimeDelta)
{
	E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	GetTransform().Update();

	m_vecInstancedBlockTransform.clear();

	// 1. 낙하 중인 블록들의 실시간 이동 행렬 수집
	for (auto& block : m_vecFallingBlocks)
	{
		_float4x4 tmp{};
		XMStoreFloat4x4(&tmp, XMMatrixTranslationFromVector(XMLoadFloat3(&block.vPos)));
		m_vecInstancedBlockTransform.push_back({ tmp });
	}

	// 2. 💡 [추가] 정착해서 대기 중인 고정 블록들의 정수 위치 행렬 추가 수집
	for (auto& settledBlock : m_vecSettledBlocks)
	{
		_float4x4 tmp{};
		XMStoreFloat4x4(&tmp, XMMatrixTranslationFromVector(XMLoadFloat3(&settledBlock.vRenderPos)));
		m_vecInstancedBlockTransform.push_back({ tmp });
	}
}

HRESULT CFallingVoxel::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_FallingVoxel");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_FallingVoxel");
	//const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>("MC_ITEM_VIBuffer", "CubeItemDirt");
	const auto& viBuffer = m_pResVIBuffer;

	pContext->IASetInputLayout(vs->GetInputLayout().Get());
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

	;


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
	
	pContext->DrawIndexedInstanced(viBuffer->GetNumIndices(), m_vecInstancedBlockTransform.size(), 0, 0, 0);
    return S_OK;
}

UPtr<CFallingVoxel> CFallingVoxel::Create()
{
    auto pInstance = ToUPtr(new CFallingVoxel{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CFallingVoxel");
        return nullptr;
    }

    return pInstance;
}
UPtr<CPrototype> CFallingVoxel::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CFallingVoxel{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CFallingVoxel");
        return nullptr;
    }

    return pInstance;
}
