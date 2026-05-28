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
					m_arrBlocks[idx].SetType(k <= SEA_LEVEL ? CBlock3::TYPE::WATER_PLACE_HOLDER : CBlock3::TYPE::AIR);
				}

				// -------------------------------------------------------------
				// [STEP 3] 후처리 (동굴 및 광물)
				// -------------------------------------------------------------
				CBlock3::TYPE currentType = m_arrBlocks[idx].GetType();
				if (k > BEDROCK_MAX_HEIGHT && k < height - 4)
				{
					if (m_arrBlocks[idx].GetType() == CBlock3::TYPE::WATER_PLACE_HOLDER)
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

	std::vector<VOX_QUAD> solidQuads{};
	std::vector<VOX_QUAD> alphaTestQuads{};
	std::vector<VOX_QUAD> waterQuads{};
	NiveFaceCulling(solidQuads, alphaTestQuads, waterQuads);

	m_SolidIndices.clear();
	m_SolidVertices.clear();

	m_SolidVertices.reserve(solidQuads.size() * 4);
	m_SolidIndices.reserve(solidQuads.size() * 6);

	for (uint32_t i = 0; i < solidQuads.size(); ++i)
	{
		uint32_t iFaceDir = ETOUI(solidQuads[i].eDir);

		// 4개 정점 데이터 준비
		E::VTX_VOXEL v[4]{};
		v[0].pos = solidQuads[i].v1;
		v[1].pos = solidQuads[i].v2;
		v[2].pos = solidQuads[i].v3;
		v[3].pos = solidQuads[i].v4;


		{
			v[0].texCoord = solidQuads[i].uv1;
			v[1].texCoord = solidQuads[i].uv2;
			v[2].texCoord = solidQuads[i].uv3;
			v[3].texCoord = solidQuads[i].uv4;
		}
		{
			v[0].vColor = solidQuads[i].color[0];
			v[1].vColor = solidQuads[i].color[1];
			v[2].vColor = solidQuads[i].color[2];
			v[3].vColor = solidQuads[i].color[3];
		}



		// 공통 데이터 인코딩 및 패킹 (AO값만 고유 적용)
		for (uint32_t vIdx = 0; vIdx < 4; ++vIdx)
		{
			v[vIdx].packedData = {};
			v[vIdx].packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;              // normal (3bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(solidQuads[i].ao[vIdx]) & 0x03) << 27;     // vertexao (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(vIdx) & 0x03) << 25;                  // vertexid (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(solidQuads[i].blockTexType) & 0xff) << 17; // textureid (8bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(solidQuads[i].lighting) & 0xff) << 9;      // Light (8bit)
		}

		m_SolidVertices.push_back(v[0]);
		m_SolidVertices.push_back(v[1]);
		m_SolidVertices.push_back(v[2]);
		m_SolidVertices.push_back(v[3]);

		uint32_t startIndex = i * 4;

		// 비대칭 이방성(Anisotropy) 보정 처리
		// 0번-2번 대각선의 AO 합산이 1번-3번보다 작다면(즉, 더 어둡다면) 삼각형 쪼개기 방향을 회전시킵니다.
		if (solidQuads[i].ao[0] + solidQuads[i].ao[2] < solidQuads[i].ao[1] + solidQuads[i].ao[3])
		{
			// 1-2-3, 1-3-0 형태로 인덱스 배치
			m_SolidIndices.push_back(startIndex + 1);
			m_SolidIndices.push_back(startIndex + 2);
			m_SolidIndices.push_back(startIndex + 3);
			m_SolidIndices.push_back(startIndex + 1);
			m_SolidIndices.push_back(startIndex + 3);
			m_SolidIndices.push_back(startIndex + 0);
		}
		else
		{
			// 기존 순서: 0-1-2, 0-2-3
			m_SolidIndices.push_back(startIndex + 0);
			m_SolidIndices.push_back(startIndex + 1);
			m_SolidIndices.push_back(startIndex + 2);
			m_SolidIndices.push_back(startIndex + 0);
			m_SolidIndices.push_back(startIndex + 2);
			m_SolidIndices.push_back(startIndex + 3);
		}
	}



	m_AlphaTestIndices.clear();
	m_AlphaTestVertices.clear();

	m_AlphaTestVertices.reserve(alphaTestQuads.size() * 4);
	m_AlphaTestIndices.reserve(alphaTestQuads.size() * 6);

	for (uint32_t i = 0; i < alphaTestQuads.size(); ++i)
	{
		uint32_t iFaceDir = ETOUI(alphaTestQuads[i].eDir);

		E::VTX_VOXEL v[4]{};
		v[0].pos = alphaTestQuads[i].v1;
		v[1].pos = alphaTestQuads[i].v2;
		v[2].pos = alphaTestQuads[i].v3;
		v[3].pos = alphaTestQuads[i].v4;

		{
			v[0].texCoord = alphaTestQuads[i].uv1;
			v[1].texCoord = alphaTestQuads[i].uv2;
			v[2].texCoord = alphaTestQuads[i].uv3;
			v[3].texCoord = alphaTestQuads[i].uv4;
		}
		{
			v[0].vColor = alphaTestQuads[i].color[0];
			v[1].vColor = alphaTestQuads[i].color[1];
			v[2].vColor = alphaTestQuads[i].color[2];
			v[3].vColor = alphaTestQuads[i].color[3];
		}

		for (uint32_t vIdx = 0; vIdx < 4; ++vIdx)
		{
			v[vIdx].packedData = {};
			v[vIdx].packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;              // normal (3bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(alphaTestQuads[i].ao[vIdx]) & 0x03) << 27;     // vertexao (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(vIdx) & 0x03) << 25;                  // vertexid (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(alphaTestQuads[i].blockTexType) & 0xff) << 17; // textureid (8bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(alphaTestQuads[i].lighting) & 0xff) << 9;      // Light (8bit)
		}

		m_AlphaTestVertices.push_back(v[0]);
		m_AlphaTestVertices.push_back(v[1]);
		m_AlphaTestVertices.push_back(v[2]);
		m_AlphaTestVertices.push_back(v[3]);

		uint32_t startIndex = i * 4;

		// 물 쿼드 대각선 보정 처리 (주변 흙/돌 지형과 AO 라인을 맞추기 위해 동일하게 세팅)
		if (alphaTestQuads[i].ao[0] + alphaTestQuads[i].ao[2] < alphaTestQuads[i].ao[1] + alphaTestQuads[i].ao[3])
		{
			m_AlphaTestIndices.push_back(startIndex + 1);
			m_AlphaTestIndices.push_back(startIndex + 2);
			m_AlphaTestIndices.push_back(startIndex + 3);
			m_AlphaTestIndices.push_back(startIndex + 1);
			m_AlphaTestIndices.push_back(startIndex + 3);
			m_AlphaTestIndices.push_back(startIndex + 0);
		}
		else
		{
			m_AlphaTestIndices.push_back(startIndex + 0);
			m_AlphaTestIndices.push_back(startIndex + 1);
			m_AlphaTestIndices.push_back(startIndex + 2);
			m_AlphaTestIndices.push_back(startIndex + 0);
			m_AlphaTestIndices.push_back(startIndex + 2);
			m_AlphaTestIndices.push_back(startIndex + 3);
		}
	}



	m_WaterIndices.clear();
	m_WaterVertices.clear();

	m_WaterVertices.reserve(waterQuads.size() * 4);
	m_WaterIndices.reserve(waterQuads.size() * 6);

	for (uint32_t i = 0; i < waterQuads.size(); ++i)
	{
		uint32_t iFaceDir = ETOUI(waterQuads[i].eDir);

		E::VTX_VOXEL v[4]{};
		v[0].pos = waterQuads[i].v1;
		v[1].pos = waterQuads[i].v2;
		v[2].pos = waterQuads[i].v3;
		v[3].pos = waterQuads[i].v4;

		{
			v[0].texCoord = waterQuads[i].uv1;
			v[1].texCoord = waterQuads[i].uv2;
			v[2].texCoord = waterQuads[i].uv3;
			v[3].texCoord = waterQuads[i].uv4;
		}
		{
			v[0].vColor = waterQuads[i].color[0];
			v[1].vColor = waterQuads[i].color[1];
			v[2].vColor = waterQuads[i].color[2];
			v[3].vColor = waterQuads[i].color[3];
		}

		for (uint32_t vIdx = 0; vIdx < 4; ++vIdx)
		{
			v[vIdx].packedData = {};
			v[vIdx].packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;              // normal (3bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(waterQuads[i].ao[vIdx]) & 0x03) << 27;     // vertexao (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(vIdx) & 0x03) << 25;                  // vertexid (2bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(waterQuads[i].blockTexType) & 0xff) << 17; // textureid (8bit)
			v[vIdx].packedData |= (static_cast<uint32_t>(waterQuads[i].lighting) & 0xff) << 9;      // Light (8bit)
		}

		m_WaterVertices.push_back(v[0]);
		m_WaterVertices.push_back(v[1]);
		m_WaterVertices.push_back(v[2]);
		m_WaterVertices.push_back(v[3]);

		uint32_t startIndex = i * 4;

		// 물 쿼드 대각선 보정 처리 (주변 흙/돌 지형과 AO 라인을 맞추기 위해 동일하게 세팅)
		if (waterQuads[i].ao[0] + waterQuads[i].ao[2] < waterQuads[i].ao[1] + waterQuads[i].ao[3])
		{
			m_WaterIndices.push_back(startIndex + 1);
			m_WaterIndices.push_back(startIndex + 2);
			m_WaterIndices.push_back(startIndex + 3);
			m_WaterIndices.push_back(startIndex + 1);
			m_WaterIndices.push_back(startIndex + 3);
			m_WaterIndices.push_back(startIndex + 0);
		}
		else
		{
			m_WaterIndices.push_back(startIndex + 0);
			m_WaterIndices.push_back(startIndex + 1);
			m_WaterIndices.push_back(startIndex + 2);
			m_WaterIndices.push_back(startIndex + 0);
			m_WaterIndices.push_back(startIndex + 2);
			m_WaterIndices.push_back(startIndex + 3);
		}
	}


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
			E::CResDynamicVIBuffer::DESC desc{};
			desc.iNumVertices = (uint32_t)m_SolidVertices.size();
			desc.iVertexStride = sizeof(E::VTX_VOXEL);
			desc.vertexDesc = {
				.ByteWidth = desc.iNumVertices * desc.iVertexStride,
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_VERTEX_BUFFER,
				.CPUAccessFlags = 0,
				.MiscFlags = 0
			};
			desc.vertexSubResource = {
				.pSysMem = m_SolidVertices.data()
			};

			desc.iIndexStride = sizeof(uint32_t);
			desc.iNumIndices = (uint32_t)m_SolidIndices.size();
			desc.IndexDesc = {
				.ByteWidth = desc.iNumIndices * desc.iIndexStride,
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_INDEX_BUFFER,
				.CPUAccessFlags = 0,
				.MiscFlags = 0
			};
			desc.indexSubResource = {
				.pSysMem = m_SolidIndices.data()
			};
			desc.eIndexFormat = DXGI_FORMAT_R32_UINT;
			desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			m_pResSolidDynamicViBuffer = E::CResDynamicVIBuffer::Create();
			if (FAILED(m_pResSolidDynamicViBuffer->Load(desc)))
			{
				m_pResSolidDynamicViBuffer.reset();
				m_eBufferState = BUFFER_STATE::NON;
				return E_FAIL;
			};

			m_SolidIndices.clear();
			m_SolidVertices.clear();
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
			E::CResDynamicVIBuffer::DESC desc{};
			desc.iNumVertices = (uint32_t)m_AlphaTestVertices.size();
			desc.iVertexStride = sizeof(E::VTX_VOXEL);
			desc.vertexDesc = {
				.ByteWidth = desc.iNumVertices * desc.iVertexStride,
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_VERTEX_BUFFER,
				.CPUAccessFlags = 0,
				.MiscFlags = 0
			};
			desc.vertexSubResource = {
				.pSysMem = m_AlphaTestVertices.data()
			};

			desc.iIndexStride = sizeof(uint32_t);
			desc.iNumIndices = (uint32_t)m_AlphaTestIndices.size();
			desc.IndexDesc = {
				.ByteWidth = desc.iNumIndices * desc.iIndexStride,
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_INDEX_BUFFER,
				.CPUAccessFlags = 0,
				.MiscFlags = 0
			};
			desc.indexSubResource = {
				.pSysMem = m_AlphaTestIndices.data()
			};
			desc.eIndexFormat = DXGI_FORMAT_R32_UINT;
			desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			m_pResAlphaTestDynamicViBuffer = E::CResDynamicVIBuffer::Create();
			if (FAILED(m_pResAlphaTestDynamicViBuffer->Load(desc)))
			{
				m_pResAlphaTestDynamicViBuffer.reset();
				m_eBufferState = BUFFER_STATE::NON;
				return E_FAIL;
			};

			m_AlphaTestIndices.clear();
			m_AlphaTestVertices.clear();
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
			E::CResDynamicVIBuffer::DESC desc{};
			desc.iNumVertices = (uint32_t)m_WaterVertices.size();
			desc.iVertexStride = sizeof(E::VTX_VOXEL);
			desc.vertexDesc = {
				.ByteWidth = desc.iNumVertices * desc.iVertexStride,
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_VERTEX_BUFFER,
				.CPUAccessFlags = 0,
				.MiscFlags = 0
			};
			desc.vertexSubResource = {
				.pSysMem = m_WaterVertices.data()
			};

			desc.iIndexStride = sizeof(uint32_t);
			desc.iNumIndices = (uint32_t)m_WaterIndices.size();
			desc.IndexDesc = {
				.ByteWidth = desc.iNumIndices * desc.iIndexStride,
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_INDEX_BUFFER,
				.CPUAccessFlags = 0,
				.MiscFlags = 0
			};
			desc.indexSubResource = {
				.pSysMem = m_WaterIndices.data()
			};
			desc.eIndexFormat = DXGI_FORMAT_R32_UINT;
			desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			m_pResWaterDynamicViBuffer = E::CResDynamicVIBuffer::Create();
			if (FAILED(m_pResWaterDynamicViBuffer->Load(desc)))
			{
				m_pResWaterDynamicViBuffer.reset();
				m_eBufferState = BUFFER_STATE::NON;
				return E_FAIL;
			};


			m_WaterIndices.clear();
			m_WaterVertices.clear();
		}
		else
		{
			m_pResWaterDynamicViBuffer.reset();
		}
	}

	m_eBufferState = BUFFER_STATE::DONE;

	m_eMessingState = MESSING_STATE::NON;

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

CBlock3::TYPE CChunk3::GetBlockTypeAt(int32_t x, int32_t y, int32_t z, CChunk3* pPX, CChunk3* pMX, CChunk3* pPZ, CChunk3* pMZ) const
{
	if (y < 0 || y >= (int32_t)VOXEL_CHUNK_Y_SIZE3) return CBlock3::TYPE::AIR;

	if (x >= 0 && x < (int32_t)VOXEL_CHUNK_X_SIZE3 && z >= 0 && z < (int32_t)VOXEL_CHUNK_Z_SIZE3)
	{
		return m_arrBlocks[BlockIndexing(x, y, z)].GetType();
	}

	const CChunk3* pTargetChunk = this;
	int32_t localX = x;
	int32_t localZ = z;

	if (x >= (int32_t)VOXEL_CHUNK_X_SIZE3)
	{
		pTargetChunk = pPX;
		localX = x - (int32_t)VOXEL_CHUNK_X_SIZE3;
	}
	else if (x < 0)
	{
		pTargetChunk = pMX;
		localX = x + (int32_t)VOXEL_CHUNK_X_SIZE3;
	}

	if (z >= (int32_t)VOXEL_CHUNK_Z_SIZE3)
	{
		if (pTargetChunk != this) return CBlock3::TYPE::AIR; // 대각선 예외
		pTargetChunk = pPZ;
		localZ = z - (int32_t)VOXEL_CHUNK_Z_SIZE3;
	}
	else if (z < 0)
	{
		if (pTargetChunk != this) return CBlock3::TYPE::AIR; // 대각선 예외
		pTargetChunk = pMZ;
		localZ = z + (int32_t)VOXEL_CHUNK_Z_SIZE3;
	}

	if (pTargetChunk == nullptr) return CBlock3::TYPE::AIR;

	return pTargetChunk->m_arrBlocks[pTargetChunk->BlockIndexing(localX, y, localZ)].GetType();
}

void CChunk3::NiveFaceCulling(std::vector<VOX_QUAD>& solidQuads, std::vector<VOX_QUAD>& alphaTestQuads, std::vector<VOX_QUAD>& waterQuads) const
{
	CChunk3* pPlusX = CGameInstance::Get().GetVoxelChunk(m_iX + 1, m_iY, m_iZ);
	CChunk3* pMinusX = CGameInstance::Get().GetVoxelChunk(m_iX - 1, m_iY, m_iZ);
	CChunk3* pPlusZ = CGameInstance::Get().GetVoxelChunk(m_iX, m_iY, m_iZ + 1);
	CChunk3* pMinusZ = CGameInstance::Get().GetVoxelChunk(m_iX, m_iY, m_iZ - 1);

	for (int x = 0; x < (int)VOXEL_CHUNK_X_SIZE3; ++x)
	{
		for (int z = 0; z < (int)VOXEL_CHUNK_Z_SIZE3; ++z)
		{
			for (int y = 0; y < (int)VOXEL_CHUNK_Y_SIZE3; ++y)
			{
				uint32_t currentIdx = BlockIndexing(x, y, z);
				CBlock3::TYPE curType = m_arrBlocks[currentIdx].GetType();
				if (curType == CBlock3::TYPE::AIR) continue;


				bool isCurrentWater = (curType == CBlock3::TYPE::WATER_PLACE_HOLDER);
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



				// 1. Top (+Y)
				{
					int ny = y + 1;
					CBlock3::TYPE nextType = GetBlockTypeAt(x, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);

					bool bExpose = false;
					if (isCurrentWater || isCurrAlphaTest)
						bExpose = (nextType == CBlock3::TYPE::AIR); // 물 위에는 공기여야 면이 보임
					else
						bExpose = (nextType == CBlock3::TYPE::AIR || nextType == CBlock3::TYPE::WATER_PLACE_HOLDER); // 고체 위에는 공기나 물일 때 보임

					if (nextType == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(nextType))
					{
						bExpose = true;
					}

					if (curType == CBlock3::TYPE::TORCH_ON)
					{
						bExpose = true;
					}

					if (bExpose) {
						VOX_QUAD quad{};
						if (curType == CBlock3::TYPE::TORCH_ON)
						{
							// [정점 위치] 순서: 좌상 -> 우상 -> 우하 -> 좌하
							quad.v1 = { fx + (7 / 16.f),    fy + (10 / 16.f), fz + (8 / 16.f) }; // 좌상
							quad.v2 = { fx + (9 / 16.f),    fy + (10 / 16.f), fz + (8 / 16.f) }; // 우상
							quad.v3 = { fx + (9 / 16.f),    fy + (10 / 16.f), fz + (6 / 16.f) }; // 우하
							quad.v4 = { fx + (7 / 16.f),    fy + (10 / 16.f), fz + (6 / 16.f) }; // 좌하

							// [UV 좌표] 정점 위치와 정확히 1:1 매칭 (좌상 -> 우상 -> 우하 -> 좌하)
							quad.uv1 = _float2(7.0f / 16.f, 6.0f / 16.f); // 좌상
							quad.uv2 = _float2(9.0f / 16.f, 6.0f / 16.f); // 우상
							quad.uv3 = _float2(9.0f / 16.f, 8.0f / 16.f); // 우하
							quad.uv4 = _float2(7.0f / 16.f, 8.0f / 16.f); // 좌하
						}
						else
						{
							quad.v1 = { fx,     fy + 1, fz + 1 };
							quad.v2 = { fx + 1, fy + 1, fz + 1 };
							quad.v3 = { fx + 1, fy + 1, fz };
							quad.v4 = { fx,     fy + 1, fz };
						}

						
						quad.eDir = FACE_DIR::POS_Y;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
						quad.lighting = (y + 1 < (int)VOXEL_CHUNK_Y_SIZE3) ? m_arrBlocks[BlockIndexing(x, y + 1, z)].GetLight() : 0xFF;

						_bool bL = IsInsideOpaque(x - 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bF = IsInsideOpaque(x, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bB = IsInsideOpaque(x, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBL = IsInsideOpaque(x - 1, ny, fz - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBR = IsInsideOpaque(x + 1, ny, fz - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFL = IsInsideOpaque(x - 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFR = IsInsideOpaque(x + 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);

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
					CBlock3::TYPE nextType = GetBlockTypeAt(x, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);

					bool bExpose = false;
					if (isCurrentWater || isCurrAlphaTest)
						bExpose = (nextType == CBlock3::TYPE::AIR); // 밑면이 뚫린 연출(하늘에 떠있는 유체 등)이 아니면 물끼리는 컬링
					else
						bExpose = (nextType == CBlock3::TYPE::AIR || nextType == CBlock3::TYPE::WATER_PLACE_HOLDER);

					if (nextType == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(nextType))
					{
						bExpose = true;
					}
					if (curType == CBlock3::TYPE::TORCH_ON)
					{
						bExpose = true;
					}
					if (bExpose) {
						VOX_QUAD quad{};
						if (curType == CBlock3::TYPE::TORCH_ON)
						{
							// [정점 위치] 순서: 좌하 -> 우하 -> 우상 -> 좌상 (높이 fy 바닥 고정)
							quad.v1 = { fx + (7 / 16.f),    fy,               fz + (6 / 16.f) }; // 좌하
							quad.v2 = { fx + (9 / 16.f),    fy,               fz + (6 / 16.f) }; // 우하
							quad.v3 = { fx + (9 / 16.f),    fy,               fz + (8 / 16.f) }; // 우상
							quad.v4 = { fx + (7 / 16.f),    fy,               fz + (8 / 16.f) }; // 좌상

							// [UV 좌표] 일반 블록 밑면 UV 패턴 매칭에 맞게 뒤집어 배분 (8~10px 단면 영역)
							quad.uv1 = _float2(7.0f / 16.f, 10.0f / 16.f); // 좌하 (U:7, V:10)
							quad.uv2 = _float2(9.0f / 16.f, 10.0f / 16.f); // 우하 (U:9, V:10)
							quad.uv3 = _float2(9.0f / 16.f, 8.0f / 16.f); // 우상 (U:9, V:8)
							quad.uv4 = _float2(7.0f / 16.f, 8.0f / 16.f); // 좌상 (U:7, V:8)
						}
						else
						{
							quad.v1 = { fx,     fy, fz };
							quad.v2 = { fx + 1, fy, fz };
							quad.v3 = { fx + 1, fy, fz + 1 };
							quad.v4 = { fx,     fy, fz + 1 };
						}
						
						quad.eDir = FACE_DIR::NEG_Y;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
						quad.lighting = (y - 1 >= 0) ? m_arrBlocks[BlockIndexing(x, y - 1, z)].GetLight() : 0;

						_bool bL = IsInsideOpaque(x - 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bF = IsInsideOpaque(x, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bB = IsInsideOpaque(x, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBL = IsInsideOpaque(x - 1, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBR = IsInsideOpaque(x + 1, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFL = IsInsideOpaque(x - 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFR = IsInsideOpaque(x + 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bL, bB, bBL);
						quad.ao[1] = CalculateVertexAO(bR, bB, bBR);
						quad.ao[2] = CalculateVertexAO(bR, bF, bFR);
						quad.ao[3] = CalculateVertexAO(bL, bF, bFL);


						quad.color[0] = CBlock3::GetBaseColor(curType);
						quad.color[1] = CBlock3::GetBaseColor(curType);
						quad.color[2] = CBlock3::GetBaseColor(curType);
						quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 3. Front (+Z)
				{
					int nz = z + 1;
					CBlock3::TYPE nextType = GetBlockTypeAt(x, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);

					bool bExpose = false;
					if (isCurrentWater || isCurrAlphaTest)
						bExpose = (nextType == CBlock3::TYPE::AIR);
					else
						bExpose = (nextType == CBlock3::TYPE::AIR || nextType == CBlock3::TYPE::WATER_PLACE_HOLDER);

					if (nextType == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(nextType))
					{
						bExpose = true;
					}
					if (curType == CBlock3::TYPE::TORCH_ON)
					{
						bExpose = true;
					}
					if (bExpose) {
						VOX_QUAD quad{};

						if (curType == CBlock3::TYPE::TORCH_ON)
						{
							
							quad.v1 = { fx + (9 / 16.f),    fy + (10 / 16.f), fz + (8 / 16.f) }; // 우상
							quad.v2 = { fx + (7 / 16.f),    fy + (10 / 16.f), fz + (8 / 16.f) }; // 좌상
							quad.v3 = { fx + (7 / 16.f),    fy,               fz + (8 / 16.f) }; // 좌하
							quad.v4 = { fx + (9 / 16.f),    fy,               fz + (8 / 16.f) }; // 우하

							quad.uv1 = _float2(9.0f / 16.f, 6.0f / 16.f); 
							quad.uv2 = _float2(7.0f / 16.f, 6.0f / 16.f);
							quad.uv3 = _float2(7.0f / 16.f, 16.0f / 16.f); 
							quad.uv4 = _float2(9.0f / 16.f, 16.0f / 16.f);

						}
						else
						{
							quad.v1 = { fx + 1, fy + 1, fz + 1 };
							quad.v2 = { fx,     fy + 1, fz + 1 };
							quad.v3 = { fx,     fy,     fz + 1 };
							quad.v4 = { fx + 1, fy,     fz + 1 };
						}

						quad.eDir = FACE_DIR::POS_Z;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
						quad.lighting = (z + 1 < (int)VOXEL_CHUNK_Z_SIZE3) ? m_arrBlocks[BlockIndexing(x, y, z + 1)].GetLight() : (pPlusZ ? pPlusZ->m_arrBlocks[pPlusZ->BlockIndexing(x, y, 0)].GetLight() : 0xFF);

						_bool bL = IsInsideOpaque(x - 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bU = IsInsideOpaque(x, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bD = IsInsideOpaque(x, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUL = IsInsideOpaque(x - 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUR = IsInsideOpaque(x + 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDL = IsInsideOpaque(x - 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDR = IsInsideOpaque(x + 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bR, bU, bUR);
						quad.ao[1] = CalculateVertexAO(bL, bU, bUL);
						quad.ao[2] = CalculateVertexAO(bL, bD, bDL);
						quad.ao[3] = CalculateVertexAO(bR, bD, bDR);


						quad.color[0] = CBlock3::GetBaseColor(curType);
						quad.color[1] = CBlock3::GetBaseColor(curType);
						quad.color[2] = CBlock3::GetBaseColor(curType);
						quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 4. Back (-Z)
				{
					int nz = z - 1;
					CBlock3::TYPE nextType = GetBlockTypeAt(x, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);

					bool bExpose = false;
					if (isCurrentWater || isCurrAlphaTest)
						bExpose = (nextType == CBlock3::TYPE::AIR);
					else
						bExpose = (nextType == CBlock3::TYPE::AIR || nextType == CBlock3::TYPE::WATER_PLACE_HOLDER);

					if (nextType == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(nextType))
					{
						bExpose = true;
					}
					if (curType == CBlock3::TYPE::TORCH_ON)
					{
						bExpose = true;
					}

					if (bExpose) {
						VOX_QUAD quad{};

						if (curType == CBlock3::TYPE::TORCH_ON)
						{							
							// Z축 고정 두께: 6/16.f, 높이는 10/16.f까지만
							quad.v1 = { fx + (7 / 16.f),    fy + (10 / 16.f), fz + (6 / 16.f) }; // 우상
							quad.v2 = { fx + (9 / 16.f),    fy + (10 / 16.f), fz + (6 / 16.f) }; // 좌상
							quad.v3 = { fx + (9 / 16.f),    fy,               fz + (6 / 16.f) }; // 좌하
							quad.v4 = { fx + (7 / 16.f),    fy,               fz + (6 / 16.f) }; // 우하

							quad.uv1 = _float2(7.0f / 16.f, 6.0f / 16.f);
							quad.uv2 = _float2(9.0f / 16.f, 6.0f / 16.f);
							quad.uv3 = _float2(9.0f / 16.f, 16.0f / 16.f);
							quad.uv4 = _float2(7.0f / 16.f, 16.0f / 16.f);
						}
						else
						{
							quad.v1 = { fx,     fy + 1, fz };
							quad.v2 = { fx + 1, fy + 1, fz };
							quad.v3 = { fx + 1, fy,     fz };
							quad.v4 = { fx,     fy,     fz };
						}
						
						quad.eDir = FACE_DIR::NEG_Z;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
						quad.lighting = (z - 1 >= 0) ? m_arrBlocks[BlockIndexing(x, y, z - 1)].GetLight() : (pMinusZ ? pMinusZ->m_arrBlocks[pMinusZ->BlockIndexing(x, y, (int)VOXEL_CHUNK_Z_SIZE3 - 1)].GetLight() : 0xFF);

						_bool bL = IsInsideOpaque(x - 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bU = IsInsideOpaque(x, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bD = IsInsideOpaque(x, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUL = IsInsideOpaque(x - 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUR = IsInsideOpaque(x + 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDL = IsInsideOpaque(x - 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDR = IsInsideOpaque(x + 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bL, bU, bUL);
						quad.ao[1] = CalculateVertexAO(bR, bU, bUR);
						quad.ao[2] = CalculateVertexAO(bR, bD, bDR);
						quad.ao[3] = CalculateVertexAO(bL, bD, bDL);


						quad.color[0] = CBlock3::GetBaseColor(curType);
						quad.color[1] = CBlock3::GetBaseColor(curType);
						quad.color[2] = CBlock3::GetBaseColor(curType);
						quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 5. Right (+X)
				{
					int nx = x + 1;
					CBlock3::TYPE nextType = GetBlockTypeAt(nx, y, z, pPlusX, pMinusX, pPlusZ, pMinusZ);

					bool bExpose = false;
					if (isCurrentWater || isCurrAlphaTest)
						bExpose = (nextType == CBlock3::TYPE::AIR);
					else
						bExpose = (nextType == CBlock3::TYPE::AIR || nextType == CBlock3::TYPE::WATER_PLACE_HOLDER);

					if (nextType == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(nextType))
					{
						bExpose = true;
					}
					if (curType == CBlock3::TYPE::TORCH_ON)
					{
						bExpose = true;
					}
					if (bExpose) {
						VOX_QUAD quad{};

						if (curType == CBlock3::TYPE::TORCH_ON)
						{
							quad.v1 = { fx + (9 / 16.f),    fy + (10 / 16.f), fz + (6 / 16.f) }; // 우상
							quad.v2 = { fx + (9 / 16.f),    fy + (10 / 16.f), fz + (8 / 16.f) }; // 좌상
							quad.v3 = { fx + (9 / 16.f),    fy,               fz + (8 / 16.f) }; // 좌하
							quad.v4 = { fx + (9 / 16.f),    fy,               fz + (6 / 16.f) }; // 우하

							quad.uv1 = _float2(7.0f / 16.f, 6.0f / 16.f); 
							quad.uv2 = _float2(9.0f / 16.f, 6.0f / 16.f);
							quad.uv3 = _float2(9.0f / 16.f, 16.0f / 16.f); 
							quad.uv4 = _float2(7.0f / 16.f, 16.0f / 16.f);
						}
						else
						{
							quad.v1 = { fx + 1, fy + 1, fz };
							quad.v2 = { fx + 1, fy + 1, fz + 1 };
							quad.v3 = { fx + 1, fy,     fz + 1 };
							quad.v4 = { fx + 1, fy,     fz };
						}
						
						quad.eDir = FACE_DIR::POS_X;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(m_arrBlocks[currentIdx].GetType(), quad.eDir));
						quad.lighting = (x + 1 < (int)VOXEL_CHUNK_X_SIZE3) ? m_arrBlocks[BlockIndexing(x + 1, y, z)].GetLight() : (pPlusX ? pPlusX->m_arrBlocks[pPlusX->BlockIndexing(0, y, z)].GetLight() : 0xFF);

						_bool bF = IsInsideOpaque(nx, y, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bB = IsInsideOpaque(nx, y, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bU = IsInsideOpaque(nx, y + 1, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bD = IsInsideOpaque(nx, y - 1, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUF = IsInsideOpaque(nx, y + 1, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUB = IsInsideOpaque(nx, y + 1, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDF = IsInsideOpaque(nx, y - 1, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDB = IsInsideOpaque(nx, y - 1, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bB, bU, bUB);
						quad.ao[1] = CalculateVertexAO(bF, bU, bUF);
						quad.ao[2] = CalculateVertexAO(bF, bD, bDF);
						quad.ao[3] = CalculateVertexAO(bB, bD, bDB);


						quad.color[0] = CBlock3::GetBaseColor(curType);
						quad.color[1] = CBlock3::GetBaseColor(curType);
						quad.color[2] = CBlock3::GetBaseColor(curType);
						quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}

				// 6. Left (-X)
				{
					int nx = x - 1;
					CBlock3::TYPE nextType = GetBlockTypeAt(nx, y, z, pPlusX, pMinusX, pPlusZ, pMinusZ);

					bool bExpose = false;
					if (isCurrentWater || isCurrAlphaTest)
						bExpose = (nextType == CBlock3::TYPE::AIR);
					else
						bExpose = (nextType == CBlock3::TYPE::AIR || nextType == CBlock3::TYPE::WATER_PLACE_HOLDER);

					if (nextType == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(nextType))
					{
						bExpose = true;
					}
					if (curType == CBlock3::TYPE::TORCH_ON)
					{
						bExpose = true;
					}
					if (bExpose) {
						VOX_QUAD quad{};

						if (curType == CBlock3::TYPE::TORCH_ON)
						{
							quad.v1 = { fx + (7 / 16.f),    fy + (10 / 16.f), fz + (8 / 16.f) }; 
							quad.v2 = { fx + (7 / 16.f),    fy + (10 / 16.f), fz + (6 / 16.f) };
							quad.v3 = { fx + (7 / 16.f),    fy,               fz + (6 / 16.f) };
							quad.v4 = { fx + (7 / 16.f),    fy,               fz + (8 / 16.f) }; 

							quad.uv1 = _float2(9.0f / 16.f, 6.0f / 16.f); 
							quad.uv2 = _float2(7.0f / 16.f, 6.0f / 16.f);
							quad.uv3 = _float2(7.0f / 16.f, 16.0f / 16.f);
							quad.uv4 = _float2(9.0f / 16.f, 16.0f / 16.f); 


						}
						else
						{
							quad.v1 = { fx, fy + 1, fz + 1 };
							quad.v2 = { fx, fy + 1, fz };
							quad.v3 = { fx, fy,     fz };
							quad.v4 = { fx, fy,     fz + 1 };
						}
						
						quad.eDir = FACE_DIR::NEG_X;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(m_arrBlocks[currentIdx].GetType(), quad.eDir));
						quad.lighting = (x - 1 >= 0) ? m_arrBlocks[BlockIndexing(x - 1, y, z)].GetLight() : (pMinusX ? pMinusX->m_arrBlocks[pMinusX->BlockIndexing((int)VOXEL_CHUNK_X_SIZE3 - 1, y, z)].GetLight() : 0xFF);

						_bool bF = IsInsideOpaque(nx, y, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bB = IsInsideOpaque(nx, y, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bU = IsInsideOpaque(nx, y + 1, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bD = IsInsideOpaque(nx, y - 1, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUF = IsInsideOpaque(nx, y + 1, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUB = IsInsideOpaque(nx, y + 1, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDF = IsInsideOpaque(nx, y - 1, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDB = IsInsideOpaque(nx, y - 1, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bF, bU, bUF);
						quad.ao[1] = CalculateVertexAO(bB, bU, bUB);
						quad.ao[2] = CalculateVertexAO(bB, bD, bDB);
						quad.ao[3] = CalculateVertexAO(bF, bD, bDF);


						quad.color[0] = CBlock3::GetBaseColor(curType);
						quad.color[1] = CBlock3::GetBaseColor(curType);
						quad.color[2] = CBlock3::GetBaseColor(curType);
						quad.color[3] = CBlock3::GetBaseColor(curType);

						if (isCurrentWater) waterQuads.push_back(quad);
						else if (isCurrAlphaTest) alphaTestQuads.push_back(quad);
						else                solidQuads.push_back(quad);
					}
				}
			}
		}
	}
}

_bool CChunk3::IsInsideOpaque(int32_t x, int32_t y, int32_t z, CChunk3* pPX, CChunk3* pMX, CChunk3* pPZ, CChunk3* pMZ) const
{
	// Y축 경계 처리
	if (y < 0 || y >= (int32_t)VOXEL_CHUNK_Y_SIZE3) return false;

	// X, Z 축 범위에 따른 청크 포인터 분기
	if (x >= 0 && x < (int32_t)VOXEL_CHUNK_X_SIZE3 && z >= 0 && z < (int32_t)VOXEL_CHUNK_Z_SIZE3)
	{
		return m_arrBlocks[BlockIndexing(x, y, z)].IsOpaque();
	}

	// 타겟 청크 탐색 및 로컬 좌표 변환
	const CChunk3* pTargetChunk = this; // 기본값은 현재 청크
	int32_t localX = x;
	int32_t localZ = z;

	// X축 청크 경계 및 로컬 좌표 처리
	if (x >= (int32_t)VOXEL_CHUNK_X_SIZE3)
	{
		pTargetChunk = pPX;
		localX = x - (int32_t)VOXEL_CHUNK_X_SIZE3;
	}
	else if (x < 0)
	{
		pTargetChunk = pMX;
		localX = x + (int32_t)VOXEL_CHUNK_X_SIZE3;
	}

	// Z축 청크 경계 및 로컬 좌표 처리 (독립적인 if문으로 분리!)
	if (z >= (int32_t)VOXEL_CHUNK_Z_SIZE3)
	{
		// 만약 이미 X축에서 청크가 바뀌었는데 Z축도 벗어났다면 대각선 청크가 필요함.
		// 현재 구조(4방향 인접 청크만 받음)에서는 대각선 참조 시 빈 공간(false) 처리
		if (pTargetChunk != this) return false;

		pTargetChunk = pPZ;
		localZ = z - (int32_t)VOXEL_CHUNK_Z_SIZE3;
	}
	else if (z < 0)
	{
		if (pTargetChunk != this) return false;

		pTargetChunk = pMZ;
		localZ = z + (int32_t)VOXEL_CHUNK_Z_SIZE3;
	}

	// 인접 청크가 로드되지 않았거나, 대각선 경계로 인해 null인 경우 예외 처리
	if (pTargetChunk == nullptr) return false;

	uint32_t cIdx = BlockIndexing(localX, y, localZ);
	return pTargetChunk->m_arrBlocks[cIdx].IsOpaque();
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
		quad.v1 = { fx,     fy + 1.f, fz };
		quad.v2 = { fx + 1.f, fy + 1.f, fz + 1.f };
		quad.v3 = { fx + 1.f, fy,       fz + 1.f };
		quad.v4 = { fx,     fy,       fz };

		quad.uv1 = { 0.f, 0.f }; quad.uv2 = { 1.f, 0.f };
		quad.uv3 = { 1.f, 1.f }; quad.uv4 = { 0.f, 1.f };

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
		quad.v1 = { fx,     fy + 1.f, fz + 1.f };
		quad.v2 = { fx + 1.f, fy + 1.f, fz };
		quad.v3 = { fx + 1.f, fy,       fz };
		quad.v4 = { fx,     fy,       fz + 1.f };

		quad.uv1 = { 0.f, 0.f }; quad.uv2 = { 1.f, 0.f };
		quad.uv3 = { 1.f, 1.f }; quad.uv4 = { 0.f, 1.f };

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
		quad.v1 = { fx + (7 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f) };
		quad.v2 = { fx + (9 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f) };
		quad.v3 = { fx + (9 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f) };
		quad.v4 = { fx + (7 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f) };

		quad.uv1 = { 7.0f / 16.f, 6.0f / 16.f };
		quad.uv2 = { 9.0f / 16.f, 6.0f / 16.f };
		quad.uv3 = { 9.0f / 16.f, 8.0f / 16.f };
		quad.uv4 = { 7.0f / 16.f, 8.0f / 16.f };

		quad.eDir = FACE_DIR::POS_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 0;
		alphaTestQuads.push_back(quad);
	}

	// [Bottom]
	{
		VOX_QUAD quad{};
		quad.v1 = { fx + (7 / 16.f), fy, fz + (6 / 16.f) };
		quad.v2 = { fx + (9 / 16.f), fy, fz + (6 / 16.f) };
		quad.v3 = { fx + (9 / 16.f), fy, fz + (8 / 16.f) };
		quad.v4 = { fx + (7 / 16.f), fy, fz + (8 / 16.f) };

		quad.uv1 = { 7.0f / 16.f, 10.0f / 16.f };
		quad.uv2 = { 9.0f / 16.f, 10.0f / 16.f };
		quad.uv3 = { 9.0f / 16.f, 8.0f / 16.f };
		quad.uv4 = { 7.0f / 16.f, 8.0f / 16.f };

		quad.eDir = FACE_DIR::NEG_Y;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 0;
		alphaTestQuads.push_back(quad);
	}

	// [Front]
	{
		VOX_QUAD quad{};
		quad.v1 = { fx + (9 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f) };
		quad.v2 = { fx + (7 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f) };
		quad.v3 = { fx + (7 / 16.f), fy,               fz + (8 / 16.f) };
		quad.v4 = { fx + (9 / 16.f), fy,               fz + (8 / 16.f) };

		quad.uv1 = { 9.0f / 16.f, 6.0f / 16.f };
		quad.uv2 = { 7.0f / 16.f, 6.0f / 16.f };
		quad.uv3 = { 7.0f / 16.f, 16.0f / 16.f };
		quad.uv4 = { 9.0f / 16.f, 16.0f / 16.f };

		quad.eDir = FACE_DIR::POS_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 0;
		alphaTestQuads.push_back(quad);
	}

	// [Back]
	{
		VOX_QUAD quad{};
		quad.v1 = { fx + (7 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f) };
		quad.v2 = { fx + (9 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f) };
		quad.v3 = { fx + (9 / 16.f), fy,               fz + (6 / 16.f) };
		quad.v4 = { fx + (7 / 16.f), fy,               fz + (6 / 16.f) };

		quad.uv1 = { 7.0f / 16.f, 6.0f / 16.f };
		quad.uv2 = { 9.0f / 16.f, 6.0f / 16.f };
		quad.uv3 = { 9.0f / 16.f, 16.0f / 16.f };
		quad.uv4 = { 7.0f / 16.f, 16.0f / 16.f };

		quad.eDir = FACE_DIR::NEG_Z;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 0;
		alphaTestQuads.push_back(quad);
	}

	// [Right]
	{
		VOX_QUAD quad{};
		quad.v1 = { fx + (9 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f) };
		quad.v2 = { fx + (9 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f) };
		quad.v3 = { fx + (9 / 16.f), fy,               fz + (8 / 16.f) };
		quad.v4 = { fx + (9 / 16.f), fy,               fz + (6 / 16.f) };

		quad.uv1 = { 7.0f / 16.f, 6.0f / 16.f };
		quad.uv2 = { 9.0f / 16.f, 6.0f / 16.f };
		quad.uv3 = { 9.0f / 16.f, 16.0f / 16.f };
		quad.uv4 = { 7.0f / 16.f, 16.0f / 16.f };

		quad.eDir = FACE_DIR::POS_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 0;
		alphaTestQuads.push_back(quad);
	}

	// [Left]
	{
		VOX_QUAD quad{};
		quad.v1 = { fx + (7 / 16.f), fy + (10 / 16.f), fz + (8 / 16.f) };
		quad.v2 = { fx + (7 / 16.f), fy + (10 / 16.f), fz + (6 / 16.f) };
		quad.v3 = { fx + (7 / 16.f), fy,               fz + (6 / 16.f) };
		quad.v4 = { fx + (7 / 16.f), fy,               fz + (8 / 16.f) };

		quad.uv1 = { 9.0f / 16.f, 6.0f / 16.f };
		quad.uv2 = { 7.0f / 16.f, 6.0f / 16.f };
		quad.uv3 = { 7.0f / 16.f, 16.0f / 16.f };
		quad.uv4 = { 9.0f / 16.f, 16.0f / 16.f };

		quad.eDir = FACE_DIR::NEG_X;
		quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(curType, quad.eDir));
		quad.lighting = blockLight;
		quad.ao[0] = quad.ao[1] = quad.ao[2] = quad.ao[3] = 0;
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
		quad.v1 = { fx,        halfY, fz + 1.f };
		quad.v2 = { fx + 1.f,  halfY, fz + 1.f };
		quad.v3 = { fx + 1.f,  halfY, fz };
		quad.v4 = { fx,        halfY, fz };

		quad.uv1 = { 0.f, 0.f };
		quad.uv2 = { 1.f, 0.f };
		quad.uv3 = { 1.f, 1.f };
		quad.uv4 = { 0.f, 1.f };

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
		quad.v1 = { fx,        fy, fz };
		quad.v2 = { fx + 1.f,  fy, fz };
		quad.v3 = { fx + 1.f,  fy, fz + 1.f };
		quad.v4 = { fx,        fy, fz + 1.f };

		quad.uv1 = { 0.f, 0.f };
		quad.uv2 = { 1.f, 0.f };
		quad.uv3 = { 1.f, 1.f };
		quad.uv4 = { 0.f, 1.f };

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
		quad.v1 = { fx + 1.f,  halfY, fz + 1.f };
		quad.v2 = { fx,        halfY, fz + 1.f };
		quad.v3 = { fx,        fy,    fz + 1.f };
		quad.v4 = { fx + 1.f,  fy,    fz + 1.f };

		quad.uv1 = { 1.f, 0.5f }; // 💡 상단 V 좌표를 0.5f로 커팅
		quad.uv2 = { 0.f, 0.5f };
		quad.uv3 = { 0.f, 1.f };
		quad.uv4 = { 1.f, 1.f };

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
		quad.v1 = { fx,        halfY, fz };
		quad.v2 = { fx + 1.f,  halfY, fz };
		quad.v3 = { fx + 1.f,  fy,    fz };
		quad.v4 = { fx,        fy,    fz };

		quad.uv1 = { 1.f, 0.5f };
		quad.uv2 = { 0.f, 0.5f };
		quad.uv3 = { 0.f, 1.f };
		quad.uv4 = { 1.f, 1.f };

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
		quad.v1 = { fx + 1.f,  halfY, fz };
		quad.v2 = { fx + 1.f,  halfY, fz + 1.f };
		quad.v3 = { fx + 1.f,  fy,    fz + 1.f };
		quad.v4 = { fx + 1.f,  fy,    fz };

		quad.uv1 = { 1.f, 0.5f };
		quad.uv2 = { 0.f, 0.5f };
		quad.uv3 = { 0.f, 1.f };
		quad.uv4 = { 1.f, 1.f };

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
		quad.v1 = { fx,        halfY, fz + 1.f };
		quad.v2 = { fx,        halfY, fz };
		quad.v3 = { fx,        fy,    fz };
		quad.v4 = { fx,        fy,    fz + 1.f };

		quad.uv1 = { 1.f, 0.5f };
		quad.uv2 = { 0.f, 0.5f };
		quad.uv3 = { 0.f, 1.f };
		quad.uv4 = { 1.f, 1.f };

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
		quad.v1 = { fx,        halfY, fz + 1.f }; quad.v2 = { fx + 1.f,  halfY, fz + 1.f };
		quad.v3 = { fx + 1.f,  halfY, fz }; quad.v4 = { fx,        halfY, fz };
		quad.uv1 = { 0.f, 0.f }; quad.uv2 = { 1.f, 0.f }; quad.uv3 = { 1.f, 1.f }; quad.uv4 = { 0.f, 1.f };
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
		quad.v1 = { fx,        fy, fz }; quad.v2 = { fx + 1.f,  fy, fz };
		quad.v3 = { fx + 1.f,  fy, fz + 1.f }; quad.v4 = { fx,        fy, fz + 1.f };
		quad.uv1 = { 0.f, 0.f }; quad.uv2 = { 1.f, 0.f }; quad.uv3 = { 1.f, 1.f }; quad.uv4 = { 0.f, 1.f };
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
		quad.v1 = { fx + 1.f,  halfY, fz + 1.f }; quad.v2 = { fx,        halfY, fz + 1.f };
		quad.v3 = { fx,        fy,    fz + 1.f }; quad.v4 = { fx + 1.f,  fy,    fz + 1.f };
		quad.uv1 = { 1.f, 0.5f }; quad.uv2 = { 0.f, 0.5f }; quad.uv3 = { 0.f, 1.f }; quad.uv4 = { 1.f, 1.f };
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
		quad.v1 = { fx,        halfY, fz }; quad.v2 = { fx + 1.f,  halfY, fz };
		quad.v3 = { fx + 1.f,  fy,    fz }; quad.v4 = { fx,        fy,    fz };
		quad.uv1 = { 1.f, 0.5f }; quad.uv2 = { 0.f, 0.5f }; quad.uv3 = { 0.f, 1.f }; quad.uv4 = { 1.f, 1.f };
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
		quad.v1 = { fx + 1.f,  halfY, fz }; quad.v2 = { fx + 1.f,  halfY, fz + 1.f };
		quad.v3 = { fx + 1.f,  fy,    fz + 1.f }; quad.v4 = { fx + 1.f,  fy,    fz };
		quad.uv1 = { 1.f, 0.5f }; quad.uv2 = { 0.f, 0.5f }; quad.uv3 = { 0.f, 1.f }; quad.uv4 = { 1.f, 1.f };
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
		quad.v1 = { fx,        halfY, fz + 1.f }; quad.v2 = { fx,        halfY, fz };
		quad.v3 = { fx,        fy,    fz }; quad.v4 = { fx,        fy,    fz + 1.f };
		quad.uv1 = { 1.f, 0.5f }; quad.uv2 = { 0.f, 0.5f }; quad.uv3 = { 0.f, 1.f }; quad.uv4 = { 1.f, 1.f };
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
		quad.v1 = { startX,     maxY,  endZ }; quad.v2 = { endX,       maxY,  endZ };
		quad.v3 = { endX,       maxY,  startZ }; quad.v4 = { startX,     maxY,  startZ };
		quad.uv1 = { 0.f, 0.f }; quad.uv2 = { 1.f, 0.f }; quad.uv3 = { 1.f, 1.f }; quad.uv4 = { 0.f, 1.f };
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
		quad.v1 = { endX,       maxY,  endZ };   quad.v2 = { startX,     maxY,  endZ };
		quad.v3 = { startX,     halfY, endZ };   quad.v4 = { endX,       halfY, endZ };
		quad.uv1 = { 1.f, 0.f }; quad.uv2 = { 0.f, 0.f }; quad.uv3 = { 0.f, 0.5f }; quad.uv4 = { 1.f, 0.5f };
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
		quad.v1 = { startX,     maxY,  startZ }; quad.v2 = { endX,       maxY,  startZ };
		quad.v3 = { endX,       halfY, startZ }; quad.v4 = { startX,     halfY, startZ };
		quad.uv1 = { 1.f, 0.f }; quad.uv2 = { 0.f, 0.f }; quad.uv3 = { 0.f, 0.5f }; quad.uv4 = { 1.f, 0.5f };
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
		quad.v1 = { endX,       maxY,  startZ }; quad.v2 = { endX,       maxY,  endZ };
		quad.v3 = { endX,       halfY, endZ };   quad.v4 = { endX,       halfY, startZ };
		quad.uv1 = { 1.f, 0.f }; quad.uv2 = { 0.f, 0.f }; quad.uv3 = { 0.f, 0.5f }; quad.uv4 = { 1.f, 0.5f };
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
		quad.v1 = { startX,     maxY,  endZ };   quad.v2 = { startX,     maxY,  startZ };
		quad.v3 = { startX,     halfY, startZ }; quad.v4 = { startX,     halfY, endZ };
		quad.uv1 = { 1.f, 0.f }; quad.uv2 = { 0.f, 0.f }; quad.uv3 = { 0.f, 0.5f }; quad.uv4 = { 1.f, 0.5f };
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

UPtr<CChunk3> CChunk3::Create(const DESC& desc)
{
	auto pInstnace = ToUPtr(new CChunk3{});
	if (FAILED(pInstnace->Initialize(desc)))
	{
		return nullptr;
	}
	return pInstnace;
}