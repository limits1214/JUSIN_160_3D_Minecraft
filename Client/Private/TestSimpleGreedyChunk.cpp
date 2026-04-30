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

HRESULT CTestSimpleGreedyChunk::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
	{
		return E_FAIL;
	}



	std::vector<E::VTX_COL> vertices{};
	vertices.resize(8);
	vertices[0].pos = {-0.5f, 0.5f, -0.5f };
	vertices[1].pos = { 0.5f, 0.5f, -0.5f };
	vertices[2].pos = { 0.5f, -0.5f, -0.5f };
	vertices[3].pos = { -0.5f, -0.5f, -0.5f };

	vertices[4].pos = { -0.5f, 0.5f, 0.5f};
	vertices[5].pos = { 0.5f, 0.5f, 0.5f };
	vertices[6].pos = { 0.5f, 0.5f, -0.5f };
	vertices[7].pos = { -0.5f, 0.5f, -0.5f };

	for (auto& v : vertices)
	{
		v.color = { 1.f, 1.f, 1.f, 1.f };
	}

	std::vector<uint32_t> indices{};
	indices.resize(12);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 4;
	indices[7] = 5;
	indices[8] = 6;

	indices[9] = 4;
	indices[10] = 6;
	indices[11] = 7;

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
