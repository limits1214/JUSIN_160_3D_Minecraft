#include "DropItem.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CollBox.h"

NS_USING(Engine)

CDropItem::CDropItem()
{
}

CDropItem::CDropItem(const CDropItem& rhs)
    : CDropItemObject{ rhs }
{

}

CDropItem::~CDropItem()
{
}

HRESULT CDropItem::Initialize(void* pArg)
{
    if (FAILED(CDropItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    if (auto res = CResDynamicBuffer::Create())
    {
        //UINT ByteWidth;
        //D3D11_USAGE Usage;
        //UINT BindFlags;
        //UINT CPUAccessFlags;
        //UINT MiscFlags;
        //UINT StructureByteStride;
        CResDynamicBuffer::DESC Desc{};
        Desc.desc = {
            .ByteWidth = ((uint32_t)sizeof(VTX_DROP_ITEM_INSTANCED_DATA) * m_iNumElements),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            .MiscFlags = 0,
            .StructureByteStride = 0,
        };

        if (FAILED(res->Load(Desc)))
        {
            return E_FAIL;
        }
        m_pResInstancedBuffer = res;
    }

	return S_OK;
}

void CDropItem::PriorityUpdate(E::_float fTimeDelta)
{
}

void CDropItem::Update(E::_float fTimeDelta)
{
    m_vecInstancedData.clear();

    for (auto& item : m_vecDropItemObjects)
    {
        if (m_bGravity)    VelocityUpdate(item, fTimeDelta);
        if (m_bAnimation)  AnimateTransformUpdate(item, fTimeDelta);

        // 최종 월드행렬
       // _matrix matRot = XMMatrixRotationY(XMConvertToDegrees(item.fBobYRot));
        _matrix matRot = XMMatrixRotationY(item.fBobYRot);
        _matrix matBob = XMMatrixTranslation(0.f, item.fBobYOffset, 0.f);
        _matrix matWorld = XMMatrixTranslation(item.vPos.x, item.vPos.y, item.vPos.z);
        XMStoreFloat4x4(&item.matWorld, matRot * matBob * matWorld);

        item.boxCollider->Transform(matWorld);
        E::CGameInstance::Get().AddColliderGroup("Coll_DropItemObject", item.boxCollider.get());


       
        
       

        VTX_DROP_ITEM_INSTANCED_DATA inst{};

        int32_t blockX = static_cast<int32_t>(std::floor(item.vPos.x));
        int32_t blockY = static_cast<int32_t>(std::floor(item.vPos.y));
        int32_t blockZ = static_cast<int32_t>(std::floor(item.vPos.z));
        if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
        {
            inst.light = optCurrBlock->GetLight();
        }
        

        inst.matWorld = item.matWorld;
        inst.texIndex = item.texIndexs.front();
        m_vecInstancedData.push_back(inst);

    }


}

void CDropItem::LateUpdate(E::_float fTimeDelta)
{

    E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
   
}

HRESULT CDropItem::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{

	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DropItem");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DropItem");
	//const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>("MC_ITEM_VIBuffer", "CubeItemDirt");
	const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>(m_viBufferID.first, m_viBufferID.second);

    pContext->IASetInputLayout(vs->GetInputLayout().Get());
    pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

    ID3D11Buffer* vertexBuffers[] = {
                viBuffer->GetVertexBuffer().Get(),
                m_pResInstancedBuffer->GetBuffer().Get()
    };
    uint32_t strides[] = {
        viBuffer->GetVertexStride(),
        (uint32_t)sizeof(VTX_DROP_ITEM_INSTANCED_DATA),
    };
    uint32_t offsets[] = {
        0,
        0,
    };
    pContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
    pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
    pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

    {
        auto pCbPerObject = m_pResInstancedBuffer;
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;

        if (SUCCEEDED(pContext->Map(pCbPerObject->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        {
            std::memcpy(mappedSubResource.pData, m_vecInstancedData.data(), sizeof(VTX_DROP_ITEM_INSTANCED_DATA) * m_vecInstancedData.size());
            pContext->Unmap(pCbPerObject->GetBuffer().Get(), 0);
        }
    }

	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}

    //pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
    pContext->DrawIndexedInstanced((UINT)viBuffer->GetNumIndices(), (UINT)m_vecInstancedData.size(), 0, 0, 0);
	
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
