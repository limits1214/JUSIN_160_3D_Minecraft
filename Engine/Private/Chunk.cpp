#include "Chunk.h"
#include <FastNoiseLite.h>
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
    //FastNoiseLite noise;
    //noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	std::vector<VOX_QUAD> quads{};
	enum class KIND
	{
		STUPID,
		GREEDY,
	};
	KIND kind = KIND::STUPID;

	if (kind == KIND::STUPID)
	{
		 int32_t dx = 0;
		 int32_t dy = 0;
		 int32_t dz = 0;
		for (uint32_t i = 0; i < VOXEL_CHUNK_X_SIZE; ++i)
		{
			for (uint32_t j = 0; j < VOXEL_CHUNK_Z_SIZE; ++j)
			{
				for (uint32_t k = 0; k < VOXEL_CHUNK_Y_SIZE; ++k)
				{
					uint32_t idx = BlockIndexing(i, k, j);

					if (Block::TYPE::GRASS == m_arrBlocks[idx].type)
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
	else if (kind == KIND::GREEDY)
	{

	}


    return quads;
}

HRESULT CChunk::BufferLoad()
{
	auto quads = GenerateQuad();


	std::vector<E::VTX_COL> vertices{};
	std::vector<uint32_t> indices{};

	for (uint32_t i = 0; i < quads.size(); ++i)
	{
		E::VTX_COL v{};
		v.pos = quads[i].v1;
		vertices.push_back(v);
		v.pos = quads[i].v2;
		vertices.push_back(v);
		v.pos = quads[i].v3;
		vertices.push_back(v);
		v.pos = quads[i].v4;
		vertices.push_back(v);

		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
	}




	//vertices.resize(8);
	//vertices[0].pos = {-0.5f, 0.5f, -0.5f };
	//vertices[1].pos = { 0.5f, 0.5f, -0.5f };
	//vertices[2].pos = { 0.5f, -0.5f, -0.5f };
	//vertices[3].pos = { -0.5f, -0.5f, -0.5f };

	//vertices[4].pos = { -0.5f, 0.5f, 0.5f};
	//vertices[5].pos = { 0.5f, 0.5f, 0.5f };
	//vertices[6].pos = { 0.5f, 0.5f, -0.5f };
	//vertices[7].pos = { -0.5f, 0.5f, -0.5f };

	for (auto& v : vertices)
	{
		v.color = { 1.f, 1.f, 1.f, 1.f };
	}

	E::CResDynamicVIBuffer::DESC desc{};
	desc.iNumVertices = (uint32_t)vertices.size();
	desc.iVertexStride = sizeof(E::VTX_COL);
	desc.vertexDesc = {
		.ByteWidth = desc.iNumVertices * desc.iVertexStride,
		.Usage = D3D11_USAGE_DEFAULT,
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
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};
	desc.indexSubResource = {
		.pSysMem = indices.data()
	};
	desc.eIndexFormat = DXGI_FORMAT_R32_UINT;

	desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	auto pBuffer = E::CResDynamicVIBuffer::Create();
	if (FAILED(pBuffer->Load(desc)))
	{
		return E_FAIL;
	};

	m_pResDynamicViBuffer = pBuffer;
	m_sResName = "DYNVIBUFFER_Chunk_" + std::to_string(m_iX) + "_" + std::to_string(m_iZ);
	E::CGameInstance::Get().AddResource("VOXEL_MANAGER", m_sResName.c_str(), pBuffer);
	return S_OK;
}

void CChunk::BindBuffer(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{

	{
		auto cBufferPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
		E::CB_PER_OBJECT cbPerObject{};


		int32_t dx = m_iX * VOXEL_CHUNK_X_SIZE;
		int32_t dz = m_iZ * VOXEL_CHUNK_Z_SIZE;

		auto worldMat = XMMatrixTranslation(dx, 0, dz);


		XMStoreFloat4x4(&cbPerObject.matWorld, worldMat);
		XMStoreFloat4x4(&cbPerObject.matWVP, worldMat * ctx.matView * ctx.matProj);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(pContext->Map(cBufferPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			memcpy(mappedResource.pData, &cbPerObject, sizeof(E::CB_PER_OBJECT));
			pContext->Unmap(cBufferPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, cBufferPerObject->GetCBuffer().GetAddressOf());
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


HRESULT CChunk::Initialize(const DESC& desc)
{
	m_iX = desc.iX;
	m_iZ = desc.iZ;
	m_iChunkCoord = desc.iChunkCoord;
	BufferLoad();
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
	E::CGameInstance::Get().DelResource("VOXEL_MANAGER", m_sResName.c_str());
	CEngineBase::Free();
}
