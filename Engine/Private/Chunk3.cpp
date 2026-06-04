#include "pch.h"
#include "Chunk3.h"

#include "CollBox.h"
#include "GameInstance.h"

NS_USING(Engine)

void CChunk3::SpawnTree(int32_t baseI, int32_t baseK, int32_t baseJ, CBlock3::TYPE eLogType, CBlock3::TYPE eLeafType)
{
	static thread_local std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<int32_t> heightDist(4, 5);
	int32_t treeHeight = heightDist(gen);

	// 나뭇잎 범위를 기둥 윗부분에 조화롭게 층별로 세팅
	int32_t leafStartK = baseK + (treeHeight - 3);
	int32_t leafEndK = baseK + treeHeight + 1;

	// 1. 나뭇잎 먼저 전개
	for (int32_t lK = leafStartK; lK <= leafEndK; ++lK)
	{
		if (lK >= VOXEL_CHUNK_Y_SIZE3) continue;

		// 최고 꼭대기 층과 최하단 층은 반지름 1로 좁혀서 동글동글하게 생성
		int32_t radius = 2;
		if (lK == leafEndK || lK == leafStartK)
			radius = 1;

		for (int32_t lI = baseI - radius; lI <= baseI + radius; ++lI)
		{
			for (int32_t lJ = baseJ - radius; lJ <= baseJ + radius; ++lJ)
			{
				if (lI < 0 || lI >= VOXEL_CHUNK_X_SIZE3 || lJ < 0 || lJ >= VOXEL_CHUNK_Z_SIZE3)
					continue;

				// 마인크래프트 특유의 구형 잎사귀 구현을 위한 모서리 깎기
				if (radius == 2 && std::abs(lI - baseI) == 2 && std::abs(lJ - baseJ) == 2)
				{
					std::uniform_int_distribution<int> randCut(0, 4);
					if (randCut(gen) == 0) continue; // 자연스러운 불규칙성
				}

				uint32_t leafIdx = BlockIndexing(lI, lK, lJ);

				// 이제 위에서 지형 초기화 시 AIR를 완벽히 밀어두었으므로 정상 작동합니다.
				if (m_arrBlocks[leafIdx].GetType() == CBlock3::TYPE::AIR)
				{
					m_arrBlocks[leafIdx].SetType(eLeafType);
				}
			}
		}
	}

	// 2. 나무 기둥 심기 (나뭇잎 중심부를 단단하게 관통)
	for (int32_t r = 0; r < treeHeight; ++r)
	{
		int32_t currentK = baseK + r;
		if (currentK >= VOXEL_CHUNK_Y_SIZE3) break;

		m_arrBlocks[BlockIndexing(baseI, currentK, baseJ)].SetType(eLogType);
	}
}
HRESULT CChunk3::BlockFilling()
{
	m_eBlockFillingState = BLOCKFILLING_STATE::ING;

	constexpr float NOISE_OFFSET = 100000.f;
	constexpr int   BEDROCK_MAX_HEIGHT = 4;
	constexpr int32_t SEA_LEVEL = 63;

	// 나무 스폰 최적화를 위해 나무 위치 정보를 담아둘 구조체
	struct TREE_SPAWN_INFO {
		int32_t i, k, j;
		CBlock3::TYPE logType;
		CBlock3::TYPE leafType;
	};
	std::vector<TREE_SPAWN_INFO> vecTreesToSpawn;

	for (int32_t i = 0; i < VOXEL_CHUNK_X_SIZE3; ++i)
	{
		for (int32_t j = 0; j < VOXEL_CHUNK_Z_SIZE3; ++j)
		{
			float tmpx = m_iX * (int32_t)VOXEL_CHUNK_X_SIZE3 + i + NOISE_OFFSET;
			float tmpz = m_iZ * (int32_t)VOXEL_CHUNK_Z_SIZE3 + j + NOISE_OFFSET;

			// -------------------------------------------------------------
			// [STEP 1] 기후 정보 수집 및 바이옴 결정 (-1.0f ~ 1.0f)
			// -------------------------------------------------------------
			float tempNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::TEMPERATURE).GetNoise(tmpx, tmpz);
			float humidNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::HUMIDITY).GetNoise(tmpx, tmpz);
			float heightNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::HEIGHT).GetNoise(tmpx, tmpz);

			BIOME_TYPE biome = BIOME_TYPE::PLAINS;
			int32_t calculatedHeight = 64;

			if (tempNoise < -0.2f)
			{
				biome = BIOME_TYPE::SNOW;
				calculatedHeight = (int32_t)(70.0f + heightNoise * 45.0f);
			}
			else if (tempNoise > 0.2f && humidNoise < -0.1f)
			{
				biome = BIOME_TYPE::DESERT;
				calculatedHeight = (int32_t)(68.0f + heightNoise * 15.0f);
			}
			else
			{
				biome = BIOME_TYPE::PLAINS;
				calculatedHeight = (int32_t)(66.0f + heightNoise * 25.0f);
			}

			if (calculatedHeight < 5)   calculatedHeight = 5;
			if (calculatedHeight > 250) calculatedHeight = 250;

			uint32_t height = (uint32_t)calculatedHeight;
			bool bShouldSpawnTree = false;

			// -------------------------------------------------------------
			// [STEP 2] 순수 기본 지형 채우기 (나무/풀 예외처리 필요 없음)
			// -------------------------------------------------------------
			for (uint32_t k = 0; k < VOXEL_CHUNK_Y_SIZE3; ++k)
			{
				uint32_t idx = BlockIndexing(i, k, j);

				// 베드락 레이어 공통
				if (k == 0)
				{
					m_arrBlocks[idx].SetType(CBlock3::TYPE::BEDROCK);
					continue;
				}
				else if (k <= BEDROCK_MAX_HEIGHT)
				{
					float threshold = 1.f - ((float)k / BEDROCK_MAX_HEIGHT);
					float noise = (CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::BEDROCK).GetNoise(tmpx, (float)k, tmpz) + 1.f) * 0.5f;
					m_arrBlocks[idx].SetType(noise < threshold ? CBlock3::TYPE::BEDROCK : CBlock3::TYPE::STONE);
					continue;
				}

				// 지하 깊은 곳
				if (k < height - 5)
				{
					m_arrBlocks[idx].SetType(CBlock3::TYPE::STONE);
				}
				// 지표면 아래 껍질
				else if (k < height)
				{
					m_arrBlocks[idx].SetType(biome == BIOME_TYPE::DESERT ? CBlock3::TYPE::SAND : CBlock3::TYPE::DIRT);
				}
				// 가장 윗 표면 블록 처리
				else if (k == height)
				{
					if (k < SEA_LEVEL)
					{
						m_arrBlocks[idx].SetType(biome == BIOME_TYPE::DESERT ? CBlock3::TYPE::SAND : CBlock3::TYPE::DIRT);
					}
					else
					{
						switch (biome)
						{
						case BIOME_TYPE::SNOW:
							m_arrBlocks[idx].SetType(CBlock3::TYPE::STONE);
							break;
						case BIOME_TYPE::DESERT:
							m_arrBlocks[idx].SetType(CBlock3::TYPE::SAND);
							break;
						case BIOME_TYPE::PLAINS:
						default:
							m_arrBlocks[idx].SetType(CBlock3::TYPE::GRASS);

							// 🌲 나무 생성 조건 판단 (안전 지대 가드)
							if (i >= 2 && i < VOXEL_CHUNK_X_SIZE3 - 2 && j >= 2 && j < VOXEL_CHUNK_Z_SIZE3 - 2)
							{
								float treeNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::TREE_DENSITY).GetNoise(tmpx, tmpz);
								if (treeNoise > 0.25f)
								{
									bShouldSpawnTree = true;

									// 💡 [수정] 임계값이 바뀌었으므로, 랜덤 분기 스케일 변환 수식도 매칭해줍니다.
									// (treeNoise - 최소값) / (최대값 - 최소값) -> 여기서는 최대값을 1.0f로 가정
									float treeTypeRand = (treeNoise - 0.25f) / (1.0f - 0.25f);

									// 안전장치: 혹시나 1.0f을 아주 살짝 넘어가는 경우를 대비해 클램핑
									if (treeTypeRand > 0.99f) treeTypeRand = 0.99f;

									TREE_SPAWN_INFO treeInfo{ i, (int32_t)k + 1, j };

									if (treeTypeRand < 0.25f) { treeInfo.logType = CBlock3::TYPE::LOG_OAK; treeInfo.leafType = CBlock3::TYPE::LEAVES_OAK; }
									else if (treeTypeRand < 0.50f) { treeInfo.logType = CBlock3::TYPE::LOG_BIRCH; treeInfo.leafType = CBlock3::TYPE::LEAVES_BIRCH; }
									else if (treeTypeRand < 0.75f) { treeInfo.logType = CBlock3::TYPE::LOG_ACACIA; treeInfo.leafType = CBlock3::TYPE::LEAVES_ACACIA; }
									else { treeInfo.logType = CBlock3::TYPE::LOG_CHERRY; treeInfo.leafType = CBlock3::TYPE::LEAVES_CHERRY; }

									vecTreesToSpawn.push_back(treeInfo);
								}
							}
							break;
						}
					}
				}
				else
				{
					// 💡 위쪽 공기/물 공간을 완벽히 밀어두어야 SpawnTree에서 AIR 검사가 제대로 먹힙니다.
					m_arrBlocks[idx].SetType(k <= SEA_LEVEL ? CBlock3::TYPE::WATER_STILL : CBlock3::TYPE::AIR);
					if (m_arrBlocks[idx].GetType() == CBlock3::TYPE::WATER_STILL)
					{
						m_arrBlocks[idx].SetFlag(7);
					}
				}

				// -------------------------------------------------------------
				// [STEP 3] 후처리 (동굴 및 광물)
				// -------------------------------------------------------------
				CBlock3::TYPE currentType = m_arrBlocks[idx].GetType();
				if (k > BEDROCK_MAX_HEIGHT && k < height - 4)
				{
					if (m_arrBlocks[idx].GetType() == CBlock3::TYPE::WATER_STILL)
						continue;

					float caveNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::CAVE).GetNoise(tmpx, (float)k, tmpz);
					if (caveNoise > 0.45f)
					{
						m_arrBlocks[idx].SetType(CBlock3::TYPE::AIR);
						continue;
					}

					if (currentType == CBlock3::TYPE::STONE)
					{
						if (k <= 16)
						{
							float oreDiamond = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::ORE_DIAMOND).GetNoise(tmpx, (float)k, tmpz);
							if (oreDiamond > 0.85f) { m_arrBlocks[idx].SetType(CBlock3::TYPE::STONE_DIAMOND_ORE); continue; }
						}
						if (k <= 60)
						{
							float oreIronNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::ORE_IRON).GetNoise(tmpx, (float)k, tmpz);
							if (oreIronNoise > 0.80f) { m_arrBlocks[idx].SetType(CBlock3::TYPE::STONE_IRON_ORE); continue; }
						}
						if (k <= 150)
						{
							float oreCoalNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::ORE_COAL).GetNoise(tmpx, (float)k, tmpz);
							if (oreCoalNoise > 0.75f) { m_arrBlocks[idx].SetType(CBlock3::TYPE::STONE_COAL_ORE); continue; }
						}
					}
				}
			} // k loop

			// 🌿 나무가 스폰되지 않은 평지(잔디) 자리에만 식물 장식 배치
			if (!bShouldSpawnTree && biome == BIOME_TYPE::PLAINS && height >= SEA_LEVEL)
			{
				if (height + 1 < VOXEL_CHUNK_Y_SIZE3)
				{
					uint32_t aboveIdx = BlockIndexing(i, height + 1, j);
					float grassNoise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::PLANT_DECO_FIJI_SHORT_GRASS).GetNoise(tmpx, tmpz);

					if (grassNoise > 0.52f)
						m_arrBlocks[aboveIdx].SetType(CBlock3::GetRandomTypeFlower());
					else if (grassNoise > 0.28f)
						m_arrBlocks[aboveIdx].SetType(CBlock3::TYPE::FIJI_SHORT_GRASS);
				}
			}

		} // j loop
	} // i loop

	// -------------------------------------------------------------
	// [STEP 4] 💡 지형 배치가 완전히 끝난 뒤 최종적으로 나무 심기
	// -------------------------------------------------------------
	for (const auto& tree : vecTreesToSpawn)
	{
		SpawnTree(tree.i, tree.k, tree.j, tree.logType, tree.leafType);
	}

	m_eBlockFillingState = BLOCKFILLING_STATE::DONE;
	return S_OK;
}

HRESULT CChunk3::QuadMessing()
{
	m_eMessingState = MESSING_STATE::ING;

	QuadBuckets buckets{};
	NiveFaceCulling(buckets);

	//std::vector<VOX_QUAD> solidQuads{};
	//std::vector<VOX_QUAD> alphaTestQuads{};
	//std::vector<VOX_QUAD> waterQuads{};
	//NiveFaceCulling(solidQuads, alphaTestQuads, waterQuads);

	QuadsToVerticies(buckets.solid, m_SolidVertices, m_SolidIndices);
	QuadsToVerticies(buckets.alphaTest, m_AlphaTestVertices, m_AlphaTestIndices);
	QuadsToVerticies(buckets.water, m_WaterVertices, m_WaterIndices);

	m_eMessingState = MESSING_STATE::DONE;
	return S_OK;
}

HRESULT CChunk3::CreateBuffer()
{
	m_eBufferState = BUFFER_STATE::ING;

	// solid buffer
	{
		if (!m_SolidVertices.empty())
		{
			if (FAILED(CreateBuffer(m_pResSolidDynamicViBuffer, m_SolidVertices, m_SolidIndices)))
			{
				m_pResSolidDynamicViBuffer.reset();
				m_eBufferState = BUFFER_STATE::NON;
				return E_FAIL;
			}

			std::vector<E::VTX_VOXEL>().swap(m_SolidVertices);
			std::vector<uint32_t>().swap(m_SolidIndices);
		}
		else
		{
			m_pResSolidDynamicViBuffer.reset();
		}
	}

	// alphatest buffer
	{
		if (!m_AlphaTestVertices.empty())
		{
			if (FAILED(CreateBuffer(m_pResAlphaTestDynamicViBuffer, m_AlphaTestVertices, m_AlphaTestIndices)))
			{
				m_pResAlphaTestDynamicViBuffer.reset();
				m_eBufferState = BUFFER_STATE::NON;
				return E_FAIL;
			}

			std::vector<E::VTX_VOXEL>().swap(m_AlphaTestVertices);
			std::vector<uint32_t>().swap(m_AlphaTestIndices);

		}
		else
		{
			m_pResAlphaTestDynamicViBuffer.reset();
		}
	}

	// water buffer
	{
		if (!m_WaterVertices.empty())
		{
			if (FAILED(CreateBuffer(m_pResWaterDynamicViBuffer, m_WaterVertices, m_WaterIndices)))
			{
				m_pResWaterDynamicViBuffer.reset();
				m_eBufferState = BUFFER_STATE::NON;
				return E_FAIL;
			}

			std::vector<E::VTX_VOXEL>().swap(m_WaterVertices);
			std::vector<uint32_t>().swap(m_WaterIndices);

		}
		else
		{
			m_pResWaterDynamicViBuffer.reset();
		}
	}

	m_eBufferState = BUFFER_STATE::DONE;

	m_eMessingState = MESSING_STATE::NON;

	++m_iBufferVersion;
	return S_OK;
}

HRESULT CChunk3::DrawSolid(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const
{
	if (m_eBufferState != BUFFER_STATE::DONE)
	{
		return S_OK;
	}
	if (!m_pResSolidDynamicViBuffer)
	{
		return S_OK;
	}
	{
		E::CB_PER_OBJECT cbPerObject{};

		int32_t dx = m_iX * VOXEL_CHUNK_X_SIZE3;
		int32_t dz = m_iZ * VOXEL_CHUNK_Z_SIZE3;

		auto worldMat = XMMatrixTranslation((float)dx, 0, (float)dz);

		XMStoreFloat4x4(&cbPerObject.matWorld, worldMat);
		XMStoreFloat4x4(&cbPerObject.matWVP, worldMat * ctx.matView * ctx.matProj);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(pContext->Map(m_pResCBufferPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			memcpy(mappedResource.pData, &cbPerObject, sizeof(E::CB_PER_OBJECT));
			pContext->Unmap(m_pResCBufferPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, m_pResCBufferPerObject->GetCBuffer().GetAddressOf());
	}

	const auto& viBuffer = m_pResSolidDynamicViBuffer;

	ID3D11Buffer* vertexBuffers[] = {
		viBuffer->GetVertexBuffer().Get()
	};
	uint32_t strides[] = {
		viBuffer->GetVertexStride()
	};
	uint32_t offsets[] = {
		0
	};
	pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

HRESULT CChunk3::DrawAlphaTest(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const
{
	if (m_eBufferState != BUFFER_STATE::DONE)
	{
		return S_OK;
	}
	if (!m_pResAlphaTestDynamicViBuffer)
	{
		return S_OK;
	}
	{
		E::CB_PER_OBJECT cbPerObject{};

		int32_t dx = m_iX * VOXEL_CHUNK_X_SIZE3;
		int32_t dz = m_iZ * VOXEL_CHUNK_Z_SIZE3;

		auto worldMat = XMMatrixTranslation((float)dx, 0, (float)dz);

		XMStoreFloat4x4(&cbPerObject.matWorld, worldMat);
		XMStoreFloat4x4(&cbPerObject.matWVP, worldMat * ctx.matView * ctx.matProj);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(pContext->Map(m_pResCBufferPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			memcpy(mappedResource.pData, &cbPerObject, sizeof(E::CB_PER_OBJECT));
			pContext->Unmap(m_pResCBufferPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, m_pResCBufferPerObject->GetCBuffer().GetAddressOf());
	}
	const auto& viBuffer = m_pResAlphaTestDynamicViBuffer;

	ID3D11Buffer* vertexBuffers[] = {
		viBuffer->GetVertexBuffer().Get()
	};
	uint32_t strides[] = {
		viBuffer->GetVertexStride()
	};
	uint32_t offsets[] = {
		0
	};
	pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

HRESULT CChunk3::DrawWater(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const
{
	if (m_eBufferState != BUFFER_STATE::DONE)
	{
		return S_OK;
	}
	if (!m_pResWaterDynamicViBuffer)
	{
		return S_OK;
	}
	{
		E::CB_PER_OBJECT cbPerObject{};

		int32_t dx = m_iX * VOXEL_CHUNK_X_SIZE3;
		int32_t dz = m_iZ * VOXEL_CHUNK_Z_SIZE3;

		auto worldMat = XMMatrixTranslation((float)dx, 0, (float)dz);

		XMStoreFloat4x4(&cbPerObject.matWorld, worldMat);
		XMStoreFloat4x4(&cbPerObject.matWVP, worldMat * ctx.matView * ctx.matProj);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(pContext->Map(m_pResCBufferPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			memcpy(mappedResource.pData, &cbPerObject, sizeof(E::CB_PER_OBJECT));
			pContext->Unmap(m_pResCBufferPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, m_pResCBufferPerObject->GetCBuffer().GetAddressOf());
	}

	const auto& viBuffer = m_pResWaterDynamicViBuffer;

	ID3D11Buffer* vertexBuffers[] = {
		viBuffer->GetVertexBuffer().Get()
	};
	uint32_t strides[] = {
		viBuffer->GetVertexStride()
	};
	uint32_t offsets[] = {
		0
	};
	pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

void CChunk3::Update(_float fTimeDelta)
{
	CGameInstance::Get().AddColliderGroup("Coll_Chunk", m_pCollBox.get());


}

std::optional<CBlock3> CChunk3::GetBlockAt(int32_t x, int32_t y, int32_t z, std::vector<CChunk3*>& vecAdjChunks) const
{

	if (y < 0 || y >= VOXEL_CHUNK_Y_SIZE3)
	{
		return std::nullopt;
	}
	else if (x >= 0 && x < VOXEL_CHUNK_X_SIZE3 && z >= 0 && z < VOXEL_CHUNK_Z_SIZE3)
	{
		return m_arrBlocks[BlockIndexing(x, y, z)];
	}
	else
	{
		_bool bR{ false };
		_bool bL{ false };

		_bool bF{ false };
		_bool bB{ false };

		int32_t localX = x;
		int32_t localZ = z;

		if (x >= VOXEL_CHUNK_X_SIZE3)
		{
			localX = x - VOXEL_CHUNK_X_SIZE3;
			bR = true;
		}
		else if (x < 0)
		{
			localX = x + VOXEL_CHUNK_X_SIZE3;
			bL = true;
		}

		if (z >= VOXEL_CHUNK_Z_SIZE3)
		{
			localZ = z - VOXEL_CHUNK_Z_SIZE3;
			bF = true;
		}
		else if (z < 0)
		{
			localZ = z + VOXEL_CHUNK_Z_SIZE3;
			bB = true;
		}

		CChunk3* pTargetChunk{};
		if (bR && bF)
		{
			//pos_x_pos_z
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::POS_X_POS_Z)];
		}
		else if (bR && bB)
		{
			// pos_x_neg_z
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::POS_X_NEG_Z)];
		}
		else if (bL && bF)
		{
			// neg_x_pos_z
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::NEG_X_POS_Z)];
		}
		else if (bL && bB)
		{
			// neg_x_neg_z
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::NEG_X_NEG_Z)];
		}
		else if (bR)
		{
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::POS_X)];
		}
		else if (bL)
		{
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::NEG_X)];
		}
		else if (bF)
		{
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::POS_Z)];
		}
		else if (bB)
		{
			pTargetChunk = vecAdjChunks[ETOUI(CHUNK_DIR::NEG_Z)];
		}

		if (pTargetChunk)
		{
			return pTargetChunk->m_arrBlocks[pTargetChunk->BlockIndexing(localX, y, localZ)];
		}
	}

	return std::nullopt;
}

std::vector<CChunk3*> CChunk3::MakeAdjChunks(int32_t cx, int32_t cy, int32_t cz)
{
	std::vector<CChunk3*> adjChunks{};
	adjChunks.resize(ETOUI(CHUNK_DIR::END));
	adjChunks[ETOUI(CHUNK_DIR::POS_X)] = CGameInstance::Get().GetVoxelChunk(cx + 1, cy, cz);
	adjChunks[ETOUI(CHUNK_DIR::NEG_X)] = CGameInstance::Get().GetVoxelChunk(cx - 1, cy, cz);
	adjChunks[ETOUI(CHUNK_DIR::POS_Z)] = CGameInstance::Get().GetVoxelChunk(cx, cy, cz + 1);
	adjChunks[ETOUI(CHUNK_DIR::NEG_Z)] = CGameInstance::Get().GetVoxelChunk(cx, cy, cz - 1);
	adjChunks[ETOUI(CHUNK_DIR::POS_X_POS_Z)] = CGameInstance::Get().GetVoxelChunk(cx + 1, cy, cz + 1);
	adjChunks[ETOUI(CHUNK_DIR::POS_X_NEG_Z)] = CGameInstance::Get().GetVoxelChunk(cx + 1, cy, cz - 1);
	adjChunks[ETOUI(CHUNK_DIR::NEG_X_POS_Z)] = CGameInstance::Get().GetVoxelChunk(cx - 1, cy, cz + 1);
	adjChunks[ETOUI(CHUNK_DIR::NEG_X_NEG_Z)] = CGameInstance::Get().GetVoxelChunk(cx - 1, cy, cz - 1);
	adjChunks[ETOUI(CHUNK_DIR::CENTER)] = CGameInstance::Get().GetVoxelChunk(cx, cy, cz);
	return adjChunks;
}

std::vector<CChunk3*> CChunk3::MakeAdjChunks() const
{
	return MakeAdjChunks(m_iX, m_iY, m_iZ);
}

bool CChunk3::IsFaceExposed(CBlock3 currBlock, FACE_DIR eDir, std::optional<CBlock3> optNextBlock)
{
	// 1. 인접한 블록이 아예 없다면 (청크가 생성되지 않은 월드의 끝 또는 공백) 
	//    일반적으로 면을 그려서 막아주거나 하늘을 보여야 하므로 true를 반환합니다.
	if (!optNextBlock.has_value())
	{
		return true;
	}

	CBlock3::TYPE nextType = optNextBlock.value().GetType();

	// 💡 예외 조건 1: 내가 횃불 같은 특수 오브젝트라면 내 면은 무조건 노출되어야 합니다.
	if (currBlock.GetType() == CBlock3::TYPE::TORCH_ON)
	{
		return true;
	}

	// 💡 예외 조건 2: 인접한 블록이 횃불이거나 알파 테스트(식물, 유리 등) 블록이라면 
	//               그 틈새로 내가 보여야 하므로 내 면을 무조건 노출합니다.
	if (nextType == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(nextType))
	{
		return true;
	}

	// 2. 현재 내 블록의 속성에 따른 일반적인 컬링 규칙
		
	bool isCurrentWater = CBlock3::IsWater(currBlock.GetType());
	bool isCurrentAlpha = CBlock3::IsNeedAlphaTest(currBlock.GetType());

	if (isCurrentWater || isCurrentAlpha)
	{
		if (isCurrentWater && currBlock.GetType() != nextType)
		{
			return true;
		}
		// 내 블록이 물이거나 알파(반투명)인 경우: 다음 블록이 완전히 비어있는 '공기'여야만 노출
		return (nextType == CBlock3::TYPE::AIR);
	}
	else
	{
		// 내 블록이 일반 고체(불투명)인 경우: 다음 블록이 '공기'이거나 '물'일 때 내 면이 노출
		return (nextType == CBlock3::TYPE::AIR || CBlock3::IsWater(nextType));
	}
}

void CChunk3::NiveFaceCulling(QuadBuckets& quadBuckets) const
{
	auto& solidQuads = quadBuckets.solid;
	auto& alphaTestQuads = quadBuckets.alphaTest;
	auto& waterQuads = quadBuckets.water;

	std::vector<CChunk3*> adjChunks{ MakeAdjChunks ()};

	for (int x = 0; x < (int)VOXEL_CHUNK_X_SIZE3; ++x)
	{
		for (int z = 0; z < (int)VOXEL_CHUNK_Z_SIZE3; ++z)
		{
			for (int y = 0; y < (int)VOXEL_CHUNK_Y_SIZE3; ++y)
			{
				auto currOptBlock = GetBlockAt(x, y, z, adjChunks);
				if (!currOptBlock.has_value()) continue;

				CBlock3 currBlock = currOptBlock.value();
				CBlock3::TYPE curType = currBlock.GetType();
				if (curType == CBlock3::TYPE::AIR) continue;

				
				bool isCurrentWater = CBlock3::IsWater(curType);
				bool isCurrAlphaTest = CBlock3::IsNeedAlphaTest(curType);

				float fx = (float)x;
				float fy = (float)y;
				float fz = (float)z;

				CBlock3::GEO_TYPE geoType = CBlock3::GetGeoType(curType);

				// 십자 교차형 블록 처리 (잔디, 꽃 등)
				if (geoType == CBlock3::GEO_TYPE::CROSS)
				{
					BuildCrossMesh(fx, fy, fz, curType, alphaTestQuads);
					continue; // 6면체 컬링 루틴 전체 패스
				}
				else if (geoType == CBlock3::GEO_TYPE::SLAP)
				{
					BuildSlapMesh(fx, fy, fz, curType, solidQuads);
					continue; // 6면체 컬링 루틴 전체 패스
				}
				else if (geoType == CBlock3::GEO_TYPE::STAIR)
				{
					BuildStairMesh(fx, fy, fz, curType, solidQuads);
					continue; // 6면체 컬링 루틴 전체 패스
				}
				else if (geoType == CBlock3::GEO_TYPE::TORCH)
				{
					BuildTorchMesh(fx, fy, fz, curType, solidQuads);
					continue;
				}
				// NiveFaceCulling 내부 루프 조건절 수정 파트
				else if (geoType == CBlock3::GEO_TYPE::WATER)
				{
					if (curType == CBlock3::TYPE::LAVA_STILL || curType == CBlock3::TYPE::LAVA_FLOW)
					{
						BuildLavaMesh((int)fx, (int)fy, (int)fz, solidQuads, adjChunks);
					}
					else
					{
						// 💡 기존의 BuildWaterMesh(fx, fy, fz, 6, solidQuads); 구조에서
											// 이웃 청크 정보와 올바른 반투명 쿼드 컨테이너(waterQuads)를 인자로 전달하도록 교체합니다.
						BuildWaterMesh((int)fx, (int)fy, (int)fz, waterQuads, adjChunks);
					}
					
					continue; // 6면체 기본 불투명 컬링 루틴 패스
				}






				// 1. Top (+Y)
				{
					int ny = y + 1;
					std::optional<CBlock3> optNextBlock = GetBlockAt(x, ny, z, adjChunks);
					bool bExpose = IsFaceExposed(currBlock, FACE_DIR::POS_Y, optNextBlock);

					if (bExpose) {
						VOX_QUAD quad{};
						quad.v[0] = { fx,     fy + 1, fz + 1 };
						quad.v[1] = { fx + 1, fy + 1, fz + 1 };
						quad.v[2] = { fx + 1, fy + 1, fz };
						quad.v[3] = { fx,     fy + 1, fz };


						quad.eDir = FACE_DIR::POS_Y;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
						

						auto optL = GetBlockAt(x - 1,	ny, z,		adjChunks);
						auto optR = GetBlockAt(x + 1,	ny, z,		adjChunks);
						auto optF = GetBlockAt(x,		ny, z + 1,	adjChunks);
						auto optB = GetBlockAt(x,		ny, z - 1,	adjChunks);
						auto optBL = GetBlockAt(x - 1,	ny, z - 1,	adjChunks);
						auto optBR = GetBlockAt(x + 1,	ny, z - 1,	adjChunks);
						auto optFL = GetBlockAt(x - 1,	ny, z + 1,	adjChunks);
						auto optFR = GetBlockAt(x + 1,	ny, z + 1,	adjChunks);

						auto optU = GetBlockAt(x, y + 1, z, adjChunks); // 💡 y + 1로 수정
						quad.lighting = optU.has_value() ? optU.value().GetLight() : 0xFF;

						_bool bL = optL.has_value() && CBlock3::IsAOOclluder(optL.value().GetType());
						_bool bR = optR.has_value() && CBlock3::IsAOOclluder(optR.value().GetType());
						_bool bF = optF.has_value() && CBlock3::IsAOOclluder(optF.value().GetType());
						_bool bB = optB.has_value() && CBlock3::IsAOOclluder(optB.value().GetType());
						_bool bBL = optBL.has_value() && CBlock3::IsAOOclluder(optBL.value().GetType());
						_bool bBR = optBR.has_value() && CBlock3::IsAOOclluder(optBR.value().GetType());
						_bool bFL = optFL.has_value() && CBlock3::IsAOOclluder(optFL.value().GetType());
						_bool bFR = optFR.has_value() && CBlock3::IsAOOclluder(optFR.value().GetType());


						quad.ao[0] = CalculateVertexAO(bL, bF, bFL);
						quad.ao[1] = CalculateVertexAO(bR, bF, bFR);
						quad.ao[2] = CalculateVertexAO(bR, bB, bBR);
						quad.ao[3] = CalculateVertexAO(bL, bB, bBL);

						quad.color[0] = CBlock3::GetBaseColor(curType);
						quad.color[1] = CBlock3::GetBaseColor(curType);
						quad.color[2] = CBlock3::GetBaseColor(curType);
						quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 2. Bottom (-Y)
				{
					int ny = y - 1;
					
					std::optional<CBlock3> optNextBlock = GetBlockAt(x, ny, z, adjChunks);
					bool bExpose = IsFaceExposed(currBlock, FACE_DIR::NEG_Y, optNextBlock);

					if (bExpose) {
						VOX_QUAD quad{};
						quad.v[0] = { fx,     fy, fz };
						quad.v[1] = { fx + 1, fy, fz };
						quad.v[2] = { fx + 1, fy, fz + 1 };
						quad.v[3] = { fx,     fy, fz + 1 };

						quad.eDir = FACE_DIR::NEG_Y;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
						//quad.lighting = (y - 1 >= 0) ? m_arrBlocks[BlockIndexing(x, y - 1, z)].GetLight() : 0;

						auto optL = GetBlockAt(x - 1, ny, z, adjChunks);
						auto optR = GetBlockAt(x + 1, ny, z, adjChunks);
						auto optF = GetBlockAt(x, ny, z + 1, adjChunks);
						auto optB = GetBlockAt(x, ny, z - 1, adjChunks);
						auto optBL = GetBlockAt(x - 1, ny, z - 1, adjChunks);
						auto optBR = GetBlockAt(x + 1, ny, z - 1, adjChunks);
						auto optFL = GetBlockAt(x - 1, ny, z + 1, adjChunks);
						auto optFR = GetBlockAt(x + 1, ny, z + 1, adjChunks);

						_bool bL = optL.has_value() && CBlock3::IsAOOclluder(optL.value().GetType());
						_bool bR = optR.has_value() && CBlock3::IsAOOclluder(optR.value().GetType());
						_bool bF = optF.has_value() && CBlock3::IsAOOclluder(optF.value().GetType());
						_bool bB = optB.has_value() && CBlock3::IsAOOclluder(optB.value().GetType());
						_bool bBL = optBL.has_value() && CBlock3::IsAOOclluder(optBL.value().GetType());
						_bool bBR = optBR.has_value() && CBlock3::IsAOOclluder(optBR.value().GetType());
						_bool bFL = optFL.has_value() && CBlock3::IsAOOclluder(optFL.value().GetType());
						_bool bFR = optFR.has_value() && CBlock3::IsAOOclluder(optFR.value().GetType());

						auto optD = GetBlockAt(x, y - 1, z, adjChunks); // 💡 y - 1로 수정
						quad.lighting = optD.has_value() ? optD.value().GetLight() : 0xFF; // 💡 기본값 0xFF로 통일 (엔진 의도에 따라 0도 가능)


						quad.ao[0] = CalculateVertexAO(bL, bB, bBL);
						quad.ao[1] = CalculateVertexAO(bR, bB, bBR);
						quad.ao[2] = CalculateVertexAO(bR, bF, bFR);
						quad.ao[3] = CalculateVertexAO(bL, bF, bFL);


						quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = CBlock3::GetBaseColor(curType);
				
						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 3. Front (+Z)
				{
					int nz = z + 1;
					std::optional<CBlock3> optNextBlock = GetBlockAt(x, y, nz, adjChunks);

					bool bExpose = IsFaceExposed(currBlock, FACE_DIR::POS_Z, optNextBlock);


					if (bExpose) {
						VOX_QUAD quad{};
						quad.v[0] = { fx + 1, fy + 1, fz + 1 };
						quad.v[1] = { fx,     fy + 1, fz + 1 };
						quad.v[2] = { fx,     fy,     fz + 1 };
						quad.v[3] = { fx + 1, fy,     fz + 1 };

						quad.eDir = FACE_DIR::POS_Z;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));

						// Front(+Z) 방향의 인접 블록 위치는 (x, y, z + 1) 입니다.
						auto optF = GetBlockAt(x, y, z + 1, adjChunks);

						// 💡 블록이 존재하면 그 블록의 Light 데이터를 가져오고, 
						// 청크가 로드되지 않는 등 완전히 비어있는 경계 밖이라면 기본값(0xFF 또는 0)을 줍니다.
						quad.lighting = optF.has_value() ? optF.value().GetLight() : 0xFF;
						//quad.lighting = (z + 1 < (int)VOXEL_CHUNK_Z_SIZE3) ? m_arrBlocks[BlockIndexing(x, y, z + 1)].GetLight() : (pPlusZ ? pPlusZ->m_arrBlocks[pPlusZ->BlockIndexing(x, y, 0)].GetLight() : 0xFF);

						auto optL = GetBlockAt(x - 1, y, nz, adjChunks);
						auto optR = GetBlockAt(x + 1, y, nz, adjChunks);
						auto optU = GetBlockAt(x, y + 1, nz, adjChunks);
						auto optD = GetBlockAt(x, y - 1, nz, adjChunks);
						auto optUL = GetBlockAt(x - 1, y + 1, nz, adjChunks);
						auto optUR = GetBlockAt(x + 1, y + 1, nz, adjChunks);
						auto optDL = GetBlockAt(x - 1, y - 1, nz, adjChunks);
						auto optDR = GetBlockAt(x + 1, y - 1, nz, adjChunks);

						_bool bL = optL.has_value() && CBlock3::IsAOOclluder(optL.value().GetType());
						_bool bR = optR.has_value() && CBlock3::IsAOOclluder(optR.value().GetType());
						_bool bU = optU.has_value() && CBlock3::IsAOOclluder(optU.value().GetType());
						_bool bD = optD.has_value() && CBlock3::IsAOOclluder(optD.value().GetType());
						_bool bUL = optUL.has_value() && CBlock3::IsAOOclluder(optUL.value().GetType());
						_bool bUR = optUR.has_value() && CBlock3::IsAOOclluder(optUR.value().GetType());
						_bool bDL = optDL.has_value() && CBlock3::IsAOOclluder(optDL.value().GetType());
						_bool bDR = optDR.has_value() && CBlock3::IsAOOclluder(optDR.value().GetType());

						quad.ao[0] = CalculateVertexAO(bR, bU, bUR);
						quad.ao[1] = CalculateVertexAO(bL, bU, bUL);
						quad.ao[2] = CalculateVertexAO(bL, bD, bDL);
						quad.ao[3] = CalculateVertexAO(bR, bD, bDR);


						quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 4. Back (-Z)
				{
					int nz = z - 1;
					//CBlock3::TYPE nextType;
					std::optional<CBlock3> optNextBlock = GetBlockAt(x, y, nz, adjChunks);
					
					bool bExpose = IsFaceExposed(currBlock, FACE_DIR::NEG_Z, optNextBlock);


					if (bExpose) {
						VOX_QUAD quad{};
						quad.v[0] = { fx,     fy + 1, fz };
						quad.v[1] = { fx + 1, fy + 1, fz };
						quad.v[2] = { fx + 1, fy,     fz };
						quad.v[3] = { fx,     fy,     fz };

						quad.eDir = FACE_DIR::NEG_Z;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
						//quad.lighting = (z - 1 >= 0) ? m_arrBlocks[BlockIndexing(x, y, z - 1)].GetLight() : (pMinusZ ? pMinusZ->m_arrBlocks[pMinusZ->BlockIndexing(x, y, (int)VOXEL_CHUNK_Z_SIZE3 - 1)].GetLight() : 0xFF);

						// Back(-Z) 방향의 인접 블록 위치는 (x, y, z - 1) 입니다.
						auto optB = GetBlockAt(x, y, z - 1, adjChunks);

						// 💡 블록이 존재하면 그 블록의 Light 데이터를 가져오고, 없으면 기본값(0xFF) 처리
						quad.lighting = optB.has_value() ? optB.value().GetLight() : 0xFF;

						auto optL = GetBlockAt(x - 1, y, nz, adjChunks);
						auto optR = GetBlockAt(x + 1, y, nz, adjChunks);
						auto optU = GetBlockAt(x, y + 1, nz, adjChunks);
						auto optD = GetBlockAt(x, y - 1, nz, adjChunks);
						auto optUL = GetBlockAt(x - 1, y + 1, nz, adjChunks);
						auto optUR = GetBlockAt(x + 1, y + 1, nz, adjChunks);
						auto optDL = GetBlockAt(x - 1, y - 1, nz, adjChunks);
						auto optDR = GetBlockAt(x + 1, y - 1, nz, adjChunks);

						_bool bL = optL.has_value() && CBlock3::IsAOOclluder(optL.value().GetType());
						_bool bR = optR.has_value() && CBlock3::IsAOOclluder(optR.value().GetType());
						_bool bU = optU.has_value() && CBlock3::IsAOOclluder(optU.value().GetType());
						_bool bD = optD.has_value() && CBlock3::IsAOOclluder(optD.value().GetType());
						_bool bUL = optUL.has_value() && CBlock3::IsAOOclluder(optUL.value().GetType());
						_bool bUR = optUR.has_value() && CBlock3::IsAOOclluder(optUR.value().GetType());
						_bool bDL = optDL.has_value() && CBlock3::IsAOOclluder(optDL.value().GetType());
						_bool bDR = optDR.has_value() && CBlock3::IsAOOclluder(optDR.value().GetType());


						quad.ao[0] = CalculateVertexAO(bL, bU, bUL);
						quad.ao[1] = CalculateVertexAO(bR, bU, bUR);
						quad.ao[2] = CalculateVertexAO(bR, bD, bDR);
						quad.ao[3] = CalculateVertexAO(bL, bD, bDL);


						quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 5. Right (+X)
				{
					int nx = x + 1;

					//CBlock3::TYPE nextType;
					std::optional<CBlock3> optNextBlock = GetBlockAt(nx, y, z, adjChunks);
					
					bool bExpose = IsFaceExposed(currBlock, FACE_DIR::POS_X, optNextBlock);
					
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v[0] = { fx + 1, fy + 1, fz };
						quad.v[1] = { fx + 1, fy + 1, fz + 1 };
						quad.v[2] = { fx + 1, fy,     fz + 1 };
						quad.v[3] = { fx + 1, fy,     fz };

						quad.eDir = FACE_DIR::POS_X;
						
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currBlock.GetType(), quad.eDir));
						//quad.lighting = (x + 1 < (int)VOXEL_CHUNK_X_SIZE3) ? m_arrBlocks[BlockIndexing(x + 1, y, z)].GetLight() : (pPlusX ? pPlusX->m_arrBlocks[pPlusX->BlockIndexing(0, y, z)].GetLight() : 0xFF);

						// Right(+X) 방향의 인접 블록 위치는 (x + 1, y, z) 입니다.
						auto optR = GetBlockAt(x + 1, y, z, adjChunks);

						// 💡 블록이 존재하면 그 블록의 Light 데이터를 가져고, 없으면 기본값(0xFF) 처리
						quad.lighting = optR.has_value() ? optR.value().GetLight() : 0xFF;

						auto optF = GetBlockAt(nx, y, z + 1, adjChunks);
						auto optB = GetBlockAt(nx, y, z - 1, adjChunks);
						auto optU = GetBlockAt(nx, y + 1, z, adjChunks);
						auto optD = GetBlockAt(nx, y - 1, z, adjChunks);
						auto optUF = GetBlockAt(nx, y + 1, z + 1, adjChunks);
						auto optUB = GetBlockAt(nx, y + 1, z - 1, adjChunks);
						auto optDF = GetBlockAt(nx, y - 1, z + 1, adjChunks);
						auto optDB = GetBlockAt(nx, y - 1, z - 1, adjChunks);


						_bool bF = optF.has_value() && CBlock3::IsAOOclluder(optF.value().GetType());
						_bool bB = optB.has_value() && CBlock3::IsAOOclluder(optB.value().GetType());
						_bool bU = optU.has_value() && CBlock3::IsAOOclluder(optU.value().GetType());
						_bool bD = optD.has_value() && CBlock3::IsAOOclluder(optD.value().GetType());
						_bool bUF = optUF.has_value() && CBlock3::IsAOOclluder(optUF.value().GetType());
						_bool bUB = optUB.has_value() && CBlock3::IsAOOclluder(optUB.value().GetType());
						_bool bDF = optDF.has_value() && CBlock3::IsAOOclluder(optDF.value().GetType());
						_bool bDB = optDB.has_value() && CBlock3::IsAOOclluder(optDB.value().GetType());

						quad.ao[0] = CalculateVertexAO(bB, bU, bUB);
						quad.ao[1] = CalculateVertexAO(bF, bU, bUF);
						quad.ao[2] = CalculateVertexAO(bF, bD, bDF);
						quad.ao[3] = CalculateVertexAO(bB, bD, bDB);

						quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 6. Left (-X)
				{
					int nx = x - 1;

					std::optional<CBlock3> optNextBlock = GetBlockAt(nx, y, z, adjChunks);
					

					bool bExpose = IsFaceExposed(currBlock, FACE_DIR::NEG_X, optNextBlock);
					
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v[0] = { fx, fy + 1, fz + 1 };
						quad.v[1] = { fx, fy + 1, fz };
						quad.v[2] = { fx, fy,     fz };
						quad.v[3] = { fx, fy,     fz + 1 };

						quad.eDir = FACE_DIR::NEG_X;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currBlock.GetType(), quad.eDir));
						//quad.lighting = (x - 1 >= 0) ? m_arrBlocks[BlockIndexing(x - 1, y, z)].GetLight() : (pMinusX ? pMinusX->m_arrBlocks[pMinusX->BlockIndexing((int)VOXEL_CHUNK_X_SIZE3 - 1, y, z)].GetLight() : 0xFF);

						// Left(-X) 방향의 인접 블록 위치는 (x - 1, y, z) 입니다.
						auto optL = GetBlockAt(x - 1, y, z, adjChunks);

						// 💡 블록이 존재하면 그 블록의 Light 데이터를 가져오고, 없으면 기본값(0xFF) 처리
						quad.lighting = optL.has_value() ? optL.value().GetLight() : 0xFF;

						auto optF = GetBlockAt(nx, y, z + 1, adjChunks);
						auto optB = GetBlockAt(nx, y, z - 1, adjChunks);
						auto optU = GetBlockAt(nx, y + 1, z, adjChunks);
						auto optD = GetBlockAt(nx, y - 1, z, adjChunks);
						auto optUF = GetBlockAt(nx, y + 1, z + 1, adjChunks);
						auto optUB = GetBlockAt(nx, y + 1, z - 1, adjChunks);
						auto optDF = GetBlockAt(nx, y - 1, z + 1, adjChunks);
						auto optDB = GetBlockAt(nx, y - 1, z - 1, adjChunks);


						_bool bF = optF.has_value() && CBlock3::IsAOOclluder(optF.value().GetType());
						_bool bB = optB.has_value() && CBlock3::IsAOOclluder(optB.value().GetType());
						_bool bU = optU.has_value() && CBlock3::IsAOOclluder(optU.value().GetType());
						_bool bD = optD.has_value() && CBlock3::IsAOOclluder(optD.value().GetType());
						_bool bUF = optUF.has_value() && CBlock3::IsAOOclluder(optUF.value().GetType());
						_bool bUB = optUB.has_value() && CBlock3::IsAOOclluder(optUB.value().GetType());
						_bool bDF = optDF.has_value() && CBlock3::IsAOOclluder(optDF.value().GetType());
						_bool bDB = optDB.has_value() && CBlock3::IsAOOclluder(optDB.value().GetType());

						quad.ao[0] = CalculateVertexAO(bF, bU, bUF);
						quad.ao[1] = CalculateVertexAO(bB, bU, bUB);
						quad.ao[2] = CalculateVertexAO(bB, bD, bDB);
						quad.ao[3] = CalculateVertexAO(bF, bD, bDF);

						quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}
			}
		}
	}
}

uint8_t CChunk3::CalculateVertexAO(_bool side1, _bool side2, _bool corner) const
{
	if (side1 && side2) return 0; // 양옆이 다 막히면 완전 어두움

	int32_t count = 0;
	if (side1) count++;
	if (side2) count++;
	if (corner) count++;

	return static_cast<uint8_t>(3 - count); // 많이 막힐수록 작은 값(어두움)
}

void CChunk3::BuildCrossMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& alphaTestQuads) const
{// 십자형은 자체 광원이나 차폐가 없으므로 정점 라이팅 값을 현재 블록 고유 값으로 세팅합니다.
	uint8_t blockLight = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)].GetLight();

	// 1번째 대각선 面 (\ 방향)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,     fy + 1.f, fz};
		quad.v[1] = {fx + 1.f, fy + 1.f, fz + 1.f};
		quad.v[2] = {fx + 1.f, fy,       fz + 1.f};
		quad.v[3] = {fx,     fy,       fz};

		quad.uv[0] = {0.f, 0.f}; quad.uv[1] = {1.f, 0.f};
		quad.uv[2] = {1.f, 1.f}; quad.uv[3] = {0.f, 1.f};

		quad.eDir = FACE_DIR::POS_Y; // 풀떼기는 보통 단일 텍스처를 쓰므로 POS_Y 등으로 긁어옴
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3; // 식물은 구석 AO림 제거
		

		quad.color[0] = CBlock3::GetBaseColor(curType);
		quad.color[1] = CBlock3::GetBaseColor(curType);
		quad.color[2] = CBlock3::GetBaseColor(curType);
		quad.color[3] = CBlock3::GetBaseColor(curType);

		alphaTestQuads.push_back(quad);
	}

	// 2번째 대각선 面 (/ 방향)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,     fy + 1.f, fz + 1.f};
		quad.v[1] = {fx + 1.f, fy + 1.f, fz};
		quad.v[2] = {fx + 1.f, fy,       fz};
		quad.v[3] = {fx,     fy,       fz + 1.f};

		quad.uv[0] = {0.f, 0.f}; quad.uv[1] = {1.f, 0.f};
		quad.uv[2] = {1.f, 1.f}; quad.uv[3] = {0.f, 1.f};

		quad.eDir = FACE_DIR::POS_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;


		quad.color[0] = CBlock3::GetBaseColor(curType);
		quad.color[1] = CBlock3::GetBaseColor(curType);
		quad.color[2] = CBlock3::GetBaseColor(curType);
		quad.color[3] = CBlock3::GetBaseColor(curType);

		alphaTestQuads.push_back(quad);
	}
}

void CChunk3::BuildTorchMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& alphaTestQuads) const
{// 횃불은 본래 밝은 블록이므로 최대 광원(0xFF) 처리하거나 고유 인덱스 라이팅 처리
	uint8_t blockLight = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)].GetLight();

	// [Top]
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + (7 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f)};
		quad.v[1] = {fx + (9 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f)};
		quad.v[2] = {fx + (9 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f)};
		quad.v[3] = {fx + (7 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f)};

		quad.uv[0] = {7.0f / 16.f, 6.0f / 16.f};
		quad.uv[1] = {9.0f / 16.f, 6.0f / 16.f};
		quad.uv[2] = {9.0f / 16.f, 8.0f / 16.f};
		quad.uv[3] = {7.0f / 16.f, 8.0f / 16.f};

		quad.eDir = FACE_DIR::POS_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		alphaTestQuads.push_back(quad);
	}

	// [Bottom]
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + (7 / 16.f), fy, fz + (6 / 16.f)};
		quad.v[1] = {fx + (9 / 16.f), fy, fz + (6 / 16.f)};
		quad.v[2] = {fx + (9 / 16.f), fy, fz + (8 / 16.f)};
		quad.v[3] = {fx + (7 / 16.f), fy, fz + (8 / 16.f)};

		quad.uv[0] = {7.0f / 16.f, 10.0f / 16.f};
		quad.uv[1] = {9.0f / 16.f, 10.0f / 16.f};
		quad.uv[2] = {9.0f / 16.f, 8.0f / 16.f};
		quad.uv[3] = {7.0f / 16.f, 8.0f / 16.f};

		quad.eDir = FACE_DIR::NEG_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		alphaTestQuads.push_back(quad);
	}

	// [Front]
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + (9 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f)};
		quad.v[1] = {fx + (7 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f)};
		quad.v[2] = {fx + (7 / 16.f), fy,               fz + (8 / 16.f)};
		quad.v[3] = {fx + (9 / 16.f), fy,               fz + (8 / 16.f)};

		quad.uv[0] = {9.0f / 16.f, 6.0f / 16.f};
		quad.uv[1] = {7.0f / 16.f, 6.0f / 16.f};
		quad.uv[2] = {7.0f / 16.f, 16.0f / 16.f};
		quad.uv[3] = {9.0f / 16.f, 16.0f / 16.f};

		quad.eDir = FACE_DIR::POS_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		alphaTestQuads.push_back(quad);
	}

	// [Back]
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + (7 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f)};
		quad.v[1] = {fx + (9 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f)};
		quad.v[2] = {fx + (9 / 16.f), fy,               fz + (6 / 16.f)};
		quad.v[3] = {fx + (7 / 16.f), fy,               fz + (6 / 16.f)};

		quad.uv[0] = {7.0f / 16.f, 6.0f / 16.f};
		quad.uv[1] = {9.0f / 16.f, 6.0f / 16.f};
		quad.uv[2] = {9.0f / 16.f, 16.0f / 16.f};
		quad.uv[3] = {7.0f / 16.f, 16.0f / 16.f};

		quad.eDir = FACE_DIR::NEG_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		alphaTestQuads.push_back(quad);
	}

	// [Right]
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + (9 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f)};
		quad.v[1] = {fx + (9 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f)};
		quad.v[2] = {fx + (9 / 16.f), fy,               fz + (8 / 16.f)};
		quad.v[3] = {fx + (9 / 16.f), fy,               fz + (6 / 16.f)};

		quad.uv[0] = {7.0f / 16.f, 6.0f / 16.f};
		quad.uv[1] = {9.0f / 16.f, 6.0f / 16.f};
		quad.uv[2] = {9.0f / 16.f, 16.0f / 16.f};
		quad.uv[3] = {7.0f / 16.f, 16.0f / 16.f};

		quad.eDir = FACE_DIR::POS_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		alphaTestQuads.push_back(quad);
	}

	// [Left]
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + (7 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f)};
		quad.v[1] = {fx + (7 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f)};
		quad.v[2] = {fx + (7 / 16.f), fy,               fz + (6 / 16.f)};
		quad.v[3] = {fx + (7 / 16.f), fy,               fz + (8 / 16.f)};

		quad.uv[0] = {9.0f / 16.f, 6.0f / 16.f};
		quad.uv[1] = {7.0f / 16.f, 6.0f / 16.f};
		quad.uv[2] = {7.0f / 16.f, 16.0f / 16.f};
		quad.uv[3] = {9.0f / 16.f, 16.0f / 16.f};

		quad.eDir = FACE_DIR::NEG_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		alphaTestQuads.push_back(quad);
	}
}

void CChunk3::BuildSlapMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& solidQuads) const
{
	// 1. 블록 자체의 광원 밝기 데이터
	//uint8_t blockLight = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)].GetLight();
	uint8_t blockLight = 240; // TODO Replace

	// 2. 블록 클래스로부터 반블록의 고유 틴트 컬러 가져오기
	uint32_t blockColor = CBlock3::GetBaseColor(curType);

	// 3. 반블록의 Y축 상단 높이 정의 (절반 높이)
	float halfY = fy + (8.f / 16.f); // fy + 0.5f

	// [Top] - 일반 큐브와 동일하지만 높이만 0.5f 지점입니다.
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        halfY, fz + 1.f};
		quad.v[1] = {fx + 1.f,  halfY, fz + 1.f};
		quad.v[2] = {fx + 1.f,  halfY, fz};
		quad.v[3] = {fx,        halfY, fz};

		quad.uv[0] = {0.f, 0.f};
		quad.uv[1] = {1.f, 0.f};
		quad.uv[2] = {1.f, 1.f};
		quad.uv[3] = {0.f, 1.f};

		quad.eDir = FACE_DIR::POS_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		// AO는 일단 기본 밝기(3)로 채워두거나, 필요시 주변 검사 로직을 연동합니다.
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		solidQuads.push_back(quad);
	}

	// [Bottom] - 바닥면은 일반 큐브와 완전하게 동일합니다.
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        fy, fz};
		quad.v[1] = {fx + 1.f,  fy, fz};
		quad.v[2] = {fx + 1.f,  fy, fz + 1.f};
		quad.v[3] = {fx,        fy, fz + 1.f};

		quad.uv[0] = {0.f, 0.f};
		quad.uv[1] = {1.f, 0.f};
		quad.uv[2] = {1.f, 1.f};
		quad.uv[3] = {0.f, 1.f};

		quad.eDir = FACE_DIR::NEG_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		solidQuads.push_back(quad);
	}

	// 💡 [옆면들] 상단 정점들의 높이를 halfY로 낮추고, 
	// 텍스처가 늘어나는 걸 막기 위해 uv의 Y축(V)을 0.0f ~ 0.5f(절반) 지점까지만 매핑합니다.

	// [Front] (+Z면)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + 1.f,  halfY, fz + 1.f};
		quad.v[1] = {fx,        halfY, fz + 1.f};
		quad.v[2] = {fx,        fy,    fz + 1.f};
		quad.v[3] = {fx + 1.f,  fy,    fz + 1.f};

		quad.uv[0] = {1.f, 0.5f}; // 💡 상단 V 좌표를 0.5f로 커팅
		quad.uv[1] = {0.f, 0.5f};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::POS_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		solidQuads.push_back(quad);
	}

	// [Back] (-Z면)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        halfY, fz};
		quad.v[1] = {fx + 1.f,  halfY, fz};
		quad.v[2] = {fx + 1.f,  fy,    fz};
		quad.v[3] = {fx,        fy,    fz};

		quad.uv[0] = {1.f, 0.5f};
		quad.uv[1] = {0.f, 0.5f};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::NEG_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		solidQuads.push_back(quad);
	}

	// [Right] (+X면)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + 1.f,  halfY, fz};
		quad.v[1] = {fx + 1.f,  halfY, fz + 1.f};
		quad.v[2] = {fx + 1.f,  fy,    fz + 1.f};
		quad.v[3] = {fx + 1.f,  fy,    fz};

		quad.uv[0] = {1.f, 0.5f};
		quad.uv[1] = {0.f, 0.5f};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::POS_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		solidQuads.push_back(quad);
	}

	// [Left] (-X면)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        halfY, fz + 1.f};
		quad.v[1] = {fx,        halfY, fz};
		quad.v[2] = {fx,        fy,    fz};
		quad.v[3] = {fx,        fy,    fz + 1.f};

		quad.uv[0] = {1.f, 0.5f};
		quad.uv[1] = {0.f, 0.5f};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::NEG_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		solidQuads.push_back(quad);
	}
}
void CChunk3::BuildStairMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& solidQuads) const
{
	// 1. 블록 자체의 라이팅 및 컬러 데이터
	uint8_t blockLight = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)].GetLight();
	uint32_t blockColor = CBlock3::GetBaseColor(curType);
	uint8_t stairAO = 3; // 기본 AO 밝기

	// 2. 💡 블록의 방향(State) 정보 가져오기 (예: 0: POS_Z, 1: NEG_Z, 2: POS_X, 3: NEG_X)
	// 프레임워크 설계에 맞게 m_arrBlocks[...].GetState() 등으로 대체하세요.
	//uint8_t stairDir = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)].GetState();
	uint8_t stairDir = 0;
	// Y축 높이 경계선 정의
	float halfY = fy + 0.5f;
	float maxY = fy + 1.0f;

	// =========================================================================
	// PART 1. 밑판 상자 (Bottom Slab) - 하단 전체를 채우는 반블록 (0.0f ~ 0.5f)
	// =========================================================================

	// 밑판 윗면 (Top Face of Bottom Slab)
	// 💡 주의: 윗판 상자가 올라가는 부분은 가려지므로 원래 컬링해야 하지만, 
	// 계단 모양을 쉽게 보기 위해 우선 윗면 전체를 다 그리도록 처리합니다.
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        halfY, fz + 1.f}; quad.v[1] = {fx + 1.f,  halfY, fz + 1.f};
		quad.v[2] = {fx + 1.f,  halfY, fz}; quad.v[3] = {fx,        halfY, fz};
		quad.uv[0] = {0.f, 0.f}; quad.uv[1] = {1.f, 0.f}; quad.uv[2] = {1.f, 1.f}; quad.uv[3] = {0.f, 1.f};
		quad.eDir = FACE_DIR::POS_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 

		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}

	// 밑판 아랫면 (Bottom Face)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        fy, fz}; quad.v[1] = {fx + 1.f,  fy, fz};
		quad.v[2] = {fx + 1.f,  fy, fz + 1.f}; quad.v[3] = {fx,        fy, fz + 1.f};
		quad.uv[0] = {0.f, 0.f}; quad.uv[1] = {1.f, 0.f}; quad.uv[2] = {1.f, 1.f}; quad.uv[3] = {0.f, 1.f};
		quad.eDir = FACE_DIR::NEG_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 

		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}

	// 밑판 옆면 4개 (높이: fy ~ halfY, UV V축: 0.5f ~ 1.0f)
	// Front (+Z)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + 1.f,  halfY, fz + 1.f}; quad.v[1] = {fx,        halfY, fz + 1.f};
		quad.v[2] = {fx,        fy,    fz + 1.f}; quad.v[3] = {fx + 1.f,  fy,    fz + 1.f};
		quad.uv[0] = {1.f, 0.5f}; quad.uv[1] = {0.f, 0.5f}; quad.uv[2] = {0.f, 1.f}; quad.uv[3] = {1.f, 1.f};
		quad.eDir = FACE_DIR::POS_Z; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor; 
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}
	// Back (-Z)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        halfY, fz}; quad.v[1] = {fx + 1.f,  halfY, fz};
		quad.v[2] = {fx + 1.f,  fy,    fz}; quad.v[3] = {fx,        fy,    fz};
		quad.uv[0] = {1.f, 0.5f}; quad.uv[1] = {0.f, 0.5f}; quad.uv[2] = {0.f, 1.f}; quad.uv[3] = {1.f, 1.f};
		quad.eDir = FACE_DIR::NEG_Z; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor; 
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}
	// Right (+X)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + 1.f,  halfY, fz}; quad.v[1] = {fx + 1.f,  halfY, fz + 1.f};
		quad.v[2] = {fx + 1.f,  fy,    fz + 1.f}; quad.v[3] = {fx + 1.f,  fy,    fz};
		quad.uv[0] = {1.f, 0.5f}; quad.uv[1] = {0.f, 0.5f}; quad.uv[2] = {0.f, 1.f}; quad.uv[3] = {1.f, 1.f};
		quad.eDir = FACE_DIR::POS_X; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor; 
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}
	// Left (-X)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        halfY, fz + 1.f}; quad.v[1] = {fx,        halfY, fz};
		quad.v[2] = {fx,        fy,    fz}; quad.v[3] = {fx,        fy,    fz + 1.f};
		quad.uv[0] = {1.f, 0.5f}; quad.uv[1] = {0.f, 0.5f}; quad.uv[2] = {0.f, 1.f}; quad.uv[3] = {1.f, 1.f};
		quad.eDir = FACE_DIR::NEG_X; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor; 
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}


	// =========================================================================
	// PART 2. 윗판 상자 (Top Step) - 방향에 따라 절반 범위만 생성 (halfY ~ maxY)
	// =========================================================================

	// 방향(stairDir)에 따른 윗판 상자의 X, Z 좌표 바운더리 설정 수식
	float startX = fx, endX = fx + 1.f;
	float startZ = fz, endZ = fz + 1.f;

	// 예시 기준 (마인크래프트 기본 사양 반영):
	// stairDir == 0 (+Z를 바라봄): 계단의 벽이 -Z(뒤)에 생김 -> 윗판은 뒤쪽 절반(fz ~ fz+0.5f)
	// stairDir == 1 (-Z를 바라봄): 계단의 벽이 +Z(앞)에 생김 -> 윗판은 앞쪽 절반(fz+0.5f ~ fz+1.0f)
	// stairDir == 2 (+X를 바라봄): 계단의 벽이 -X(왼쪽)에 생김 -> 윗판은 왼쪽 절반(fx ~ fx+0.5f)
	// stairDir == 3 (-X를 바라봄): 계단의 벽이 +X(오른쪽)에 생김 -> 윗판은 오른쪽 절반(fx+0.5f ~ fx+1.0f)

	if (stairDir == 0)      endZ = fz + 0.5f;
	else if (stairDir == 1) startZ = fz + 0.5f;
	else if (stairDir == 2) endX = fx + 0.5f;
	else if (stairDir == 3) startX = fx + 0.5f;

	// 윗판 윗면 (Top Face)
	{
		VOX_QUAD quad{};
		quad.v[0] = {startX,     maxY,  endZ}; quad.v[1] = {endX,       maxY,  endZ};
		quad.v[2] = {endX,       maxY,  startZ}; quad.v[3] = {startX,     maxY,  startZ};
		quad.uv[0] = {0.f, 0.f}; quad.uv[1] = {1.f, 0.f}; quad.uv[2] = {1.f, 1.f}; quad.uv[3] = {0.f, 1.f};
		quad.eDir = FACE_DIR::POS_Y; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor; 
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}

	// 윗판 옆면 4개 (높이: halfY ~ maxY, UV V축: 0.0f ~ 0.5f)
	// Front (+Z)
	{
		VOX_QUAD quad{};
		quad.v[0] = {endX,       maxY,  endZ};   quad.v[1] = {startX,     maxY,  endZ};
		quad.v[2] = {startX,     halfY, endZ};   quad.v[3] = {endX,       halfY, endZ};
		quad.uv[0] = {1.f, 0.f}; quad.uv[1] = {0.f, 0.f}; quad.uv[2] = {0.f, 0.5f}; quad.uv[3] = {1.f, 0.5f};
		quad.eDir = FACE_DIR::POS_Z; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor; 
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}
	// Back (-Z)
	{
		VOX_QUAD quad{};
		quad.v[0] = {startX,     maxY,  startZ}; quad.v[1] = {endX,       maxY,  startZ};
		quad.v[2] = {endX,       halfY, startZ}; quad.v[3] = {startX,     halfY, startZ};
		quad.uv[0] = {1.f, 0.f}; quad.uv[1] = {0.f, 0.f}; quad.uv[2] = {0.f, 0.5f}; quad.uv[3] = {1.f, 0.5f};
		quad.eDir = FACE_DIR::NEG_Z; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor; 
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}
	// Right (+X)
	{
		VOX_QUAD quad{};
		quad.v[0] = {endX,       maxY,  startZ}; quad.v[1] = {endX,       maxY,  endZ};
		quad.v[2] = {endX,       halfY, endZ};   quad.v[3] = {endX,       halfY, startZ};
		quad.uv[0] = {1.f, 0.f}; quad.uv[1] = {0.f, 0.f}; quad.uv[2] = {0.f, 0.5f}; quad.uv[3] = {1.f, 0.5f};
		quad.eDir = FACE_DIR::POS_X; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		
		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;

		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}
	// Left (-X)
	{
		VOX_QUAD quad{};
		quad.v[0] = {startX,     maxY,  endZ};   quad.v[1] = {startX,     maxY,  startZ};
		quad.v[2] = {startX,     halfY, startZ}; quad.v[3] = {startX,     halfY, endZ};
		quad.uv[0] = {1.f, 0.f}; quad.uv[1] = {0.f, 0.f}; quad.uv[2] = {0.f, 0.5f}; quad.uv[3] = {1.f, 0.5f};
		quad.eDir = FACE_DIR::NEG_X; quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight; 

		quad.color[0] = blockColor;
		quad.color[1] = blockColor;
		quad.color[2] = blockColor;
		quad.color[3] = blockColor;
		
		
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = stairAO;
		solidQuads.push_back(quad);
	}
}

void CChunk3::BuildWaterMesh(int x, int y, int z, std::vector<VOX_QUAD>& waterQuads, std::vector<CChunk3*>& vecAdjChunks) const
{
	float fx = (float)x;
	float fy = (float)y;
	float fz = (float)z;

	CBlock3 currentBlock = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)];

	uint8_t blockLight = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)].GetLight();
	uint32_t blockColor = CBlock3::GetBaseColor(currentBlock.GetType());

	// 💡 1. 3비트 수위 (0 ~ 7) 읽기
	// m_iFlag에서 하위 3비트만 마스킹해서 가져옵니다. (예: currentBlock.GetFlag() & 0x07)
	uint8_t waterLevel = currentBlock.GetFlag() & 0x07;

	// 💡 2. 8분법 공식 적용 (0일 때 1/8, 7일 때 8/8 즉 1.0)
	float hTop = fy + (((float)waterLevel + 1.f) / 8.f);
	float vTop = 1.0f - (hTop - fy);

	// -------------------------------------------------------------
	// 💡 수평 옆면 렌더링을 위한 헬퍼 람다 함수 (8분법 보정 반영)
	// -------------------------------------------------------------
	auto ProcessHorizontalFace = [&](FACE_DIR dir, int nx, int ny, int nz, auto&& BuildQuadFunc) {
		std::optional<CBlock3> optNext = GetBlockAt(nx, ny, nz, vecAdjChunks);

		bool bExpose = false;
		float hBottom = fy;      // 기본 사각형 바닥 높이
		float vBottom = 1.0f;    // 기본 사각형 바닥 UV

		if (!optNext.has_value()) {
			bExpose = true;
		}
		else if (optNext->GetType() == CBlock3::TYPE::WATER_STILL || optNext->GetType() == CBlock3::TYPE::WATER_FLOW) {
			uint8_t nextLevel = optNext->GetFlag() & 0x07; // 이웃 물도 똑같이 3비트 마스킹

			// 내 수위가 더 높을 때만 "그 차이만큼" 단차면을 렌더링
			if (waterLevel > nextLevel) {
				bExpose = true;
				// 💡 보정: 이웃 물의 높이만큼 사각형 바닥을 들어올림 (8분법 공식)
				hBottom = fy + (((float)nextLevel + 1.f) / 8.f);
				// 💡 UV 보정: 들어올려진 바닥 높이에 맞춰 UV 텍스처 컷팅
				vBottom = 1.0f - (hBottom - fy);
			}
		}
		else {
			// 일반 블록(AIR 등)인 경우 통째로 다 그림
			bExpose = IsFaceExposed(currentBlock, dir, optNext);
		}

		if (bExpose) {
			BuildQuadFunc(hBottom, vBottom);
		}
		};

	// [Top] 면 (수정된 hTop 반영)
	{
		int ny = y + 1;
		std::optional<CBlock3> optNextBlock = GetBlockAt(x, ny, z, vecAdjChunks);
		if (IsFaceExposed(currentBlock, FACE_DIR::POS_Y, optNextBlock)) {
			VOX_QUAD quad{};
			quad.v[0] = { fx,        hTop, fz + 1.f }; quad.v[1] = { fx + 1.f,  hTop, fz + 1.f };
			quad.v[2] = { fx + 1.f,  hTop, fz };       quad.v[3] = { fx,        hTop, fz };
			quad.uv[0] = { 0.f, 0.f }; quad.uv[1] = { 1.f, 0.f }; quad.uv[2] = { 1.f, 1.f }; quad.uv[3] = { 0.f, 1.f };
			quad.eDir = FACE_DIR::POS_Y;
			quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
			quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
			waterQuads.push_back(quad);
		}
	}

	// [Bottom] 면 (언제나 고정된 바닥 fy)
	{
		int ny = y - 1;
		std::optional<CBlock3> optNextBlock = GetBlockAt(x, ny, z, vecAdjChunks);
		if (IsFaceExposed(currentBlock, FACE_DIR::NEG_Y, optNextBlock)) {
			VOX_QUAD quad{};
			quad.v[0] = { fx,        fy, fz };          quad.v[1] = { fx + 1.f,  fy, fz };
			quad.v[2] = { fx + 1.f,  fy, fz + 1.f };    quad.v[3] = { fx,        fy, fz + 1.f };
			quad.uv[0] = { 0.f, 0.f }; quad.uv[1] = { 1.f, 0.f }; quad.uv[2] = { 1.f, 1.f }; quad.uv[3] = { 0.f, 1.f };
			quad.eDir = FACE_DIR::NEG_Y;
			quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
			quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
			waterQuads.push_back(quad);
		}
	}

	// [Front] (+Z면)
	ProcessHorizontalFace(FACE_DIR::POS_Z, x, y, z + 1, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx + 1.f,  hTop,    fz + 1.f };
		quad.v[1] = { fx,        hTop,    fz + 1.f };
		quad.v[2] = { fx,        hBottom, fz + 1.f };
		quad.v[3] = { fx + 1.f,  hBottom, fz + 1.f };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::POS_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});

	// [Back] (-Z면)
	ProcessHorizontalFace(FACE_DIR::NEG_Z, x, y, z - 1, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx,        hTop,    fz };
		quad.v[1] = { fx + 1.f,  hTop,    fz };
		quad.v[2] = { fx + 1.f,  hBottom, fz };
		quad.v[3] = { fx,        hBottom, fz };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::NEG_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});

	// [Right] (+X면)
	ProcessHorizontalFace(FACE_DIR::POS_X, x + 1, y, z, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx + 1.f,  hTop,    fz };
		quad.v[1] = { fx + 1.f,  hTop,    fz + 1.f };
		quad.v[2] = { fx + 1.f,  hBottom, fz + 1.f };
		quad.v[3] = { fx + 1.f,  hBottom, fz };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::POS_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});

	// [Left] (-X면)
	ProcessHorizontalFace(FACE_DIR::NEG_X, x - 1, y, z, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx,        hTop,    fz + 1.f };
		quad.v[1] = { fx,        hTop,    fz };
		quad.v[2] = { fx,        hBottom, fz };
		quad.v[3] = { fx,        hBottom, fz + 1.f };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::NEG_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});
}

void CChunk3::BuildLavaMesh(int x, int y, int z, std::vector<VOX_QUAD>& waterQuads, std::vector<CChunk3*>& vecAdjChunks) const
{
	float fx = (float)x;
	float fy = (float)y;
	float fz = (float)z;

	CBlock3 currentBlock = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)];

	uint8_t blockLight = m_arrBlocks[BlockIndexing((int)fx, (int)fy, (int)fz)].GetLight();
	uint32_t blockColor = CBlock3::GetBaseColor(currentBlock.GetType());

	// 💡 1. 3비트 수위 (0 ~ 7) 읽기
	// m_iFlag에서 하위 3비트만 마스킹해서 가져옵니다. (예: currentBlock.GetFlag() & 0x07)
	uint8_t waterLevel = currentBlock.GetFlag() & 0x03;

	// 💡 2. 8분법 공식 적용 (0일 때 1/8, 7일 때 8/8 즉 1.0)
	float hTop = fy + (((float)waterLevel + 1.f) / 4.f);
	float vTop = 1.0f - (hTop - fy);

	// -------------------------------------------------------------
	// 💡 수평 옆면 렌더링을 위한 헬퍼 람다 함수 (8분법 보정 반영)
	// -------------------------------------------------------------
	auto ProcessHorizontalFace = [&](FACE_DIR dir, int nx, int ny, int nz, auto&& BuildQuadFunc) {
		std::optional<CBlock3> optNext = GetBlockAt(nx, ny, nz, vecAdjChunks);

		bool bExpose = false;
		float hBottom = fy;      // 기본 사각형 바닥 높이
		float vBottom = 1.0f;    // 기본 사각형 바닥 UV

		if (!optNext.has_value()) {
			bExpose = true;
		}
		else if (optNext->GetType() == CBlock3::TYPE::LAVA_STILL || optNext->GetType() == CBlock3::TYPE::LAVA_FLOW) {
			uint8_t nextLevel = optNext->GetFlag() & 0x07; // 이웃 물도 똑같이 3비트 마스킹

			// 내 수위가 더 높을 때만 "그 차이만큼" 단차면을 렌더링
			if (waterLevel > nextLevel) {
				bExpose = true;
				// 💡 보정: 이웃 물의 높이만큼 사각형 바닥을 들어올림 (8분법 공식)
				hBottom = fy + (((float)nextLevel + 1.f) / 4.f);
				// 💡 UV 보정: 들어올려진 바닥 높이에 맞춰 UV 텍스처 컷팅
				vBottom = 1.0f - (hBottom - fy);
			}
		}
		else {
			// 일반 블록(AIR 등)인 경우 통째로 다 그림
			bExpose = IsFaceExposed(currentBlock, dir, optNext);
		}

		if (bExpose) {
			BuildQuadFunc(hBottom, vBottom);
		}
		};

	// [Top] 면 (수정된 hTop 반영)
	{
		int ny = y + 1;
		std::optional<CBlock3> optNextBlock = GetBlockAt(x, ny, z, vecAdjChunks);
		if (IsFaceExposed(currentBlock, FACE_DIR::POS_Y, optNextBlock)) {
			VOX_QUAD quad{};
			quad.v[0] = { fx,        hTop, fz + 1.f }; quad.v[1] = { fx + 1.f,  hTop, fz + 1.f };
			quad.v[2] = { fx + 1.f,  hTop, fz };       quad.v[3] = { fx,        hTop, fz };
			quad.uv[0] = { 0.f, 0.f }; quad.uv[1] = { 1.f, 0.f }; quad.uv[2] = { 1.f, 1.f }; quad.uv[3] = { 0.f, 1.f };
			quad.eDir = FACE_DIR::POS_Y;
			quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
			quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
			waterQuads.push_back(quad);
		}
	}

	// [Bottom] 면 (언제나 고정된 바닥 fy)
	{
		int ny = y - 1;
		std::optional<CBlock3> optNextBlock = GetBlockAt(x, ny, z, vecAdjChunks);
		if (IsFaceExposed(currentBlock, FACE_DIR::NEG_Y, optNextBlock)) {
			VOX_QUAD quad{};
			quad.v[0] = { fx,        fy, fz };          quad.v[1] = { fx + 1.f,  fy, fz };
			quad.v[2] = { fx + 1.f,  fy, fz + 1.f };    quad.v[3] = { fx,        fy, fz + 1.f };
			quad.uv[0] = { 0.f, 0.f }; quad.uv[1] = { 1.f, 0.f }; quad.uv[2] = { 1.f, 1.f }; quad.uv[3] = { 0.f, 1.f };
			quad.eDir = FACE_DIR::NEG_Y;
			quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
			quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
			waterQuads.push_back(quad);
		}
	}

	// [Front] (+Z면)
	ProcessHorizontalFace(FACE_DIR::POS_Z, x, y, z + 1, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx + 1.f,  hTop,    fz + 1.f };
		quad.v[1] = { fx,        hTop,    fz + 1.f };
		quad.v[2] = { fx,        hBottom, fz + 1.f };
		quad.v[3] = { fx + 1.f,  hBottom, fz + 1.f };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::POS_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});

	// [Back] (-Z면)
	ProcessHorizontalFace(FACE_DIR::NEG_Z, x, y, z - 1, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx,        hTop,    fz };
		quad.v[1] = { fx + 1.f,  hTop,    fz };
		quad.v[2] = { fx + 1.f,  hBottom, fz };
		quad.v[3] = { fx,        hBottom, fz };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::NEG_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});

	// [Right] (+X면)
	ProcessHorizontalFace(FACE_DIR::POS_X, x + 1, y, z, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx + 1.f,  hTop,    fz };
		quad.v[1] = { fx + 1.f,  hTop,    fz + 1.f };
		quad.v[2] = { fx + 1.f,  hBottom, fz + 1.f };
		quad.v[3] = { fx + 1.f,  hBottom, fz };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::POS_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});

	// [Left] (-X면)
	ProcessHorizontalFace(FACE_DIR::NEG_X, x - 1, y, z, [&](float hBottom, float vBottom) {
		VOX_QUAD quad{};
		quad.v[0] = { fx,        hTop,    fz + 1.f };
		quad.v[1] = { fx,        hTop,    fz };
		quad.v[2] = { fx,        hBottom, fz };
		quad.v[3] = { fx,        hBottom, fz + 1.f };

		quad.uv[0] = { 1.f, vTop }; quad.uv[1] = { 0.f, vTop };
		quad.uv[2] = { 0.f, vBottom }; quad.uv[3] = { 1.f, vBottom };

		quad.eDir = FACE_DIR::NEG_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(currentBlock.GetType(), quad.eDir));
		quad.lighting = blockLight; quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor; quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;
		waterQuads.push_back(quad);
		});
}

/*


void CChunk3::BuildWaterMesh(int x, int y, int z, std::vector<VOX_QUAD>& waterQuads, std::vector<CChunk3*>& vecAdjChunks) const
{
	float fx = (float)x;
	float fy = (float)y;
	float fz = (float)z;

	// 1. 블록 자체의 광원 및 색상 데이터 유지
	uint8_t blockLight = 240;
	uint32_t blockColor = CBlock3::GetBaseColor(CBlock3::TYPE::WATER_PLACE_HOLDER);

	// 2. 💡 4개 꼭짓점(Corner)의 수위를 주변 상태를 보고 각각 동적 계산
	uint8_t levelTL = GetCornerWaterLevel(x, y, z + 1, vecAdjChunks); // Back-Left
	uint8_t levelTR = GetCornerWaterLevel(x + 1, y, z + 1, vecAdjChunks); // Back-Right
	uint8_t levelBL = GetCornerWaterLevel(x, y, z, vecAdjChunks); // Front-Left
	uint8_t levelBR = GetCornerWaterLevel(x + 1, y, z, vecAdjChunks); // Front-Right

	// 3. 💡 수위 레벨(0~8)을 실제 렌더링에 사용할 Y축 높이 좌표로 맵핑하는 람다 함수
	auto LevelToHeight = [&](uint8_t level) -> float {
		if (level == 0) return fy + 0.05f; // 물이 없어도 최소 두께 유지
		return fy + ((float)level / 8.f); // 레벨 8일 때 가득 찬 큐브(1.0f)
		};

	float hTL = LevelToHeight(levelTL);
	float hTR = LevelToHeight(levelTR);
	float hBL = LevelToHeight(levelBL);
	float hBR = LevelToHeight(levelBR);

	// 각 꼭짓점별 텍스처 밀림 방지용 상단 V 좌표 계산
	float vTL = 1.0f - ((hTL - fy));
	float vTR = 1.0f - ((hTR - fy));
	float vBL = 1.0f - ((hBL - fy));
	float vBR = 1.0f - ((hBR - fy));

	// [Top] - 💡 4개 정점의 Y축 높이가 달라서 부드러운 경사면이 형성됩니다.
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        hTL, fz + 1.f}; // Back-Left
		quad.v[1] = {fx + 1.f,  hTR, fz + 1.f}; // Back-Right
		quad.v[2] = {fx + 1.f,  hBR, fz};       // Front-Right
		quad.v[3] = {fx,        hBL, fz};       // Front-Left

		quad.uv[0] = {0.f, 0.f};
		quad.uv[1] = {1.f, 0.f};
		quad.uv[2] = {1.f, 1.f};
		quad.uv[3] = {0.f, 1.f};

		quad.eDir = FACE_DIR::POS_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(CBlock3::TYPE::WATER_PLACE_HOLDER, quad.eDir));
		quad.lighting = blockLight;

		quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;

		waterQuads.push_back(quad);
	}

	// [Bottom] - 바닥면 (평평한 큐브 바닥 그대로 유지)
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        fy, fz};
		quad.v[1] = {fx + 1.f,  fy, fz};
		quad.v[2] = {fx + 1.f,  fy, fz + 1.f};
		quad.v[3] = {fx,        fy, fz + 1.f};

		quad.uv[0] = {0.f, 0.f};
		quad.uv[1] = {1.f, 0.f};
		quad.uv[2] = {1.f, 1.f};
		quad.uv[3] = {0.f, 1.f};

		quad.eDir = FACE_DIR::NEG_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(CBlock3::TYPE::WATER_PLACE_HOLDER, quad.eDir));
		quad.lighting = blockLight;

		quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;

		waterQuads.push_back(quad);
	}

	// [Front] (+Z면) - hTR(우상)과 hTL(좌상)의 경사 적용
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + 1.f,  hTR, fz + 1.f};
		quad.v[1] = {fx,        hTL, fz + 1.f};
		quad.v[2] = {fx,        fy,  fz + 1.f};
		quad.v[3] = {fx + 1.f,  fy,  fz + 1.f};

		quad.uv[0] = {1.f, vTR};
		quad.uv[1] = {0.f, vTL};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::POS_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(CBlock3::TYPE::WATER_PLACE_HOLDER, quad.eDir));
		quad.lighting = blockLight;

		quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;

		waterQuads.push_back(quad);
	}

	// [Back] (-Z면) - hTL(우상)과 hTR(좌상)의 경사 적용
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        hTL, fz};
		quad.v[1] = {fx + 1.f,  hTR, fz};
		quad.v[2] = {fx + 1.f,  fy,  fz};
		quad.v[3] = {fx,        fy,  fz};

		quad.uv[0] = {1.f, vTL};
		quad.uv[1] = {0.f, vTR};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::NEG_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(CBlock3::TYPE::WATER_PLACE_HOLDER, quad.eDir));
		quad.lighting = blockLight;

		quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;

		waterQuads.push_back(quad);
	}

	// [Right] (+X면) - hBR(우상)과 hTR(좌상)의 경사 적용
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx + 1.f,  hBR, fz};
		quad.v[1] = {fx + 1.f,  hTR, fz + 1.f};
		quad.v[2] = {fx + 1.f,  fy,  fz + 1.f};
		quad.v[3] = {fx + 1.f,  fy,  fz};

		quad.uv[0] = {1.f, vBR};
		quad.uv[1] = {0.f, vTR};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::POS_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(CBlock3::TYPE::WATER_PLACE_HOLDER, quad.eDir));
		quad.lighting = blockLight;

		quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;

		waterQuads.push_back(quad);
	}

	// [Left] (-X면) - hTL(우상)과 hBL(좌상)의 경사 적용
	{
		VOX_QUAD quad{};
		quad.v[0] = {fx,        hTL, fz + 1.f};
		quad.v[1] = {fx,        hBL, fz};
		quad.v[2] = {fx,        fy,  fz};
		quad.v[3] = {fx,        fy,  fz + 1.f};

		quad.uv[0] = {1.f, vTL};
		quad.uv[1] = {0.f, vBL};
		quad.uv[2] = {0.f, 1.f};
		quad.uv[3] = {1.f, 1.f};

		quad.eDir = FACE_DIR::NEG_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(CBlock3::TYPE::WATER_PLACE_HOLDER, quad.eDir));
		quad.lighting = blockLight;

		quad.color[0] = quad.color[1] = quad.color[2] = quad.color[3] = blockColor;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 3;

		waterQuads.push_back(quad);
	}
}

*/

//uint8_t CChunk3::GetCornerWaterLevel(int cx, int cy, int cz, std::vector<CChunk3*>& vecAdjChunks) const
//{
//	// 하나의 꼭짓점을 공유하는 상하좌우 4개의 셀 오프셋
//	int dx[4] = { -1,  0, -1, 0 };
//	int dz[4] = { -1, -1,  0, 0 };
//
//	uint8_t maxLevel = 0;
//	int waterCount = 0;
//	uint8_t centerLevel = 4; // 폴백용 기본값
//
//	for (int i = 0; i < 4; ++i)
//	{
//		int nx = cx + dx[i];
//		int nz = cz + dz[i];
//
//		// 기존에 사용하시던 안전한 블록 타입 추출 함수 활용
//		CBlock3::TYPE neighborType;
//
//		auto optBlock = GetBlockAt(nx, cy, nz, vecAdjChunks);
//		if (optBlock)
//		{
//			neighborType = optBlock.value().GetType();
//		}
//		else
//		{
//			neighborType = CBlock3::TYPE::AIR;
//		}
//		//CBlock3::TYPE neighborType = GetBlockTypeAt(nx, cy, nz, pPX, pMX, pPZ, pMZ);
//
//		if (neighborType == CBlock3::TYPE::WATER_PLACE_HOLDER)
//		{
//			// TODO: 실제 CBlock3에 구현된 수위 getter 연동 (예: GetWaterLevel())
//			// 지금은 알고리즘 테스트를 위해 x축 방향으로 갈수록 수위가 낮아지는 경사면 레이아웃을 임시 적용합니다.
//			uint8_t nLevel = 7 - (abs(nx) % 6);
//			if (nLevel < 1) nLevel = 1;
//
//			if (nLevel > maxLevel) maxLevel = nLevel;
//			waterCount++;
//		}
//		else if (neighborType != CBlock3::TYPE::AIR)
//		{
//			// 벽면(SOLID)에 붙은 코너 처리를 위해 카운트만 유지하거나 패스합니다.
//		}
//	}
//
//	// 주변에 물이 아예 없다면 바닥에 붙도록 처리
//	if (waterCount == 0) return 0;
//	return maxLevel;
//}

CChunk3::CChunk3()
{
}

CChunk3::~CChunk3()
{
}

HRESULT CChunk3::Initialize(const DESC& desc)
{
	m_iX = desc.iX;
	m_iY = desc.iY;
	m_iZ = desc.iZ;
	m_iChunkCoord = desc.iChunkCoord;

	m_pResCBufferPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT);

	_float3 vBoxCenter = {
		(m_iX * (int32_t)VOXEL_CHUNK_X_SIZE3) + ((int32_t)VOXEL_CHUNK_X_SIZE3) * 0.5f,
		(m_iY * (int32_t)VOXEL_CHUNK_Y_SIZE3) + ((int32_t)VOXEL_CHUNK_Y_SIZE3) * 0.5f,
		(m_iZ * (int32_t)VOXEL_CHUNK_Z_SIZE3) + ((int32_t)VOXEL_CHUNK_Z_SIZE3) * 0.5f,
		
	};

	_float3 vBoxExtents = {
		((int32_t)VOXEL_CHUNK_X_SIZE3) * 0.5f,
		((int32_t)VOXEL_CHUNK_Y_SIZE3) * 0.5f,
		((int32_t)VOXEL_CHUNK_Z_SIZE3) * 0.5f,
	};

	m_pCollBox = CCollBox::Create(vBoxCenter, vBoxExtents);

	return S_OK;
}

void CChunk3::QuadsToVerticies(std::vector<VOX_QUAD>& quads, std::vector<E::VTX_VOXEL>& vertices, std::vector<uint32_t>& indices)
{
	vertices.clear();
	indices.clear();

	vertices.reserve(quads.size() * 4);
	indices.reserve(quads.size() * 6);

	for (uint32_t i = 0; i < quads.size(); ++i)
	{
		uint32_t iFaceDir = ETOUI(quads[i].eDir);

		// 4개 정점 데이터 준비
		E::VTX_VOXEL v[4]{};
		v[0].pos = quads[i].v[0];
		v[1].pos = quads[i].v[1];
		v[2].pos = quads[i].v[2];
		v[3].pos = quads[i].v[3];


		{
			v[0].texCoord = quads[i].uv[0];
			v[1].texCoord = quads[i].uv[1];
			v[2].texCoord = quads[i].uv[2];
			v[3].texCoord = quads[i].uv[3];
		}
		{
			v[0].vColor = quads[i].color[0];
			v[1].vColor = quads[i].color[1];
			v[2].vColor = quads[i].color[2];
			v[3].vColor = quads[i].color[3];
		}



		// 공통 데이터 인코딩 및 패킹 (AO값만 고유 적용)
		for (uint32_t vIdx = 0; vIdx < 4; ++vIdx)
		{
			v[vIdx].packedData = {};
			v[vIdx].packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;              // normal (3bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(quads[i].ao[vIdx]) & 0x03) << 27;     // vertexao (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(vIdx) & 0x03) << 25;                  // vertexid (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(quads[i].blockTexType) & 0xff) << 17; // textureid (8bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(quads[i].lighting) & 0xff) << 9;      // Light (8bit)
		}

		vertices.push_back(v[0]);
		vertices.push_back(v[1]);
		vertices.push_back(v[2]);
		vertices.push_back(v[3]);

		uint32_t startIndex = i * 4;

		// 비대칭 이방성(Anisotropy) 보정 처리
		// 0번-2번 대각선의 AO 합산이 1번-3번보다 작다면(즉, 더 어둡다면) 삼각형 쪼개기 방향을 회전시킵니다.
		if (quads[i].ao[0] + quads[i].ao[2] < quads[i].ao[1] + quads[i].ao[3])
		{
			// 1-2-3, 1-3-0 형태로 인덱스 배치
			indices.push_back(startIndex + 1);
			indices.push_back(startIndex + 2);
			indices.push_back(startIndex + 3);
			indices.push_back(startIndex + 1);
			indices.push_back(startIndex + 3);
			indices.push_back(startIndex + 0);
		}
		else
		{
			// 기존 순서: 0-1-2, 0-2-3
			indices.push_back(startIndex + 0);
			indices.push_back(startIndex + 1);
			indices.push_back(startIndex + 2);
			indices.push_back(startIndex + 0);
			indices.push_back(startIndex + 2);
			indices.push_back(startIndex + 3);
		}
	}
}

HRESULT CChunk3::CreateBuffer(SPtr<CResDynamicVIBuffer>& pResBuffer, std::vector<E::VTX_VOXEL>& vertices, std::vector<uint32_t>& indices)
{
	E::CResDynamicVIBuffer::DESC desc{};
	desc.iNumVertices = (uint32_t)vertices.size();
	desc.iVertexStride = sizeof(E::VTX_VOXEL);
	desc.vertexDesc = {
		.ByteWidth = desc.iNumVertices * desc.iVertexStride,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};
	desc.vertexSubResource = {
		.pSysMem = vertices.data()
	};

	desc.iIndexStride = sizeof(uint32_t);
	desc.iNumIndices = (uint32_t)indices.size();
	desc.IndexDesc = {
		.ByteWidth = desc.iNumIndices * desc.iIndexStride,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};
	desc.indexSubResource = {
		.pSysMem = indices.data()
	};
	desc.eIndexFormat = DXGI_FORMAT_R32_UINT;
	desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	pResBuffer = E::CResDynamicVIBuffer::Create();
	if (FAILED(pResBuffer->Load(desc)))
	{
		return E_FAIL;
	};

	return S_OK;
}

UPtr<CChunk3> CChunk3::Create(const DESC& desc)
{
	auto pInstnace = ToUPtr(new CChunk3{});
	if (FAILED(pInstnace->Initialize(desc)))
	{
		return nullptr;
	}
	return pInstnace;
}