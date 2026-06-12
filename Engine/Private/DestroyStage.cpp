#include "DestroyStage.h"

#include "GameInstance.h"

#include "Resources.h"

NS_USING(Engine)

CDestroyStage::CDestroyStage()
{
}

CDestroyStage::~CDestroyStage()
{
}

HRESULT CDestroyStage::InitializePrototype(void* pArg)
{
	// 일반적인경우 쿼드 6개 인데 넉넉히 한 12개정도 다이나믹으로
	// max quad vi buffer
	CResDynamicVIBuffer::DESC desc{};
	desc.iNumVertices = 48; // 쿼드 12개 * 4
	desc.iVertexStride = sizeof(VTX_DESTROY_STAGE);
	desc.vertexDesc = {
		.ByteWidth = desc.iNumVertices * desc.iVertexStride,
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0
	};
	//desc.vertexSubResource = {
	//	.pSysMem = cubeEdges
	//};
	desc.iNumIndices = 72; // 12쿼드 * 6
	desc.iIndexStride = sizeof(uint16_t);
	desc.IndexDesc = {
		.ByteWidth = desc.iNumIndices * desc.iIndexStride,
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE,
		.MiscFlags = 0
	};
	desc.eIndexFormat = DXGI_FORMAT_R16_UINT;
	//desc.indexSubResource = 

	m_pVIBuffer = CResDynamicVIBuffer::Create();
	if (FAILED(m_pVIBuffer->Load(desc)))
	{
		return E_FAIL;
	}
    return S_OK;
}

HRESULT CDestroyStage::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

		// +X
		m_quads.push_back({ {
			{ {1,0,0}, {1,0,0}, {0,1} },
			{ {1,1,0}, {1,0,0}, {0,0} },
			{ {1,1,1}, {1,0,0}, {1,0} },
			{ {1,0,1}, {1,0,0}, {1,1} },
		} });

		// -X
		m_quads.push_back({ {
			{ {0,0,1}, {-1,0,0}, {0,1} },
			{ {0,1,1}, {-1,0,0}, {0,0} },
			{ {0,1,0}, {-1,0,0}, {1,0} },
			{ {0,0,0}, {-1,0,0}, {1,1} },
		} });

		// +Y
		m_quads.push_back({ {
			{ {0,1,0}, {0,1,0}, {0,1} },
			{ {0,1,1}, {0,1,0}, {0,0} },
			{ {1,1,1}, {0,1,0}, {1,0} },
			{ {1,1,0}, {0,1,0}, {1,1} },
		} });

		// -Y
		m_quads.push_back({ {
			{ {0,0,1}, {0,-1,0}, {0,1} },
			{ {0,0,0}, {0,-1,0}, {0,0} },
			{ {1,0,0}, {0,-1,0}, {1,0} },
			{ {1,0,1}, {0,-1,0}, {1,1} },
		} });

		// +Z
		m_quads.push_back({ {
			{ {1,0,1}, {0,0,1}, {0,1} },
			{ {1,1,1}, {0,0,1}, {0,0} },
			{ {0,1,1}, {0,0,1}, {1,0} },
			{ {0,0,1}, {0,0,1}, {1,1} },
		} });

		// -Z
		m_quads.push_back({ {
			{ {0,0,0}, {0,0,-1}, {0,1} },
			{ {0,1,0}, {0,0,-1}, {0,0} },
			{ {1,1,0}, {0,0,-1}, {1,0} },
			{ {1,0,0}, {0,0,-1}, {1,1} },
		} });
    return S_OK;
}

void CDestroyStage::PriorityUpdate(E::_float fTimeDelta)
{
	m_iLight = 0xFF;
}

void CDestroyStage::Update(E::_float fTimeDelta)
{
	m_fElapsed += fTimeDelta;

	float goal = 1.f;


	//m_iFrameIndex = std::min((int)(m_fElapsed / (goal / 10.f)), 9);
	//if (m_fElapsed > goal)
	//{
	//	m_fElapsed = 0.f;
	//}
}

void CDestroyStage::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::BLEND, this);
	}
	//GetTransform().SetScale(XMVectorSet(1.01f, 1.11f, 1.01f, 0.f));
	GetTransform().Update();
}

HRESULT CDestroyStage::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	UpdateVertexBuffer(pContext);
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DestroyStage");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DestroyStage");
	const auto& viBuffer = m_pVIBuffer;

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
		auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
			E::CB_PER_OBJECT cbPerObject{};
			cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
			XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);

			memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
			pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
		pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
	}

	{
		auto pCbPerDestroyStage = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerDestroyStage");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pCbPerDestroyStage->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
			E::CB_PER_DESTROYSTAGE destryStage{};
			destryStage.destroyStage = m_iFrameIndex;
			destryStage.light = m_iLight;
			
			memcpy(mappedSubResource.pData, &destryStage, sizeof(destryStage));
			pContext->Unmap(pCbPerDestroyStage->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(6, 1, pCbPerDestroyStage->GetCBuffer().GetAddressOf());
		pContext->PSSetConstantBuffers(6, 1, pCbPerDestroyStage->GetCBuffer().GetAddressOf());
	}
	{
		const auto& rasterizer = CGameInstance::Get().GetResourceFirst<CResRasterizerState>(
			TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_BACKCULL);
		pContext->RSSetState(rasterizer->GetRasterizerState().Get());
	}
	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP_NOMIP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}

	const auto& alphaBlend = E::CGameInstance::GetConst().GetResourceFirst<E::CResBlendState>(TAG_RES_GRP_PERMANENT_STATE, "BS_ALPHA_BLEND");
	//const auto& alphaDepth = E::CGameInstance::GetConst().GetResourceFirst<E::CResDepthStencilState>(TAG_RES_GRP_PERMANENT_STATE, "DS_NO_DEPTHWRITE");
	_float fBlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	pContext->OMSetBlendState(alphaBlend->GetBlendState().Get(), fBlendFactor, 0xffffffff);

	pContext->DrawIndexed(viBuffer->GetNumIndices(),0, 0);

	pContext->OMSetBlendState(nullptr, fBlendFactor, 0xffffffff);
    return S_OK;
}

void CDestroyStage::UpdateVertexBuffer(ID3D11DeviceContext* pContext)
{
	if (!m_bDirty) return;

	// 버텍스 평탄화
	std::vector<VTX_DESTROY_STAGE> vertices;
	std::vector<uint16_t> indices;

	for (int i = 0; i < m_quads.size(); ++i)
	{
		int base = i * 4;
		for (int v = 0; v < 4; ++v)
			vertices.push_back(m_quads[i].v[v]);

		// 쿼드 → 삼각형 2개
		indices.push_back(base + 0);
		indices.push_back(base + 1);
		indices.push_back(base + 2);
		indices.push_back(base + 0);
		indices.push_back(base + 2);
		indices.push_back(base + 3);
	}

	// Dynamic VB Map/Unmap
	D3D11_MAPPED_SUBRESOURCE mapped{};
	if (SUCCEEDED(pContext->Map(m_pVIBuffer->GetVertexBuffer().Get(),
		0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		memcpy(mapped.pData, vertices.data(),
			vertices.size() * sizeof(VTX_DESTROY_STAGE));
		pContext->Unmap(m_pVIBuffer->GetVertexBuffer().Get(), 0);
	}

	if (SUCCEEDED(pContext->Map(m_pVIBuffer->GetIndexBuffer().Get(),
		0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		memcpy(mapped.pData, indices.data(),
			indices.size() * sizeof(uint16_t));
		pContext->Unmap(m_pVIBuffer->GetIndexBuffer().Get(), 0);
	}
	m_currentIndexCount = (uint32_t)indices.size();
	m_bDirty = false;
}

UPtr<CDestroyStage> CDestroyStage::Create()
{
	auto pInstance = ToUPtr(new CDestroyStage{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CDestroyStage");
		return nullptr;
	}

	return pInstance;
}

UPtr<CPrototype> CDestroyStage::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CDestroyStage{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CDestroyStage");
		return nullptr;
	}

	return pInstance;
}
