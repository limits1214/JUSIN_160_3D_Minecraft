#include "Chunk.h"
#include "Resources.h"
#include "GameInstance.h"

NS_USING(Engine)


CChunk::CChunk()
{

}

CChunk::~CChunk()
{
}

std::vector<VOX_QUAD> CChunk::GenerateQuad()
{
	std::vector<VOX_QUAD> quads{};
	
	FaceCulling(quads);

    return quads;
}

void CChunk::BlockGenerate()
{
	// 블록 데이터 초기화 (Height Map 적용)
	for (uint32_t i = 0; i < VOXEL_CHUNK_X_SIZE; ++i)
	{
		for (uint32_t j = 0; j < VOXEL_CHUNK_Z_SIZE; ++j)
		{
			auto tmpx = m_iX * VOXEL_CHUNK_X_SIZE + i;
			auto tmpz = m_iZ * VOXEL_CHUNK_Z_SIZE + j;
			float n = CGameInstance::Get().GetVoxelHeightNoise((float)tmpx, (float)tmpz);
			float t = (n + 1.0f) * 0.5f;   // 0~1
			uint32_t height = (uint32_t)(32.0f + t * 64.0f); // 64~128
			for (uint32_t k = 0; k < VOXEL_CHUNK_Y_SIZE; ++k)
			{
				uint32_t idx = BlockIndexing(i, k, j);
				if (k < height)
					m_arrBlocks[idx].SetType(Block::TYPE::GRASS);
				else
					m_arrBlocks[idx].SetType(Block::TYPE::AIR);
			}
		}
	}
}

void CChunk::FaceCulling(std::vector<VOX_QUAD>& quads)
{
	//CChunk* pPlusXAdjChunk = CGameInstance::Get().GetVoxelChunk(m_iX + 1, m_iZ);
	//CChunk* pMinusXAdjChunk = CGameInstance::Get().GetVoxelChunk(m_iX - 1, m_iZ);
	//CChunk* pPlusZAdjChunk = CGameInstance::Get().GetVoxelChunk(m_iX , m_iZ + 1);
	//CChunk* pMinusZAdjChunk = CGameInstance::Get().GetVoxelChunk(m_iX, m_iZ - 1);

	//CGameInstance::Get().GetVoxelChunk()
	// Culled Meshing (JS의 p != b 로직 적용)
	for (int x = 0; x < (int)VOXEL_CHUNK_X_SIZE; ++x)
	{
		for (int z = 0; z < (int)VOXEL_CHUNK_Z_SIZE; ++z)
		{
			for (int y = 0; y < (int)VOXEL_CHUNK_Y_SIZE; ++y)
			{
				uint32_t currentIdx = BlockIndexing(x, y, z);
				if (m_arrBlocks[currentIdx].GetType() == Block::TYPE::AIR) continue;

				float fx = (float)x;
				float fy = (float)y;
				float fz = (float)z;

				// 6방향 검사 및 면 생성
				// 

				// 1. Top (+Y)
				int ny = y + 1;
				if (ny >= (int)VOXEL_CHUNK_Y_SIZE || !m_arrBlocks[BlockIndexing(x, ny, z)].IsOpaque()) {
					VOX_QUAD quad{};
					quad.v1 = { fx,     fy + 1, fz + 1 };
					quad.v2 = { fx + 1, fy + 1, fz + 1 };
					quad.v3 = { fx + 1, fy + 1, fz };
					quad.v4 = { fx,     fy + 1, fz };
					quad.eDir = FACE_DIR::POS_Y;
					quads.push_back(quad);
				}

				// 2. Bottom (-Y)
				ny = y - 1;
				if (ny < 0 || !m_arrBlocks[BlockIndexing(x, ny, z)].IsOpaque()) {
					VOX_QUAD quad{};
					quad.v1 = { fx,     fy, fz };
					quad.v2 = { fx + 1, fy, fz };
					quad.v3 = { fx + 1, fy, fz + 1 };
					quad.v4 = { fx,     fy, fz + 1 };
					quad.eDir = FACE_DIR::NEG_Y;
					quads.push_back(quad);
				}

				// 3. Front (+Z)
				int nz = z + 1;
				if (nz >= (int)VOXEL_CHUNK_Z_SIZE || !m_arrBlocks[BlockIndexing(x, y, nz)].IsOpaque()) {
					VOX_QUAD quad{};
					quad.v1 = { fx,     fy + 1, fz + 1 };
					quad.v2 = { fx,     fy,     fz + 1 };
					quad.v3 = { fx + 1, fy,     fz + 1 };
					quad.v4 = { fx + 1, fy + 1, fz + 1 };
					quad.eDir = FACE_DIR::POS_Z;
					quads.push_back(quad);
				}

				// 4. Back (-Z)
				nz = z - 1;
				if (nz < 0 || !m_arrBlocks[BlockIndexing(x, y, nz)].IsOpaque()) {
					VOX_QUAD quad{};
					quad.v1 = { fx,     fy + 1, fz };
					quad.v2 = { fx + 1, fy + 1, fz };
					quad.v3 = { fx + 1, fy,     fz };
					quad.v4 = { fx,     fy,     fz };
					quad.eDir = FACE_DIR::NEG_Z;
					quads.push_back(quad);
				}

				// 5. Right (+X)
				int nx = x + 1;
				if (nx >= (int)VOXEL_CHUNK_X_SIZE || !m_arrBlocks[BlockIndexing(nx, y, z)].IsOpaque()) {
					VOX_QUAD quad{};
					quad.v1 = { fx + 1, fy + 1, fz };
					quad.v2 = { fx + 1, fy + 1, fz + 1 };
					quad.v3 = { fx + 1, fy,     fz + 1 };
					quad.v4 = { fx + 1, fy,     fz };
					quad.eDir = FACE_DIR::POS_X;
					quads.push_back(quad);
				}

				// 6. Left (-X)
				nx = x - 1;
				if (nx < 0 || !m_arrBlocks[BlockIndexing(nx, y, z)].IsOpaque()) {
					VOX_QUAD quad{};
					quad.v1 = { fx, fy + 1, fz + 1 };
					quad.v2 = { fx, fy + 1, fz };
					quad.v3 = { fx, fy,     fz };
					quad.v4 = { fx, fy,     fz + 1 };
					quad.eDir = FACE_DIR::NEG_X;
					quads.push_back(quad);
				}
			}
		}
	}
}

void CChunk::GreedyCulling(std::vector<VOX_QUAD>& quads)
{// 1. 블록 데이터 초기화 (Height Map 적용 - 이전과 동일)
	for (uint32_t i = 0; i < VOXEL_CHUNK_X_SIZE; ++i) {
		for (uint32_t j = 0; j < VOXEL_CHUNK_Z_SIZE; ++j) {
			auto tmpx = m_iX * VOXEL_CHUNK_X_SIZE + i;
			auto tmpz = m_iZ * VOXEL_CHUNK_Z_SIZE + j;
			float n = CGameInstance::Get().GetVoxelHeightNoise((float)tmpx, (float)tmpz);
			uint32_t height = (uint32_t)((n + 1.0f) * 0.5f * 64.0f);
			for (uint32_t k = 0; k < VOXEL_CHUNK_Y_SIZE; ++k) {
				uint32_t idx = BlockIndexing(i, k, j);
				m_arrBlocks[idx].SetType((k < height) ? Block::TYPE::GRASS : Block::TYPE::AIR);
			}
		}
	}

	//std::vector<VOX_QUAD> quads{};
	uint32_t dims[3] = { VOXEL_CHUNK_X_SIZE, VOXEL_CHUNK_Y_SIZE, VOXEL_CHUNK_Z_SIZE };

	// 2. Greedy Meshing 시작
	for (int d = 0; d < 3; ++d) { // Sweep over 3-axes (X, Y, Z)
		int u = (d + 1) % 3;
		int v = (d + 2) % 3;
		int x[3] = { 0, 0, 0 };
		int q[3] = { 0, 0, 0 };
		q[d] = 1;

		// 마스크 생성 (현재 단면의 면 노출 여부 저장)
		std::vector<int> mask(dims[u] * dims[v]);

		for (x[d] = -1; x[d] < (int)dims[d]; ) {
			int n = 0;
			for (x[v] = 0; x[v] < (int)dims[v]; ++x[v]) {
				for (x[u] = 0; x[u] < (int)dims[u]; ++x[u]) {
					// 현재 블록과 인접 블록 비교
					bool blockCurrent = (0 <= x[d]) ? (m_arrBlocks[BlockIndexing(x[0], x[1], x[2])].GetType() != Block::TYPE::AIR) : false;
					bool blockCompare = (x[d] < (int)dims[d] - 1) ? (m_arrBlocks[BlockIndexing(x[0] + q[0], x[1] + q[1], x[2] + q[2])].GetType() != Block::TYPE::AIR) : false;

					// 두 블록의 상태가 다를 때만 면이 필요함 (Culled 원리 포함)
					if (blockCurrent != blockCompare) {
						// 어느 쪽 면이 노출되는지 방향 결정 (1: 정방향, -1: 역방향)
						mask[n++] = blockCurrent ? 1 : -1;
					}
					else {
						mask[n++] = 0;
					}
				}
			}

			++x[d];
			n = 0;

			// 마스크를 기반으로 쿼드 생성
			for (int j = 0; j < (int)dims[v]; ++j) {
				for (int i = 0; i < (int)dims[u]; ) {
					if (mask[n] != 0) {
						int currentMask = mask[n];
						int w, h;

						// 너비(Width) 계산
						for (w = 1; i + w < (int)dims[u] && mask[n + w] == currentMask; ++w) {}

						// 높이(Height) 계산
						bool done = false;
						for (h = 1; j + h < (int)dims[v]; ++h) {
							for (int k = 0; k < w; ++k) {
								if (mask[n + k + h * dims[u]] != currentMask) {
									done = true;
									break;
								}
							}
							if (done) break;
						}

						// 좌표 결정 및 쿼드 추가
						x[u] = i; x[v] = j;
						int du[3] = { 0, 0, 0 }; du[u] = w;
						int dv[3] = { 0, 0, 0 }; dv[v] = h;

						// 노출 방향(currentMask)에 따른 정점 순서 결정
						if (currentMask > 0) { // 정방향
							quads.push_back({
								_float3{(float)x[0],           (float)x[1],           (float)x[2]},
								_float3{(float)x[0] + du[0],   (float)x[1] + du[1],   (float)x[2] + du[2]},
								_float3{(float)x[0] + du[0] + dv[0], (float)x[1] + du[1] + dv[1], (float)x[2] + du[2] + dv[2]},
								_float3{(float)x[0] + dv[0],   (float)x[1] + dv[1],   (float)x[2] + dv[2]}
								});
						}
						else { // 역방향
							quads.push_back({
								_float3{(float)x[0],           (float)x[1],           (float)x[2]},
								_float3{(float)x[0] + dv[0],   (float)x[1] + dv[1],   (float)x[2] + dv[2]},
								_float3{(float)x[0] + du[0] + dv[0], (float)x[1] + du[1] + dv[1], (float)x[2] + du[2] + dv[2]},
								_float3{(float)x[0] + du[0],   (float)x[1] + du[1],   (float)x[2] + du[2]}
								});
						}

						// 사용한 마스크 영역 초기화
						for (int l = 0; l < h; ++l) {
							for (int k = 0; k < w; ++k) {
								mask[n + k + l * dims[u]] = 0;
							}
						}

						i += w; n += w;
					}
					else {
						++i; ++n;
					}
				}
			}
		}
	}
}

void CChunk::NoCulling(std::vector<VOX_QUAD>& quads)
{
	int32_t dx = 0;
	int32_t dy = 0;
	int32_t dz = 0;
	for (uint32_t i = 0; i < VOXEL_CHUNK_X_SIZE; ++i)
	{
		for (uint32_t j = 0; j < VOXEL_CHUNK_Z_SIZE; ++j)
		{
			auto tmpx = m_iX * VOXEL_CHUNK_X_SIZE + i;
			auto tmpz = m_iZ * VOXEL_CHUNK_Z_SIZE + j;

			float n = CGameInstance::Get().GetVoxelHeightNoise((float)tmpx, (float)tmpz); // [-1, 1]
			uint32_t height = (uint32_t)((n + 1.0f) * 0.5f * 64.0f);

			for (uint32_t k = 0; k < height; ++k)
			{
				uint32_t idx = BlockIndexing(i, k, j);

				if (Block::TYPE::GRASS == m_arrBlocks[idx].GetType())
				{
					// back (-Z)
					quads.push_back({
						_float3{ (float)dx,     (float)dy + 1, (float)dz     },
						_float3{ (float)dx + 1, (float)dy + 1, (float)dz     },
						_float3{ (float)dx + 1, (float)dy,     (float)dz     },
						_float3{ (float)dx,     (float)dy,     (float)dz     }
						});

					// front (+Z)
					quads.push_back({
						_float3{ (float)dx,     (float)dy + 1, (float)dz + 1 },
						_float3{ (float)dx,     (float)dy,     (float)dz + 1 },
						_float3{ (float)dx + 1, (float)dy,     (float)dz + 1 },
						_float3{ (float)dx + 1, (float)dy + 1, (float)dz + 1 }
						});

					// top (+Y)
					quads.push_back({
						_float3{ (float)dx,     (float)dy + 1, (float)dz + 1 },
						_float3{ (float)dx + 1, (float)dy + 1, (float)dz + 1 },
						_float3{ (float)dx + 1, (float)dy + 1, (float)dz     },
						_float3{ (float)dx,     (float)dy + 1, (float)dz     }
						});

					// bottom (-Y)
					quads.push_back({
						_float3{ (float)dx,     (float)dy, (float)dz     },
						_float3{ (float)dx + 1, (float)dy, (float)dz     },
						_float3{ (float)dx + 1, (float)dy, (float)dz + 1 },
						_float3{ (float)dx,     (float)dy, (float)dz + 1 }
						});

					// left (-X)
					quads.push_back({
						_float3{ (float)dx, (float)dy + 1, (float)dz + 1 },
						_float3{ (float)dx, (float)dy + 1, (float)dz     },
						_float3{ (float)dx, (float)dy,     (float)dz     },
						_float3{ (float)dx, (float)dy,     (float)dz + 1 }
						});

					// right (+X)
					quads.push_back({
						_float3{ (float)dx + 1, (float)dy + 1, (float)dz     },
						_float3{ (float)dx + 1, (float)dy + 1, (float)dz + 1 },
						_float3{ (float)dx + 1, (float)dy,     (float)dz + 1 },
						_float3{ (float)dx + 1, (float)dy,     (float)dz     }
						});
				}

				++dy;
			}
			++dz;
			dy = 0;
		}
		++dx;
		dz = 0;
	}

}


HRESULT CChunk::GenBuffer()
{
	//if (m_vertices.empty())
	//{
	//	return S_OK;
	//}
	m_eBufferState = BUFFER_STATE::ING;


	E::CResDynamicVIBuffer::DESC desc{};
	desc.iNumVertices = (uint32_t)m_vertices.size();
	desc.iVertexStride = sizeof(E::VTX_VOXEL);
	desc.vertexDesc = {
		.ByteWidth = desc.iNumVertices * desc.iVertexStride,
		.Usage = D3D11_USAGE_DEFAULT,
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
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};
	desc.indexSubResource = {
		.pSysMem = m_indices.data()
	};
	desc.eIndexFormat = DXGI_FORMAT_R32_UINT;

	desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	auto pBuffer = E::CResDynamicVIBuffer::Create();
	if (FAILED(pBuffer->Load(desc)))
	{
		m_pResDynamicViBuffer.reset();
		m_eBufferState = BUFFER_STATE::NO;
		return E_FAIL;
	};

	{
		//std::lock_guard<std::mutex> lock(m_Mutex);

		m_pResDynamicViBuffer = pBuffer;
		m_sResName = "DYNVIBUFFER_Chunk_" + std::to_string(m_iX) + "_" + std::to_string(m_iZ);
		//E::CGameInstance::Get().AddResource("VOXEL_MANAGER_CHUNK", m_sResName.c_str(), pBuffer);

	}

	m_iNumIndices = (uint32_t)m_indices.size();
	m_indices.clear();
	m_vertices.clear();


	m_eBufferState = BUFFER_STATE::DONE;
	m_eVIState = VI_STATE::NO;


	return S_OK;
}

void CChunk::BindBuffer(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{
	//if (!m_bLoaded)
	//{
	//	return;
	//}

	//if (m_eState == STATE::BUFFER_NO_VI_OK
	//	|| m_eState == STATE::BUFFER_NO_VI_NO)
	//{
	//	return;
	//}

	if (m_eBufferState != BUFFER_STATE::DONE)
	{
		return;
	}

	{
		E::CB_PER_OBJECT cbPerObject{};

		int32_t dx = m_iX * VOXEL_CHUNK_X_SIZE;
		int32_t dz = m_iZ * VOXEL_CHUNK_Z_SIZE;

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
}

void CChunk::Update(_float fTimeDelta)
{


	m_bDirty = false;
}

_bool CChunk::QuadCalc()
{

	m_eVIState = VI_STATE::ING;

	std::vector<VOX_QUAD> quads = GenerateQuad();

	//std::vector<E::VTX_VOXEL> vertices{};
	//std::vector<uint32_t> indices{};
	m_indices.clear();
	m_vertices.clear();

	m_vertices.reserve(quads.size() * 4);
	m_indices.reserve(quads.size() * 6);

	//01
	//32
	for (uint32_t i = 0; i < quads.size(); ++i)
	{
		uint32_t iFaceDir = ETOUI(quads[i].eDir);

		// 0000 0000  0000 0000  0000 0000  0000 0000
		// 

		// normal(3)
		// 1110 0000  0000 0000  0000 0000  0000 0000

		// vertexao(2)
		// 0001 1000  0000 0000  0000 0000  0000 0000

		// vertexid(2)
		// 0000 0110  0000 0000  0000 0000  0000 0000

		// textureid(8)
		// 0000 0001  1111 1110  0000 0000  0000 0000

		E::VTX_VOXEL v{};
		v.pos = quads[i].v1;
		{
			v.packedData = {};
			v.packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;
			v.packedData |= (static_cast<uint32_t>(0) & 0x03) << 25;
		}
		m_vertices.push_back(v);

		v.pos = quads[i].v2;
		{
			v.packedData = {};
			v.packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;
			v.packedData |= (static_cast<uint32_t>(1) & 0x03) << 25;
		}
		m_vertices.push_back(v);

		v.pos = quads[i].v3;
		{
			v.packedData = {};
			v.packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;
			v.packedData |= (static_cast<uint32_t>(2) & 0x03) << 25;
		}
		m_vertices.push_back(v);

		v.pos = quads[i].v4;
		{
			v.packedData = {};
			v.packedData |= (static_cast<uint32_t>(iFaceDir) & 0x07) << 29;
			v.packedData |= (static_cast<uint32_t>(3) & 0x03) << 25;
		}
		m_vertices.push_back(v);

		m_indices.push_back(i * 4 + 0);
		m_indices.push_back(i * 4 + 1);
		m_indices.push_back(i * 4 + 2);
		m_indices.push_back(i * 4 + 0);
		m_indices.push_back(i * 4 + 2);
		m_indices.push_back(i * 4 + 3);
	}


	m_eVIState = VI_STATE::DONE;
	return true;
}

std::mutex m_Mutex2;
HRESULT CChunk::MapBuffer(ID3D11DeviceContext* pContext)
{
	if (false)
	{
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		if (SUCCEEDED(pContext->Map(m_pResDynamicViBuffer->GetVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
		{
			memcpy(mapped.pData, m_vertices.data(), sizeof(VTX_VOXEL) * m_vertices.size());
			pContext->Unmap(m_pResDynamicViBuffer->GetVertexBuffer().Get(), 0);
			m_vertices.clear();
		}

		if (SUCCEEDED(pContext->Map(m_pResDynamicViBuffer->GetIndexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
		{
			memcpy(mapped.pData, m_indices.data(), sizeof(uint32_t) * m_indices.size());
			pContext->Unmap(m_pResDynamicViBuffer->GetIndexBuffer().Get(), 0);
			m_iNumIndices = (uint32_t)m_indices.size();
			m_indices.clear();
		}
	}



	m_eBufferState = BUFFER_STATE::ING;
	
		pContext->UpdateSubresource(
			m_pResDynamicViBuffer->GetVertexBuffer().Get(),  // pDstResource
			0,                                               // DstSubresource
			nullptr,                                         // pDstBox
			m_vertices.data(),                               // pSrcData
			0,                                               // SrcRowPitch (Buffer는 0)
			0                                                // SrcDepthPitch (Buffer는 0)
		);

		{
			std::lock_guard lock(m_Mutex2);
			m_pResDynamicViBuffer->SetNumVertices(m_vertices.size());
		}

		m_vertices.clear();
		if (!m_vertices.empty())
		{
		}

	// Index Buffer 업데이트
	
		pContext->UpdateSubresource(
			m_pResDynamicViBuffer->GetIndexBuffer().Get(),
			0,
			nullptr,
			m_indices.data(),
			0,
			0
		);

		{
			std::lock_guard lock(m_Mutex2);
			m_pResDynamicViBuffer->SetNumIndices(m_indices.size());

			m_iNumIndices = (uint32_t)m_indices.size();
		}
		m_indices.clear();
		if (!m_indices.empty())
		{
		}


		m_eBufferState = BUFFER_STATE::DONE;
	return S_OK;
}



HRESULT CChunk::Initialize(const DESC& desc)
{
	m_iX = desc.iX;
	m_iY = desc.iY;
	m_iZ = desc.iZ;
	m_iChunkCoord = desc.iChunkCoord;
	//BufferLoad();

	m_pResCBufferPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT);

	//m_eState = STATE::BUFFER_NO_VI_NO;

	// create air buffer
	if(false)
	{
		constexpr uint32_t MAX_QUADS_PER_CHUNK = VOXEL_CHUNK_X_SIZE * VOXEL_CHUNK_Z_SIZE * VOXEL_CHUNK_Y_SIZE * 6;  // 196608

		uint32_t initialNumQuad = MAX_QUADS_PER_CHUNK * 0.07f;   // 7% 추천
		uint32_t iInitialNumVertices = initialNumQuad * 4;
		uint32_t iInitialNumIndices = initialNumQuad * 6;

		//16 16 256 = 65536
		// 65536 * 6 = 393216;
		//uint32_t initialNumQuad = 393216 * 0.05;
		//uint32_t iInitialNumVertices = initialNumQuad * 4;
		//uint32_t iInitialNumIndices = initialNumQuad * 6;
		E::CResDynamicVIBuffer::DESC desc{};
		desc.iNumVertices = iInitialNumVertices;
		desc.iVertexStride = sizeof(E::VTX_VOXEL);
		desc.vertexDesc = {
			.ByteWidth = desc.iNumVertices * desc.iVertexStride,
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0
		};
		//desc.vertexSubResource = {
		//	.pSysMem = vertices.data()
		//};

		desc.iIndexStride = sizeof(uint32_t);
		desc.iNumIndices = iInitialNumIndices;
		desc.IndexDesc = {
			.ByteWidth = desc.iNumIndices * desc.iIndexStride,
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0
		};
		//desc.indexSubResource = {
		//	.pSysMem = indices.data()
		//};
		desc.eIndexFormat = DXGI_FORMAT_R32_UINT;

		desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		auto pBuffer = E::CResDynamicVIBuffer::Create();
		if (FAILED(pBuffer->Load(desc)))
		{
			return E_FAIL;
		};

		{
			std::lock_guard lock(m_Mutex2);
			//std::lock_guard<std::mutex> lock(m_Mutex);
			std::random_device rd;
			std::mt19937 gen(rd());

			std::uniform_int_distribution<int> dist(0, 1000);

			int value = dist(gen);
			m_pResDynamicViBuffer = pBuffer;
			m_sResName = "DYNVIBUFFER_Chunk_" + std::to_string(m_iX) + "_" + std::to_string(m_iZ) ;
			//E::CGameInstance::Get().AddResource("VOXEL_MANAGER_CHUNK", m_sResName.c_str(), pBuffer);
		}
	}
	return S_OK;
}

UPtr<CChunk> CChunk::Create(const DESC& desc)
{
	auto pInstnace = ToUPtr(new CChunk{});
	if (FAILED(pInstnace->Initialize(desc)))
	{
		return nullptr;
	}
	return pInstnace;
}

void CChunk::Free()
{
	//E::CGameInstance::Get().DelResource("VOXEL_MANAGER_CHUNK", m_sResName.c_str());
	CEngineBase::Free();
}
