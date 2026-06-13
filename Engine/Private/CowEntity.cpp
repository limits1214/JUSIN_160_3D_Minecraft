#include "CowEntity.h"

#include "Resources.h"
#include "GameInstance.h"

#include "ComEntityModel.h"

NS_USING(Engine)

CCowEntity::CCowEntity()
{
}

CCowEntity::~CCowEntity()
{
}

HRESULT CCowEntity::Initialize(void* pArg)
{
    if (FAILED(CAnimalEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC componentDesc{};
        componentDesc.pGameObject = this;
        componentDesc.viBufferId = { "MC_ENTITY_VIBuffer", "Cow" };
        componentDesc.geometryId = { "MC_ENTITY_GEOMETRY", "Cow" };
        auto pProto = CGameInstance::Get().ClonePrototype("PERMANENT", "Prototype_Component_EntityModel", &componentDesc);
        if (pProto == nullptr)
        {
            return E_FAIL;
        }
        m_pComEntityModel = AddComponent("Com_EntityModel", static_uptr_cast<CComEntityModel>(std::move(pProto)));
    }

    return S_OK;
}

void CCowEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CCowEntity::Update(E::_float fTimeDelta)
{
    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);
}

void CCowEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CCowEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
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

    m_pComEntityModel->BindBoneMatrix(pContext);

    m_pComEntityModel->Render(pContext, ctx);

    return S_OK;
}

UPtr<CCowEntity> CCowEntity::Create()
{
    auto pInstance = ToUPtr(new CCowEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CCowEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CCowEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CCowEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CCowEntity");
        return nullptr;
    }

    return pInstance;
}
