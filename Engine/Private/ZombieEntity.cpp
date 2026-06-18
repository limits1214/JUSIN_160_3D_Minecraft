#include "ZombieEntity.h"

#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"
#include "ComAnimator.h"
#include "ComConstantBuffer.h"
#include "CameraObject.h"

NS_USING(Engine)





CZombieEntity::CZombieEntity()
{
}

CZombieEntity::~CZombieEntity()
{
}

HRESULT CZombieEntity::Initialize(void* pArg)
{
    if (FAILED(CMonsterEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Zombie" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Zombie" };
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_EntityModel", "Com_EntityModel", &Desc, &m_pComEntityModel)))
        {
            return E_FAIL;
        };
    }

    {
        CComAnimator::DESC Desc{};
        Desc.pComEntityModel = m_pComEntityModel;
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Animator", "Com_Animator", &Desc, &m_pComAnimator)))
        {
            return E_FAIL;
        };
    }

    {
        CComConstantBuffer::DESC Desc{};
        Desc.cBufferId = { TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT };
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_ConstantBuffer", "ComCBufferPerObject", &Desc, &m_pComCBufferPerObject)))
        {
            return E_FAIL;
        };
    }

    return S_OK;
}

void CZombieEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CZombieEntity::Update(E::_float fTimeDelta)
{
    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);
}

void CZombieEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CZombieEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
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

UPtr<CZombieEntity> CZombieEntity::Create()
{
    auto pInstance = ToUPtr(new CZombieEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CZombieEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CZombieEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CZombieEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CZombieEntity");
        return nullptr;
    }

    return pInstance;
}
