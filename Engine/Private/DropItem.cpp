#include "DropItem.h"
#include "GameInstance.h"
#include "Resources.h"

NS_USING(Engine)

CDropItem::CDropItem()
{
}

CDropItem::~CDropItem()
{
}

HRESULT CDropItem::Initialize(void* pArg)
{
    if (FAILED(CItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

	return S_OK;
}

void CDropItem::PriorityUpdate(E::_float fTimeDelta)
{
}

void CDropItem::Update(E::_float fTimeDelta)
{
}

void CDropItem::LateUpdate(E::_float fTimeDelta)
{
    E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CDropItem::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	{
		auto pResCBuf = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerQuadItemAnim");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pResCBuf->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
			E::CB_PER_QUADITEM_ANIM cbPerQuadItemAnim{};
			memcpy(mappedSubResource.pData, &cbPerQuadItemAnim, sizeof(cbPerQuadItemAnim));
			pContext->Unmap(pResCBuf->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(5, 1, pResCBuf->GetCBuffer().GetAddressOf());
	}
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Item");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Item");
	const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResExtrudedItemVIBuffer>("MC_ITEM_VIBuffer", "MuttonRaw");

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
			cbPerObject.matWorld = *GetTransform().GetWorldMatrix();
			XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedWorldMatrix() * ctx.matViewProj);

			memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
			pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
		pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
	}

	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

UPtr<CDropItem> CDropItem::Create()
{
    auto pInstance = ToUPtr(new CDropItem{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CDropItem");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CDropItem::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CDropItem{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CDropItem");
        return nullptr;
    }

    return pInstance;
}
