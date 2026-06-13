#include "PigEntity.h"

#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"
#include "ComAnimator.h"
#include "ComConstantBuffer.h"
#include "CameraObject.h"

NS_USING(Engine)

CPigEntity::CPigEntity()
{
}

CPigEntity::~CPigEntity()
{
}

HRESULT CPigEntity::Initialize(void* pArg)
{
    if (FAILED(CAnimalEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Pig" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Pig" };
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

void CPigEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CPigEntity::Update(E::_float fTimeDelta)
{
    m_pComAnimator->QuadrupedWalk(fTimeDelta * 0.1f);

    if (auto pPlayer = CGameInstance::Get().GetGameObjectByHandle(m_hPlayer))
    {
        m_pComAnimator->HeadLookAt(fTimeDelta, pPlayer->GetTransform().GetPosition());
    }
    

    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);
}

void CPigEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CPigEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    {
        E::CB_PER_OBJECT cbPerObject{};
        cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
        XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);
        if (FAILED(m_pComCBufferPerObject->MapDiscard(pContext, &cbPerObject, sizeof(cbPerObject))))
        {
            return E_FAIL;
        }
        pContext->VSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
        pContext->PSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
    }

    m_pComEntityModel->BindBoneMatrix(pContext);

    m_pComEntityModel->Render(pContext, ctx);
    return S_OK;
}

UPtr<CPigEntity> CPigEntity::Create()
{
    auto pInstance = ToUPtr(new CPigEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CPigEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CPigEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CPigEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPigEntity");
        return nullptr;
    }

    return pInstance;
}
