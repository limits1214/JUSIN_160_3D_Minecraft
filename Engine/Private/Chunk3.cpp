#include "pch.h"
#include "Chunk3.h"

NS_USING(Engine)

HRESULT CChunk3::BlockFilling()
{
	m_eBlockFillingState = BLOCKFILLING_STATE::ING;
	for (uint32_t i = 0; i < VOXEL_CHUNK_X_SIZE3; ++i)
	{
		for (uint32_t j = 0; j < VOXEL_CHUNK_Z_SIZE3; ++j)
		{
			auto tmpx = m_iX * VOXEL_CHUNK_X_SIZE3 + i;
			auto tmpz = m_iZ * VOXEL_CHUNK_Z_SIZE3 + j;
			float n = CGameInstance::Get().GetVoxelHeightNoise((float)tmpx, (float)tmpz);
			float t = (n + 1.0f) * 0.5f;   // 0~1
			uint32_t height = (uint32_t)(32.0f + t * 64.0f); // 64~128
			for (uint32_t k = 0; k < VOXEL_CHUNK_Y_SIZE3; ++k)
			{
				uint32_t idx = BlockIndexing(i, k, j);
				if (k < height)
					m_arrBlocks[idx].SetType(CBlock3::TYPE::GRASS);
				else
					m_arrBlocks[idx].SetType(CBlock3::TYPE::AIR);
			}
		}
	}
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
					bool bExpose = (ny >= (int)VOXEL_CHUNK_Y_SIZE3)
						? true  // 청크 Y 상단 경계 - 인접 청크 없음(혹은 항상 노출)
						: !m_arrBlocks[BlockIndexing(x, ny, z)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx,     fy + 1, fz + 1 };
						quad.v2 = { fx + 1, fy + 1, fz + 1 };
						quad.v3 = { fx + 1, fy + 1, fz };
						quad.v4 = { fx,     fy + 1, fz };
						quad.eDir = FACE_DIR::POS_Y;
						quads.push_back(quad);
					}
				}

				// 2. Bottom (-Y)
				{
					int ny = y - 1;
					bool bExpose = (ny < 0)
						? true
						: !m_arrBlocks[BlockIndexing(x, ny, z)].IsOpaque();
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx,     fy, fz };
						quad.v2 = { fx + 1, fy, fz };
						quad.v3 = { fx + 1, fy, fz + 1 };
						quad.v4 = { fx,     fy, fz + 1 };
						quad.eDir = FACE_DIR::NEG_Y;
						quads.push_back(quad);
					}
				}

				// 3. Front (+Z)
				{
					int nz = z + 1;
					bool bExpose;
					if (nz >= (int)VOXEL_CHUNK_Z_SIZE3) {
						// 인접 청크의 z=0 줄 확인
						bExpose = (pPlusZ == nullptr)
							? true
							: !pPlusZ->m_arrBlocks[pPlusZ->BlockIndexing(x, y, 0)].IsOpaque();
					}
					else {
						bExpose = !m_arrBlocks[BlockIndexing(x, y, nz)].IsOpaque();
					}
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx,     fy + 1, fz + 1 };
						quad.v2 = { fx,     fy,     fz + 1 };
						quad.v3 = { fx + 1, fy,     fz + 1 };
						quad.v4 = { fx + 1, fy + 1, fz + 1 };
						quad.eDir = FACE_DIR::POS_Z;
						quads.push_back(quad);
					}
				}

				// 4. Back (-Z)
				{
					int nz = z - 1;
					bool bExpose;
					if (nz < 0) {
						bExpose = (pMinusZ == nullptr)
							? true
							: !pMinusZ->m_arrBlocks[pMinusZ->BlockIndexing(x, y, (int)VOXEL_CHUNK_Z_SIZE3 - 1)].IsOpaque();
					}
					else {
						bExpose = !m_arrBlocks[BlockIndexing(x, y, nz)].IsOpaque();
					}
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx,     fy + 1, fz };
						quad.v2 = { fx + 1, fy + 1, fz };
						quad.v3 = { fx + 1, fy,     fz };
						quad.v4 = { fx,     fy,     fz };
						quad.eDir = FACE_DIR::NEG_Z;
						quads.push_back(quad);
					}
				}

				// 5. Right (+X)
				{
					int nx = x + 1;
					bool bExpose;
					if (nx >= (int)VOXEL_CHUNK_X_SIZE3) {
						bExpose = (pPlusX == nullptr)
							? true
							: !pPlusX->m_arrBlocks[pPlusX->BlockIndexing(0, y, z)].IsOpaque();
					}
					else {
						bExpose = !m_arrBlocks[BlockIndexing(nx, y, z)].IsOpaque();
					}
					if (bExpose) {
						VOX_QUAD quad{};
						quad.v1 = { fx + 1, fy + 1, fz };
						quad.v2 = { fx + 1, fy + 1, fz + 1 };
						quad.v3 = { fx + 1, fy,     fz + 1 };
						quad.v4 = { fx + 1, fy,     fz };
						quad.eDir = FACE_DIR::POS_X;
						quads.push_back(quad);
					}
				}

				// 6. Left (-X)
				{
					int nx = x - 1;
					bool bExpose;
					if (nx < 0) {
						bExpose = (pMinusX == nullptr)
							? true
							: !pMinusX->m_arrBlocks[pMinusX->BlockIndexing((int)VOXEL_CHUNK_X_SIZE3 - 1, y, z)].IsOpaque();
					}
					else {
						bExpose = !m_arrBlocks[BlockIndexing(nx, y, z)].IsOpaque();
					}
					if (bExpose) {
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