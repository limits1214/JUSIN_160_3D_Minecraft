#include "BlockOutline.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CameraObject.h"

#include "Block3.h"
NS_USING(Engine)

CBlockOutline::CBlockOutline()
{
}

CBlockOutline::~CBlockOutline()
{
}

static const XMFLOAT3 cubeEdges[] =
{
	// bottom
	{-1,-1,-1},{+1,-1,-1},  {+1,-1,-1},{+1,-1,+1},  {+1,-1,+1},{-1,-1,+1},  {-1,-1,+1},{-1,-1,-1},
	// top
	{-1,+1,-1},{+1,+1,-1},  {+1,+1,-1},{+1,+1,+1},  {+1,+1,+1},{-1,+1,+1},  {-1,+1,+1},{-1,+1,-1},
	// verticals
	{-1,-1,-1},{-1,+1,-1},  {+1,-1,-1},{+1,+1,-1},  {+1,-1,+1},{+1,+1,+1},  {-1,-1,+1},{-1,+1,+1},
};

HRESULT CBlockOutline::InitializePrototype(void* pArg)
{
	CResDynamicVIBuffer::DESC desc{};
	desc.iNumVertices = 24;
	desc.iVertexStride = sizeof(_float3);
	desc.ePrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
	desc.vertexDesc = {
		.ByteWidth = desc.iNumVertices * desc.iVertexStride,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};
	desc.vertexSubResource = {
		.pSysMem = cubeEdges
	};

	m_pVIBuffer = CResDynamicVIBuffer::Create();
	if (FAILED(m_pVIBuffer->Load(desc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlockOutline::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

	
	return S_OK;
}

void CBlockOutline::PriorityUpdate(E::_float fTimeDelta)
{
}

void CBlockOutline::Update(E::_float fTimeDelta)
{
	static float fTemp = 0;
	fTemp += fTimeDelta;


}

void CBlockOutline::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	}
	
	GetTransform().Update();
}

HRESULT CBlockOutline::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{

	auto pGameCam = CGameInstance::Get().GetActiveGameCamera();
	if (pGameCam)
	{
		const auto& [rayOrigin, rayDir] = pGameCam->GetRay();
		CVoxelManager3::BLOCK_RAY_RESULT res{};

		if (CGameInstance::Get().VoxelBlockRaycast(rayOrigin, rayDir, 5, res))
		{
			if (res.block)
			{
				if (CBlock3::TYPE::AIR != res.block.value().GetType())
				{
					int32_t wbx, wby, wbz;
					wbx = res.iWorldBlockX;
					wby = res.iWorldBlockY;
					wbz = res.iWorldBlockZ;

					{
						auto pResCBuf = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBlockOutline");
						D3D11_MAPPED_SUBRESOURCE mappedSubResource;
						if (SUCCEEDED(pContext->Map(pResCBuf->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
						{
							E::CB_PER_BLOCKOUTLINE cbPerBlockOutline{};

							auto val = CBlock3::GetOutlineExtents(res.block.value().GetType());
							cbPerBlockOutline.vBlockPos = { wbx + 0.5f + val.first.x, wby + 0.5f + val.first.y, wbz + 0.5f + val.first.z };
							cbPerBlockOutline.vExtents = { val.second.x, val.second.y, val.second.z };
							cbPerBlockOutline.fThickness = 0.015f;
							cbPerBlockOutline.vColor = { 0.f, 0.f, 0.f, 1.f };
							//cbPerBlockOutline.light = res.block->GetLight();
							
							


							memcpy(mappedSubResource.pData, &cbPerBlockOutline, sizeof(cbPerBlockOutline));
							pContext->Unmap(pResCBuf->GetCBuffer().Get(), 0);
						}
						pContext->VSSetConstantBuffers(10, 1, pResCBuf->GetCBuffer().GetAddressOf());
						pContext->PSSetConstantBuffers(10, 1, pResCBuf->GetCBuffer().GetAddressOf());
						pContext->GSSetConstantBuffers(10, 1, pResCBuf->GetCBuffer().GetAddressOf());
					}
				}
			}
		}
		else
		{
			return S_OK;
		}
	}

	


	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_BlockOutline");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_BlockOutline");
	const auto& gs = E::CGameInstance::Get().GetResourceFirst<E::CResGeometryShader>(TAG_RES_GRP_PERMANENT_SHADER, "GS_BlockOutline");
	const auto& viBuffer = m_pVIBuffer;

	pContext->IASetInputLayout(vs->GetInputLayout().Get());
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

	pContext->GSSetShader(gs->GetGeometryShader().Get(), nullptr, 0);

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

	
	pContext->Draw(viBuffer->GetNumVertices(), 0);
	pContext->GSSetShader(nullptr, nullptr, 0);
	return S_OK;
}

UPtr<CBlockOutline> CBlockOutline::Create()
{
	auto pInstance = ToUPtr(new CBlockOutline{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CBlockOutline");
		return nullptr;
	}

	return pInstance;
}

UPtr<CPrototype> CBlockOutline::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CBlockOutline{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CBlockOutline");
		return nullptr;
	}

	return pInstance;
}
