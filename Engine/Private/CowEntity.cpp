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
        //m_pComTransform = AddComponent("Com_Transform", static_uptr_cast<CTransform>(std::move(pProto)));
        //m_pComTransform = GetComponent<CTransform>("Com_Transform");
    }

    //GetTransform().SetRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
    return S_OK;
}

void CCowEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CCowEntity::Update(E::_float fTimeDelta)
{
    static float fTmpElapsed = 0.f;
    fTmpElapsed += fTimeDelta * 0.1f;

    int boneIdx = 0;
    for (auto& bone : m_Bones)
    {
        XMMATRIX local = XMMatrixIdentity();

        //if (bone.name == "leg0" || bone.name == "leg3") {
        //    float rotX = XMConvertToRadians(cosf(fTmpElapsed * 38.17f) * 80.f);
        //    local = XMMatrixRotationX(rotX);
        //}
        //else if (bone.name == "leg1" || bone.name == "leg2") {
        //    float rotX = XMConvertToRadians(cosf(fTmpElapsed * 38.17f) * -80.f);
        //    local = XMMatrixRotationX(rotX);
        //}
        //else if (bone.name == "head")
        //{
        //    float rotY = XMConvertToRadians(cosf(fTmpElapsed * 38.17f) * -10.f);
        //    local = XMMatrixRotationY(rotY);
        //}

        XMVECTOR pivot = XMLoadFloat3(&bone.pivot);

        XMMATRIX boneMatrix =
            XMMatrixTranslationFromVector(-pivot)  // 1. pivot을 원점으로
            * local                                  // 2. 회전
            * XMMatrixTranslationFromVector(pivot);  // 3. pivot 위치로 복원

        XMStoreFloat4x4(&m_boneMatrices[boneIdx], boneMatrix);
        ++boneIdx;
    }

    for (uint32_t i = 0; i < 10; ++i)
    {
        XMMATRIX local = XMMatrixIdentity();
        XMStoreFloat4x4(&m_boneMatrices[i], local);
    }

    //{
    //    auto pContext = CGameInstance::Get().GetGraphicDeviceContext();
    //    auto pCbPerBone = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBone");
    //    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    //    if (SUCCEEDED(pContext->Map(pCbPerBone->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
    //    {
    //        E::CB_PER_BONE cbPerBone{};
    //        memcpy(cbPerBone.matBone, m_boneMatrices, sizeof(_float4x4) * 64);
    //        memcpy(mappedSubResource.pData, &cbPerBone, sizeof(cbPerBone));
    //        pContext->Unmap(pCbPerBone->GetCBuffer().Get(), 0);
    //    }
    //    pContext->VSSetConstantBuffers(4, 1, pCbPerBone->GetCBuffer().GetAddressOf());
    //}


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

    m_pComEntityModel->BindBoneMatrix();

    m_pComEntityModel->Render(pContext, ctx);


    //const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Entity");
    //const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Entity");

    //{
    //    const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResEnttVIBuffer>("MC_ENTITY_VIBuffer", "Cow");
    //    pContext->IASetInputLayout(vs->GetInputLayout().Get());
    //    pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    //    pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

    //    ID3D11Buffer* vertexBuffers[] = {
    //            viBuffer->GetVertexBuffer().Get()
    //    };
    //    uint32_t strides[] = {
    //        viBuffer->GetVertexStride()
    //    };
    //    uint32_t offsets[] = {
    //        0
    //    };
    //    pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    //    pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
    //    pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

    //    {
    //        const auto& texArray = E::CGameInstance::GetConst().GetResourceFirst<E::CResTexture2DArray>("MC_ENTITY_TEX_64_64", "TEXTURE_ARRAY");
    //        pContext->PSSetShaderResources(8, 1, texArray->GetSRV().GetAddressOf());

    //        const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
    //        pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
    //    }
    //    if (0)
    //    {
    //        const auto& rasterizer = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, "RS_SOLID_BACKCULL_DEPTHBIAS");
    //        pContext->RSSetState(rasterizer->GetRasterizerState().Get());
    //    }
    //    pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
    //}

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
