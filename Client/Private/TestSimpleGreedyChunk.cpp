#include "TestSimpleGreedyChunk.h"
#include "GameInstance.h"
#include "Resources.h"


NS_USING(Client)

CTestSimpleGreedyChunk::CTestSimpleGreedyChunk()
{

}
CTestSimpleGreedyChunk::~CTestSimpleGreedyChunk()
{
}

static std::vector< std::array< std::array<int32_t, 3>, 4>> test(std::vector<int32_t>& testVolume, std::array<int32_t, 3> dims)
{
	auto f = [&](int32_t i, int32_t j, int32_t k)->int32_t
		{
			int32_t xCnt = dims[0];

			int32_t yCnt = dims[1];

			int32_t zCnt = dims[2];

			int32_t idx = (xCnt * zCnt * j) + (xCnt * k) + i;
			
			return testVolume[idx];
		};


	std::vector< std::array< std::array<int32_t, 3>, 4>> quads{};

	for (int32_t d = 0; d < 3; ++d)
	{
		std::array<int32_t, 3> q{}, x{};
		int32_t u{ (d + 1) % 3 };
		int32_t v{ (d + 2) % 3 };
		std::vector<int32_t> mask{};
		mask.resize(dims[u] * dims[v]);

		q[d] = 1;

		for (x[d] = -1; x[d] < dims[d];)
		{
			int32_t n{};
			for (x[v] = 0; x[v] < dims[v]; ++x[v])
			{
				for (x[u] = 0; x[u] < dims[u]; ++x[u])
				{
					bool b =
					0 <= x[d] ? f(x[0], x[1], x[2]) : 0
					!=
					x[d] < dims[d] - 1 ? f(x[0] + q[0], x[1] + q[1], x[2] + q[2]) : 0;


					if (b)
					{
						mask[n++] = 1;
					}
					else
					{
						mask[n++] = 0;
					}
				}
			}

			++x[d];

			n = 0;
			int32_t w{};
			int32_t h{};
			int32_t k{};
			for (int32_t j = 0; j < dims[v]; ++j)
			{
				for (int32_t i = 0; i < dims[u]; )
				{
					if (mask[n] == 1)
					{
						for(w = 1; mask[n+w] && i + w <dims[u]; ++w)
						{ }

						bool done = false;

						for ( h = 1; j + h < dims[v]; ++h)
						{
							for (k = 0; k < w; ++k)
							{
								if (!mask[n + k + h * dims[u]])
								{
									done = true;
									break;
								}
							}
							if (done)
							{
								break;
							}
						}

						x[u] = i;
						x[v] = j;

						std::array<int32_t, 3> du{ 0, 0, 0 };
						du[u] = w;
						std::array<int32_t, 3> dv{ 0, 0, 0 };
						dv[v] = h;

						std::array< std::array<int32_t, 3>, 4> quad{};
						quad[0] = { x[0],x[1], x[2]};
						quad[1] = { x[0] + du[0],x[1] + du[1], x[2] + du[2] };
						quad[2] = { x[0] + du[0] + dv[0],x[1] + du[1] + dv[1], x[2] + du[2] + dv[2] };
						quad[3] = { x[0] + dv[0],x[1] + dv[1], x[2] + dv[2] };

						quads.push_back(quad);

						for (int32_t l = 0; l < h; ++l)
						{
							for (k = 0; k < w; ++k)
							{
								mask[n + k + l * dims[u]] = false;
							}
						}

						i += w;
						n += w;
					}
					else
					{
						++i;
						++n;
					}
				}
			}
		}


	}
	return quads;
}



HRESULT CTestSimpleGreedyChunk::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

	//std::vector<std::vector<std::vector<uint32_t>>> testVolume = {
	//	{
	//		{ 1,1,1 },
	//		{ 1,1,1 },
	//		{ 1,1,1 }
	//	},
	//	{
	//		{ 1,1,1 },
	//		{ 1,1,1 },
	//		{ 1,1,1 }
	//	},
	//	{
	//		{ 1,1,1 },
	//		{ 1,1,1 },
	//		{ 1,1,1 }
	//	}
	//};

	std::vector<std::vector<std::vector<uint32_t>>> testVolume(
		8, std::vector<std::vector<uint32_t>>(
			8, std::vector<uint32_t>(8, 1)
		)
	);

	std::vector<int32_t> testvol1(8 * 8 * 8, 1);
	auto sie = testvol1.size();
	auto aaa = test(testvol1, { 8,8,8 });


	struct Quad
	{
		E::_float3 v1;
		E::_float3 v2;
		E::_float3 v3;
		E::_float3 v4;
	};
	std::vector<Quad> quads{};


	uint32_t dx = 0;
	uint32_t dy = 0;
	uint32_t dz = 0;
	// y
	for (const std::vector<std::vector<uint32_t>>& v1 : testVolume)
	{
		// z
		for (const std::vector<uint32_t>& v2 : v1)
		{
			//x
			for (const uint32_t& v3 : v2)
			{
				if (v3 == 1)
				{
					// back (-Z)
					quads.push_back({
						{-0.5f + (dx),  0.5f + (dy), -0.5f + (dz)},
						{ 0.5f + (dx),  0.5f + (dy), -0.5f + (dz)},
						{ 0.5f + (dx), -0.5f + (dy), -0.5f + (dz)},
						{-0.5f + (dx), -0.5f + (dy), -0.5f + (dz)}
						});

					// front (+Z)
					quads.push_back({
						{-0.5f + (dx),  0.5f + (dy),  0.5f + (dz)},
						{-0.5f + (dx), -0.5f + (dy),  0.5f + (dz)},
						{ 0.5f + (dx), -0.5f + (dy),  0.5f + (dz)},
						{ 0.5f + (dx),  0.5f + (dy),  0.5f + (dz)}
						});

					// top (+Y)
					quads.push_back({
						{-0.5f + (dx),  0.5f + (dy),  0.5f + (dz)},
						{ 0.5f + (dx),  0.5f + (dy),  0.5f + (dz)},
						{ 0.5f + (dx),  0.5f + (dy), -0.5f + (dz)},
						{-0.5f + (dx),  0.5f + (dy), -0.5f + (dz)}
						});

					// bottom (-Y)
					quads.push_back({
						{-0.5f + (dx), -0.5f + (dy), -0.5f + (dz)},
						{ 0.5f + (dx), -0.5f + (dy), -0.5f + (dz)},
						{ 0.5f + (dx), -0.5f + (dy),  0.5f + (dz)},
						{-0.5f + (dx), -0.5f + (dy),  0.5f + (dz)}
						});

					// left (-X)
					quads.push_back({
						{-0.5f + (dx),  0.5f + (dy),  0.5f + (dz)},
						{-0.5f + (dx),  0.5f + (dy), -0.5f + (dz)},
						{-0.5f + (dx), -0.5f + (dy), -0.5f + (dz)},
						{-0.5f + (dx), -0.5f + (dy),  0.5f + (dz)}
						});

					// right (+X)
					quads.push_back({
						{ 0.5f + (dx),  0.5f + (dy), -0.5f + (dz)},
						{ 0.5f + (dx),  0.5f + (dy),  0.5f + (dz)},
						{ 0.5f + (dx), -0.5f + (dy),  0.5f + (dz)},
						{ 0.5f + (dx), -0.5f + (dy), -0.5f + (dz)}
						});
				}
				
				++dx;
			}

			++dz;
			dx = 0;
		}

		++dy;
		dz = 0;
	}

	

	
	
	



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
	E::CGameInstance::Get().AddResource("LEVEL_TEST_SIMPLE_GREEDY", "DYNVIBUFFER_Chunk", pBuffer);

    return S_OK;
}

void CTestSimpleGreedyChunk::PriorityUpdate(E::_float fTimeDelta)
{
	int x = 0;
}

void CTestSimpleGreedyChunk::Update(E::_float fTimeDelta)
{
}

void CTestSimpleGreedyChunk::LateUpdate(E::_float fTimeDelta)
{
	E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	GetTransform().Update();
}

HRESULT CTestSimpleGreedyChunk::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::GetConst().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_QuadCol");
	const auto& ps = E::CGameInstance::GetConst().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_QuadCol");
	const auto& viBuffer = E::CGameInstance::GetConst().GetResourceFirst<E::CResDynamicVIBuffer>("LEVEL_TEST_SIMPLE_GREEDY", "DYNVIBUFFER_Chunk");

	pContext->IASetInputLayout(vs->GetInputLayout().Get());
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

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

	{
		auto cBufferPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
		E::CB_PER_OBJECT cbPerObject{};
		cbPerObject.matWorld = *GetTransform().GetWorldMatrix();
		XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedWorldMatrix() * ctx.matView * ctx.matProj);
		
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(pContext->Map(cBufferPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			memcpy(mappedResource.pData, &cbPerObject, sizeof(E::CB_PER_OBJECT));
			pContext->Unmap(cBufferPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, cBufferPerObject->GetCBuffer().GetAddressOf());
	}

	if (1)
	{
		const auto& rasterizer = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_WIREFRAME_NOCULL);
		pContext->RSSetState(rasterizer->GetRasterizerState().Get());
	}

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);

    return S_OK;
}

E::UPtr<CTestSimpleGreedyChunk> CTestSimpleGreedyChunk::Create()
{
	auto pInstance = E::ToUPtr(new CTestSimpleGreedyChunk{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTestSimpleGreedyChunk");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CTestSimpleGreedyChunk::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CTestSimpleGreedyChunk{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestSimpleGreedyChunk");
		return nullptr;
	}
	return pInstance;
}
