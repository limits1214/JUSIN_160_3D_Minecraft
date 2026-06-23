#include "HandHeldItem.h"
#include "GameInstance.h"
#include "Resources.h"

#include "ComConstantBuffer.h"
#include "ResDynamicBuffer.h"
//#include "DropItemObject.h"

#include "PlayerEntity.h"

NS_USING(Engine)

CHandHeldItem::CHandHeldItem()
{
}

CHandHeldItem::~CHandHeldItem()
{
}

HRESULT CHandHeldItem::Initialize(void* pArg)
{
    if (FAILED(CHandHeldItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    //{
    //    CComConstantBuffer::DESC Desc{};
    //    Desc.cBufferId = { TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT };
    //    if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_ConstantBuffer", "ComCBufferPerObject", &Desc, &m_pComCBufferPerObject)))
    //    {
    //        return E_FAIL;
    //    };
    //}

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
        m_pResItemInstancedBuffer = res;
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
            .ByteWidth = ((uint32_t)sizeof(VTX_DROP_BLOCK_INSTANCED_DATA) * m_iNumElements),
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
        m_pResBlockInstancedBuffer = res;
    }

    //GetTransform().SetPosition(XMVectorSet(0.1f, -0.15f, 0.05f, 1.f));
    //fpsArmObj->GetTransform().AddRotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), -90.f);
    //fpsArmObj->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 30.f);

    //GetTransform().AddQuaternion(XMQuaternionRotationRollPitchYaw(
    //    XMConvertToRadians(-90.f),
    //    XMConvertToRadians(30.f),
    //    XMConvertToRadians(0.f)
    //));

	return S_OK;
}

void CHandHeldItem::PriorityUpdate(E::_float fTimeDelta)
{
}

void CHandHeldItem::Update(E::_float fTimeDelta)
{
   
}

void CHandHeldItem::LateUpdate(E::_float fTimeDelta)
{
    if (m_bRender)
    {
        E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
    }
   
    //if (auto pPlayer = CGameInstance::Get().GetGameObjectByHandleT<CPlayerEntity>(m_hPlayerHandle))
    //{
    //    //GetTransform().SetParentWorldMatrix(*pPlayer->GetTransform().GetWorldMatrix());
    //}
    GetTransform().Update();
}

HRESULT CHandHeldItem::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    if (!m_ItemInfo)
    {
        return S_OK;
    }
    if (m_ItemInfo->block)
    {
        const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DropBlock");
        const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DropBlock");
        //const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>("MC_ITEM_VIBuffer", "CubeItemDirt");
        const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>(m_viBufferID.first, m_viBufferID.second);

        pContext->IASetInputLayout(vs->GetInputLayout().Get());
        pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
        pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

        ID3D11Buffer* vertexBuffers[] = {
                    viBuffer->GetVertexBuffer().Get(),
                    m_pResBlockInstancedBuffer->GetBuffer().Get()
        };
        uint32_t strides[] = {
            viBuffer->GetVertexStride(),
            (uint32_t)sizeof(VTX_DROP_BLOCK_INSTANCED_DATA),
        };
        uint32_t offsets[] = {
            0,
            0,
        };
        pContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
        pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
        pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

        {
            {
                VTX_DROP_BLOCK_INSTANCED_DATA inst{};
                m_vecBlockInstancedData.clear();
                inst.matWorld = *GetTransform().GetCombinedWorldMatrix();
                //inst.texIndex = CItemObject::GetPackedTexIdByType(m_ItemInfo.eItemType);

                std::vector<uint32_t> texs{};
                for (uint32_t i = 0; i < ETOUI(FACE_DIR::END); ++i)
                {
                    texs.push_back(PackTexId(9, ETOUI(CBlock3::GetTexType(m_ItemInfo->block->GetType(), static_cast<FACE_DIR>(i)))));
                }
                memcpy(inst.texIndexs, texs.data(), sizeof(uint32_t) * texs.size());

                inst.light = m_iLight;

                m_vecBlockInstancedData.push_back(inst);
            }
            auto pCbPerObject = m_pResBlockInstancedBuffer;
            D3D11_MAPPED_SUBRESOURCE mappedSubResource;

            if (SUCCEEDED(pContext->Map(pCbPerObject->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
            {
                std::memcpy(mappedSubResource.pData, m_vecBlockInstancedData.data(), sizeof(VTX_DROP_BLOCK_INSTANCED_DATA) * m_vecBlockInstancedData.size());
                pContext->Unmap(pCbPerObject->GetBuffer().Get(), 0);
            }
        }

        {
            const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
            pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
        }

        //pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
        pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);

        return S_OK;
    }
    else
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
                    m_pResItemInstancedBuffer->GetBuffer().Get()
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
            {
                VTX_DROP_ITEM_INSTANCED_DATA inst{};
                m_vecItemInstancedData.clear();
                inst.matWorld = *GetTransform().GetCombinedWorldMatrix();
                inst.texIndex = CItemObject::GetPackedTexIdByType(m_ItemInfo->eItemType);
                
                inst.light = m_iLight;
                m_vecItemInstancedData.push_back(inst);
            }
            auto pCbPerObject = m_pResItemInstancedBuffer;
            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                
            if (SUCCEEDED(pContext->Map(pCbPerObject->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
            {
                std::memcpy(mappedSubResource.pData, m_vecItemInstancedData.data(), sizeof(VTX_DROP_ITEM_INSTANCED_DATA) * m_vecItemInstancedData.size());
                pContext->Unmap(pCbPerObject->GetBuffer().Get(), 0);
            }
        }

        {
            const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
            pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
        }

        //pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
        pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);

        return S_OK;
    }

    
}

UPtr<CHandHeldItem> CHandHeldItem::Create()
{
    auto pInstance = ToUPtr(new CHandHeldItem{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CHandHeldItem");
        return nullptr;
    }

    return pInstance;
}


UPtr<CPrototype> CHandHeldItem::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CHandHeldItem{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CHandHeldItem");
        return nullptr;
    }

    return pInstance;
}