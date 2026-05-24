#include "pch.h"
#include "Chunk3.h"

NS_USING(Engine)

HRESULT CChunk3::InitialChunkLighting()
{
	std::queue<XMINT3> floodFillSkyLightQ{};
	std::queue<XMINT3> floodFillBlockLightQ{};

	{
		for (int32_t i = 0; i < VOXEL_CHUNK_X_SIZE3; ++i)
		{
			for (int32_t k = 0; k < VOXEL_CHUNK_Z_SIZE3; ++k)
			{
				_bool bBlocked{ false };
				bool bSkySeeded = false;
				for (int32_t j = VOXEL_CHUNK_Y_SIZE3 - 1; j >= 0; --j)
				{
					uint32_t idx =
						CChunk3::BlockIndexing(i, j, k);

					CBlock3 block = GetBlock(idx);

					//
					// SKY LIGHT
					//

					if (block.IsOpaque())
					{
						bBlocked = true;
						block.SetSkyLight(0);
					}
					else
					{
						if (!bBlocked)
						{
							block.SetSkyLight(15);

							if (!bSkySeeded)
							{
								floodFillSkyLightQ.push({ i,j,k });
								bSkySeeded = true;
							}
						}
						else
						{
							block.SetSkyLight(0);
						}
					}

					//
					// BLOCK LIGHT
					//

					uint8_t light =
						CBlock3::GetBlockLightByType(block.GetType());

					block.SetBlockLight(light);

					if (light > 0)
					{
						floodFillBlockLightQ.push({
							i,
							j,
							k
							});
					}

					SetBlock(idx, block);

				}

			}
		}
	}

	InitialChunkFloodFillSkyLighting(floodFillSkyLightQ);
	InitialChunkFloodFillBlockLighting(floodFillBlockLightQ);

	return S_OK;
}

void CChunk3::InitialChunkFloodFillSkyLighting(std::queue<XMINT3>& q)
{
	constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
	constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
	constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };

	while (!q.empty())
	{
		auto [cbx, cby, cbz] = q.front(); q.pop();

		CBlock3& block = GetBlock(cbx, cby, cbz);

		uint8_t curLight = block.GetSkyLight();
		if (curLight == 0) continue;


		for (int d = 0; d < 6; ++d)
		{
			int nx = cbx + dx[d];
			int ny = cby + dy[d];
			int nz = cbz + dz[d];

			if (nx < 0 || nx >= VOXEL_CHUNK_X_SIZE3)
				continue;
			if (ny < 0 || ny >= VOXEL_CHUNK_Y_SIZE3)
				continue;
			if (nz < 0 || nz >= VOXEL_CHUNK_Z_SIZE3)
				continue;

			CBlock3& nBlock = GetBlock(nx, ny, nz);
			if (nBlock.IsOpaque()) continue;

			uint8_t nSkyLight = nBlock.GetSkyLight();

			uint8_t newLight{};

			bool goingDown = (d == 3);
			if (!goingDown && curLight <= 1) continue;

			if (goingDown)
			{
				newLight = curLight;
			}
			else
			{
				newLight = curLight - 1;
			}

			if (newLight > nSkyLight)
			{
				nBlock.SetSkyLight(newLight);
				q.push({ nx, ny, nz });
			}
		}
	}
}

void CChunk3::InitialChunkFloodFillBlockLighting(std::queue<XMINT3>& q)
{
	constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
	constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
	constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };

	while (!q.empty())
	{
		auto [cbx, cby, cbz] = q.front(); q.pop();

		CBlock3& block = GetBlock(cbx, cby, cbz);

		uint8_t curLight = block.GetBlockLight();

		if (curLight <= 1)
			continue;

		for (int d = 0; d < 6; ++d)
		{
			int nx = cbx + dx[d];
			int ny = cby + dy[d];
			int nz = cbz + dz[d];

			if (nx < 0 || nx >= VOXEL_CHUNK_X_SIZE3)
				continue;
			if (ny < 0 || ny >= VOXEL_CHUNK_Y_SIZE3)
				continue;
			if (nz < 0 || nz >= VOXEL_CHUNK_Z_SIZE3)
				continue;

			CBlock3& nBlock = GetBlock(nx, ny, nz);
			if (nBlock.IsOpaque()) continue;

			if (nBlock.IsOpaque())
				continue;

			uint8_t newLight = curLight - 1;

			if (newLight > nBlock.GetBlockLight())
			{
				nBlock.SetBlockLight(newLight);
				q.push({ nx, ny, nz });
			}
		}
	}
}

HRESULT CChunk3::BlockFilling()
{
	m_eBlockFillingState = BLOCKFILLING_STATE::ING;

	constexpr float NOISE_OFFSET = 100000.f;
	constexpr int   BEDROCK_MAX_HEIGHT = 4;

	for (int32_t i = 0; i < VOXEL_CHUNK_X_SIZE3; ++i)
	{
		for (int32_t j = 0; j < VOXEL_CHUNK_Z_SIZE3; ++j)
		{
			float tmpx = m_iX * (int32_t)VOXEL_CHUNK_X_SIZE3 + i + NOISE_OFFSET;
			float tmpz = m_iZ * (int32_t)VOXEL_CHUNK_Z_SIZE3 + j + NOISE_OFFSET;

			float n = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::HEIGHT).GetNoise(tmpx, tmpz);
			float t = (n + 1.0f) * 0.5f;
			uint32_t height = (uint32_t)(32.0f + t * 64.0f);

			for (uint32_t k = 0; k < VOXEL_CHUNK_Y_SIZE3; ++k)
			{
				uint32_t idx = BlockIndexing(i, k, j);

				if (k == 0)
				{
					m_arrBlocks[idx].SetType(CBlock3::TYPE::BEDROCK);
				}
				else if (k <= BEDROCK_MAX_HEIGHT)
				{
					float threshold = 1.f - ((float)k / BEDROCK_MAX_HEIGHT);
					float noise = (CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::BEDROCK).GetNoise(
						tmpx, (float)k, tmpz) + 1.f) * 0.5f;

					m_arrBlocks[idx].SetType(noise < threshold
						? CBlock3::TYPE::BEDROCK
						: CBlock3::TYPE::STONE);   // AIR 대신 STONE — 베드락 위는 돌
				}
				else if (k < height - 10)
				{
					m_arrBlocks[idx].SetType(CBlock3::TYPE::STONE);
				}
				else if (k < height)
				{
					m_arrBlocks[idx].SetType(CBlock3::TYPE::DIRT);
				}
				else if (k == height)
				{
					m_arrBlocks[idx].SetType(CBlock3::TYPE::GRASS);
				}
				else
				{
					m_arrBlocks[idx].SetType(CBlock3::TYPE::AIR);
				}
			}
		}
	}

	InitialChunkLighting();

	m_eBlockFillingState = BLOCKFILLING_STATE::DONE;
	return S_OK;
}

HRESULT CChunk3::QuadMessing()
{
	m_eMessingState = MESSING_STATE::ING;

	std::vector<VOX_QUAD> quads{};
	NiveFaceCulling(quads);

	m_indices.clear();
	m_vertices.clear();

	m_vertices.reserve(quads.size() * 4);
	m_indices.reserve(quads.size() * 6);

	for (uint32_t i = 0; i < quads.size(); ++i)
	{
		uint32_t iFaceDir = ETOUI(quads[i].eDir);

		// 4개 정점 데이터 준비
		E::VTX_VOXEL v[4]{};
		v[0].pos = quads[i].v1;
		v[1].pos = quads[i].v2;
		v[2].pos = quads[i].v3;
		v[3].pos = quads[i].v4;

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

		m_vertices.push_back(v[0]);
		m_vertices.push_back(v[1]);
		m_vertices.push_back(v[2]);
		m_vertices.push_back(v[3]);

		uint32_t startIndex = i * 4;

		// 비대칭 이방성(Anisotropy) 보정 처리
		// 0번-2번 대각선의 AO 합산이 1번-3번보다 작다면(즉, 더 어둡다면) 삼각형 쪼개기 방향을 회전시킵니다.
		if (quads[i].ao[0] + quads[i].ao[2] < quads[i].ao[1] + quads[i].ao[3])
		{
			// 1-2-3, 1-3-0 형태로 인덱스 배치
			m_indices.push_back(startIndex + 1);
			m_indices.push_back(startIndex + 2);
			m_indices.push_back(startIndex + 3);
			m_indices.push_back(startIndex + 1);
			m_indices.push_back(startIndex + 3);
			m_indices.push_back(startIndex + 0);
		}
		else
		{
			// 기존 순서: 0-1-2, 0-2-3
			m_indices.push_back(startIndex + 0);
			m_indices.push_back(startIndex + 1);
			m_indices.push_back(startIndex + 2);
			m_indices.push_back(startIndex + 0);
			m_indices.push_back(startIndex + 2);
			m_indices.push_back(startIndex + 3);
		}
	}
	m_eMessingState = MESSING_STATE::DONE;
	return S_OK;
}

HRESULT CChunk3::CreateBuffer()
{
	m_eBufferState = BUFFER_STATE::ING;

	E::CResDynamicVIBuffer::DESC desc{};
	desc.iNumVertices = (uint32_t)m_vertices.size();
	desc.iVertexStride = sizeof(E::VTX_VOXEL);
	desc.vertexDesc = {
		.ByteWidth = desc.iNumVertices * desc.iVertexStride,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};
	desc.vertexSubResource = {
		.pSysMem = m_vertices.data()
	};

	desc.iIndexStride = sizeof(uint32_t);
	desc.iNumIndices = (uint32_t)m_indices.size();
	desc.IndexDesc = {
		.ByteWidth = desc.iNumIndices * desc.iIndexStride,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};
	desc.indexSubResource = {
		.pSysMem = m_indices.data()
	};
	desc.eIndexFormat = DXGI_FORMAT_R32_UINT;
	desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pResDynamicViBuffer = E::CResDynamicVIBuffer::Create();
	if (FAILED(m_pResDynamicViBuffer->Load(desc)))
	{
		m_pResDynamicViBuffer.reset();
		m_eBufferState = BUFFER_STATE::NON;
		return E_FAIL;
	};

	m_eBufferState = BUFFER_STATE::DONE;

	m_indices.clear();
	m_vertices.clear();
	m_eMessingState = MESSING_STATE::NON;

	return S_OK;
}

HRESULT CChunk3::Draw(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const
{
	if (m_eBufferState != BUFFER_STATE::DONE)
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

	const auto& viBuffer = m_pResDynamicViBuffer;

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

void CChunk3::NiveFaceCulling(std::vector<VOX_QUAD>& quads) const
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
				if (m_arrBlocks[currentIdx].GetType() == CBlock3::TYPE::AIR) continue;

				float fx = (float)x;
				float fy = (float)y;
				float fz = (float)z;

				// 1. Top (+Y)
				{
					int ny = y + 1;
					bool bExpose = (ny >= (int)VOXEL_CHUNK_Y_SIZE3) ? true : !m_arrBlocks[BlockIndexing(x, ny, z)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx,     fy + 1, fz + 1 };
						quad.v2 = { fx + 1, fy + 1, fz + 1 };
						quad.v3 = { fx + 1, fy + 1, fz };
						quad.v4 = { fx,     fy + 1, fz };
						quad.eDir = FACE_DIR::POS_Y;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(m_arrBlocks[currentIdx].GetType(), quad.eDir));
						quad.lighting = (y + 1 < (int)VOXEL_CHUNK_Y_SIZE3) ? m_arrBlocks[BlockIndexing(x, y + 1, z)].GetLight() : 0xFF;

						// AO 대상 기준 평면: Y + 1 레이어의 주변 8개 블록 조회
						_bool bL = IsInsideOpaque(x - 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bF = IsInsideOpaque(x, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bB = IsInsideOpaque(x, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBL = IsInsideOpaque(x - 1, ny, fz - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBR = IsInsideOpaque(x + 1, ny, fz - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFL = IsInsideOpaque(x - 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFR = IsInsideOpaque(x + 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bL, bF, bFL); // v1 (Left, Front)
						quad.ao[1] = CalculateVertexAO(bR, bF, bFR); // v2 (Right, Front)
						quad.ao[2] = CalculateVertexAO(bR, bB, bBR); // v3 (Right, Back)
						quad.ao[3] = CalculateVertexAO(bL, bB, bBL); // v4 (Left, Back)

						quads.push_back(quad);
					}
				}

				// 2. Bottom (-Y)
				{
					int ny = y - 1;
					bool bExpose = (ny < 0) ? true : !m_arrBlocks[BlockIndexing(x, ny, z)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx,     fy, fz };
						quad.v2 = { fx + 1, fy, fz };
						quad.v3 = { fx + 1, fy, fz + 1 };
						quad.v4 = { fx,     fy, fz + 1 };
						quad.eDir = FACE_DIR::NEG_Y;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(m_arrBlocks[currentIdx].GetType(), quad.eDir));
						quad.lighting = (y - 1 >= 0) ? m_arrBlocks[BlockIndexing(x, y - 1, z)].GetLight() : 0;

						_bool bL = IsInsideOpaque(x - 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, ny, z, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bF = IsInsideOpaque(x, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bB = IsInsideOpaque(x, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBL = IsInsideOpaque(x - 1, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bBR = IsInsideOpaque(x + 1, ny, z - 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFL = IsInsideOpaque(x - 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bFR = IsInsideOpaque(x + 1, ny, z + 1, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bL, bB, bBL); // v1
						quad.ao[1] = CalculateVertexAO(bR, bB, bBR); // v2
						quad.ao[2] = CalculateVertexAO(bR, bF, bFR); // v3
						quad.ao[3] = CalculateVertexAO(bL, bF, bFL); // v4

						quads.push_back(quad);
					}
				}

				// 3. Front (+Z)
				{
					int nz = z + 1;
					bool bExpose = (nz >= (int)VOXEL_CHUNK_Z_SIZE3) ? (pPlusZ ? !pPlusZ->m_arrBlocks[pPlusZ->BlockIndexing(x, y, 0)].IsOpaque() : true) : !m_arrBlocks[BlockIndexing(x, y, nz)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx + 1, fy + 1, fz + 1 };
						quad.v2 = { fx,     fy + 1, fz + 1 };
						quad.v3 = { fx,     fy,     fz + 1 };
						quad.v4 = { fx + 1, fy,     fz + 1 };
						quad.eDir = FACE_DIR::POS_Z;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(m_arrBlocks[currentIdx].GetType(), quad.eDir));
						quad.lighting = (z + 1 < (int)VOXEL_CHUNK_Z_SIZE3) ? m_arrBlocks[BlockIndexing(x, y, z + 1)].GetLight() : (pPlusZ ? pPlusZ->m_arrBlocks[pPlusZ->BlockIndexing(x, y, 0)].GetLight() : 0xFF);

						_bool bL = IsInsideOpaque(x - 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bU = IsInsideOpaque(x, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bD = IsInsideOpaque(x, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUL = IsInsideOpaque(x - 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUR = IsInsideOpaque(x + 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDL = IsInsideOpaque(x - 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDR = IsInsideOpaque(x + 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bR, bU, bUR); // v1
						quad.ao[1] = CalculateVertexAO(bL, bU, bUL); // v2
						quad.ao[2] = CalculateVertexAO(bL, bD, bDL); // v3
						quad.ao[3] = CalculateVertexAO(bR, bD, bDR); // v4

						quads.push_back(quad);
					}
				}

				// 4. Back (-Z)
				{
					int nz = z - 1;
					bool bExpose = (nz < 0) ? (pMinusZ ? !pMinusZ->m_arrBlocks[pMinusZ->BlockIndexing(x, y, (int)VOXEL_CHUNK_Z_SIZE3 - 1)].IsOpaque() : true) : !m_arrBlocks[BlockIndexing(x, y, nz)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx,     fy + 1, fz };
						quad.v2 = { fx + 1, fy + 1, fz };
						quad.v3 = { fx + 1, fy,     fz };
						quad.v4 = { fx,     fy,     fz };
						quad.eDir = FACE_DIR::NEG_Z;
						quad.blockTexType = static_cast<uint8_t>(CBlock3::GetTexType(m_arrBlocks[currentIdx].GetType(), quad.eDir));
						quad.lighting = (z - 1 >= 0) ? m_arrBlocks[BlockIndexing(x, y, z - 1)].GetLight() : (pMinusZ ? pMinusZ->m_arrBlocks[pMinusZ->BlockIndexing(x, y, (int)VOXEL_CHUNK_Z_SIZE3 - 1)].GetLight() : 0xFF);

						_bool bL = IsInsideOpaque(x - 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bR = IsInsideOpaque(x + 1, y, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bU = IsInsideOpaque(x, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bD = IsInsideOpaque(x, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUL = IsInsideOpaque(x - 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bUR = IsInsideOpaque(x + 1, y + 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDL = IsInsideOpaque(x - 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);
						_bool bDR = IsInsideOpaque(x + 1, y - 1, nz, pPlusX, pMinusX, pPlusZ, pMinusZ);

						quad.ao[0] = CalculateVertexAO(bL, bU, bUL); // v1
						quad.ao[1] = CalculateVertexAO(bR, bU, bUR); // v2
						quad.ao[2] = CalculateVertexAO(bR, bD, bDR); // v3
						quad.ao[3] = CalculateVertexAO(bL, bD, bDL); // v4

						quads.push_back(quad);
					}
				}

				// 5. Right (+X)
				{
					int nx = x + 1;
					bool bExpose = (nx >= (int)VOXEL_CHUNK_X_SIZE3) ? (pPlusX ? !pPlusX->m_arrBlocks[pPlusX->BlockIndexing(0, y, z)].IsOpaque() : true) : !m_arrBlocks[BlockIndexing(nx, y, z)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx + 1, fy + 1, fz };
						quad.v2 = { fx + 1, fy + 1, fz + 1 };
						quad.v3 = { fx + 1, fy,     fz + 1 };
						quad.v4 = { fx + 1, fy,     fz };
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

						quad.ao[0] = CalculateVertexAO(bB, bU, bUB); // v1
						quad.ao[1] = CalculateVertexAO(bF, bU, bUF); // v2
						quad.ao[2] = CalculateVertexAO(bF, bD, bDF); // v3
						quad.ao[3] = CalculateVertexAO(bB, bD, bDB); // v4

						quads.push_back(quad);
					}
				}

				// 6. Left (-X)
				{
					int nx = x - 1;
					bool bExpose = (nx < 0) ? (pMinusX ? !pMinusX->m_arrBlocks[pMinusX->BlockIndexing((int)VOXEL_CHUNK_X_SIZE3 - 1, y, z)].IsOpaque() : true) : !m_arrBlocks[BlockIndexing(nx, y, z)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx, fy + 1, fz + 1 };
						quad.v2 = { fx, fy + 1, fz };
						quad.v3 = { fx, fy,     fz };
						quad.v4 = { fx, fy,     fz + 1 };
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

						quad.ao[0] = CalculateVertexAO(bF, bU, bUF); // v1
						quad.ao[1] = CalculateVertexAO(bB, bU, bUB); // v2
						quad.ao[2] = CalculateVertexAO(bB, bD, bDB); // v3
						quad.ao[3] = CalculateVertexAO(bF, bD, bDF); // v4

						quads.push_back(quad);
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