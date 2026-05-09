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

void CTestSimpleGreedyChunk::UpdateGUI()
{
	CGameObject::UpdateGUI();

	if (ImGui::Button("test"))
	{
		m_bTest = !m_bTest;
		BuildMesh();
	}
}

static std::vector<std::array<std::array<int32_t, 3>, 4>> test(std::vector<int32_t>& testVolume, std::array<int32_t, 3> dims)
{
	// BuildMesh의 flat 생성 순서(Y->Z->X)에 맞춘 인덱싱
	auto f = [&](int32_t x, int32_t y, int32_t z) -> int32_t {
		if (x < 0 || x >= dims[0] || y < 0 || y >= dims[1] || z < 0 || z >= dims[2]) return 0;
		return testVolume[x + dims[0] * (z + dims[2] * y)];
		};

	std::vector<std::array<std::array<int32_t, 3>, 4>> quads{};

	for (int32_t d = 0; d < 3; ++d) {
		int32_t u = (d + 1) % 3;
		int32_t v = (d + 2) % 3;
		std::array<int32_t, 3> x{}, q{};
		q[d] = 1;

		std::vector<int32_t> mask(dims[u] * dims[v]);

		// x[d] 증가를 for문 끝에서 수행하도록 변경
		for (x[d] = -1; x[d] < dims[d]; ++x[d]) {
			int32_t n = 0;
			// 마스크 생성
			for (x[v] = 0; x[v] < dims[v]; ++x[v]) {
				for (x[u] = 0; x[u] < dims[u]; ++x[u]) {
					int32_t a = f(x[0], x[1], x[2]);
					int32_t b = f(x[0] + q[0], x[1] + q[1], x[2] + q[2]);
					mask[n++] = (a != b);
				}
			}

			// 쿼드 생성
			n = 0;
			for (int32_t j = 0; j < dims[v]; ++j) {
				for (int32_t i = 0; i < dims[u]; ) {
					if (mask[n]) {
						int32_t w, h;
						// 가로 확장
						for (w = 1; i + w < dims[u] && mask[n + w]; ++w);

						// 세로 확장
						bool done = false;
						for (h = 1; j + h < dims[v]; ++h) {
							for (int32_t k = 0; k < w; ++k) {
								if (!mask[n + k + h * dims[u]]) {
									done = true; break;
								}
							}
							if (done) break;
						}

						// 좌표 기록 (x[d]가 증가하기 전이므로 +1 보정 필요할 수 있음)
						// 면의 위치는 x[d]와 x[d]+1 사이입니다.
						x[u] = i; x[v] = j;
						std::array<int32_t, 3> du{ 0 }, dv{ 0 };
						du[u] = w; dv[v] = h;

						// 렌더링 시 보정: x[d] + 1 지점에 면을 생성
						std::array<int32_t, 3> offset = x;
						offset[d]++;

						quads.push_back({ {
							{ offset[0], offset[1], offset[2] },
							{ offset[0] + du[0], offset[1] + du[1], offset[2] + du[2] },
							{ offset[0] + du[0] + dv[0], offset[1] + du[1] + dv[1], offset[2] + du[2] + dv[2] },
							{ offset[0] + dv[0], offset[1] + dv[1], offset[2] + dv[2] }
						} });

						for (int l = 0; l < h; ++l)
							for (int k = 0; k < w; ++k)
								mask[n + k + l * dims[u]] = 0;

						i += w; n += w;
					}
					else {
						++i; ++n;
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
	BuildMesh();
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

HRESULT CTestSimpleGreedyChunk::BuildMesh()
{
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
	std::array<int32_t, 3> dims{ 16,32,16 };//x/y/z
	std::vector<std::vector<std::vector<int32_t>>> testVolume(
		dims[1], std::vector<std::vector<int32_t>>(
			dims[2], std::vector<int32_t>(dims[0], 1)
		)
	);

	testVolume[0][0][0] = 0;
	testVolume[0][0][9] = 0;
	testVolume[0][0][10] = 0;
	testVolume[12][2][11] = 0;
	testVolume[11][2][11] = 0;
	testVolume[12][1][11] = 0;
	testVolume[11][1][11] = 0;
	testVolume[12][0][11] = 0;
	testVolume[11][0][11] = 0;
	testVolume[12][0][10] = 0;
	testVolume[11][0][10] = 0;

	std::vector<int32_t> flat;
	flat.reserve(dims[0] * dims[1] * dims[2]);

	for (int y = 0; y < dims[1]; ++y)
	{
		for (int z = 0; z < dims[2]; ++z)
		{
			for (int x = 0; x < dims[0]; ++x)
			{
				flat.push_back(testVolume[y][z][x]);
			}
		}
	}


	

	//std::vector<int32_t> testvol1(8 * 8 * 8, 1);
	//auto sie = testvol1.size();
	auto aaa = test(flat, dims);


	struct Quad
	{
		E::_float3 v1;
		E::_float3 v2;
		E::_float3 v3;
		E::_float3 v4;
	};
	std::vector<Quad> quads{};



	if (m_bTest)
	{
		float dx = 0;
		float dy = 0;
		float dz = 0;
		// y
		for (const std::vector<std::vector<int32_t>>& v1 : testVolume)
		{
			// z
			for (const std::vector<int32_t>& v2 : v1)
			{
				//x
				for (const int32_t& v3 : v2)
				{
					if (v3 == 1)
					{
						// back (-Z)
						quads.push_back({
							{ dx,     dy + 1, dz     },
							{ dx + 1, dy + 1, dz     },
							{ dx + 1, dy,     dz     },
							{ dx,     dy,     dz     }
							});

						// front (+Z)
						quads.push_back({
							{ dx,     dy + 1, dz + 1 },
							{ dx,     dy,     dz + 1 },
							{ dx + 1, dy,     dz + 1 },
							{ dx + 1, dy + 1, dz + 1 }
							});

						// top (+Y)
						quads.push_back({
							{ dx,     dy + 1, dz + 1 },
							{ dx + 1, dy + 1, dz + 1 },
							{ dx + 1, dy + 1, dz     },
							{ dx,     dy + 1, dz     }
							});

						// bottom (-Y)
						quads.push_back({
							{ dx,     dy, dz     },
							{ dx + 1, dy, dz     },
							{ dx + 1, dy, dz + 1 },
							{ dx,     dy, dz + 1 }
							});

						// left (-X)
						quads.push_back({
							{ dx, dy + 1, dz + 1 },
							{ dx, dy + 1, dz     },
							{ dx, dy,     dz     },
							{ dx, dy,     dz + 1 }
							});

						// right (+X)
						quads.push_back({
							{ dx + 1, dy + 1, dz     },
							{ dx + 1, dy + 1, dz + 1 },
							{ dx + 1, dy,     dz + 1 },
							{ dx + 1, dy,     dz     }
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

	}
	else
	{
		for (const auto& a : aaa)
		{
			Quad quad{};
			quad.v1 = E::_float3{ (float)a[0][0], (float)a[0][1], (float)a[0][2] };
			quad.v2 = E::_float3{ (float)a[1][0], (float)a[1][1], (float)a[1][2] };
			quad.v3 = E::_float3{ (float)a[2][0],(float)a[2][1], (float)a[2][2] };
			quad.v4 = E::_float3{ (float)a[3][0], (float)a[3][1], (float)a[3][2] };
			quads.push_back(quad);
		}
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
	E::CGameInstance::Get().DelResource("LEVEL_TEST_SIMPLE_GREEDY", "DYNVIBUFFER_Chunk");
	E::CGameInstance::Get().AddResource("LEVEL_TEST_SIMPLE_GREEDY", "DYNVIBUFFER_Chunk", pBuffer);

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
