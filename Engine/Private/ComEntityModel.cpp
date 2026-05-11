#include "ComEntityModel.h"
#include "GameInstance.h"
#include "Resources.h"

NS_USING(Engine)

void CComEntityModel::UpdateGUI()
{
}

CComEntityModel::CComEntityModel()
{
}

CComEntityModel::~CComEntityModel()
{
}

HRESULT CComEntityModel::Initialize(void* pArg)
{
    if (FAILED(CComponent::Initialize(pArg)))
    {
        return E_FAIL;
    }

    auto pDesc = static_cast<DESC*>(pArg);
    const auto& [grpTag, resTag] = pDesc->viBufferId;
    m_pResEntityViBuffer = CGameInstance::Get().GetResourceFirst<CResEnttVIBuffer>(grpTag, resTag);


    auto resources = CGameInstance::Get().GetResourceFirst<CResEnttGeo>(pDesc->geometryId.first, pDesc->geometryId.second);
    const auto& geometry = resources->GetGeometry();

    for (const auto& bone : geometry.bones)
    {
        auto b = CEntityModelBone{ bone.name };
        b.SetPivot(bone.pivot);
        b.SetParentName(bone.parent); // 추가
        m_Bones.push_back(b);
    }

	return S_OK;
}

void CComEntityModel::UpdateBoneMatrix(_float fTimeDelta)
{
    m_fElapsed += fTimeDelta * 0.1f;

    // 1. 로컬 행렬 세팅
    for (auto& bone : m_Bones)
    {
        XMMATRIX local = XMMatrixIdentity();

        if (bone.GetName() == "leg0" || bone.GetName() == "leg3")
        {
            float rotX = XMConvertToRadians(cosf(m_fElapsed * 38.17f) * 80.f);
            local = XMMatrixRotationX(rotX);
        }
        else if (bone.GetName() == "leg1" || bone.GetName() == "leg2")
        {
            float rotX = XMConvertToRadians(cosf(m_fElapsed * 38.17f) * -80.f);
            local = XMMatrixRotationX(rotX);
        }

        bone.UpdateTransformationMatrix(local);
    }

    // 2. 부모 → 자식 순으로 combined 계산
    for (uint32_t i = 0; i < m_Bones.size(); ++i)
    {
        const std::string& parentName = m_Bones[i].GetParentName();

        if (parentName.empty())
        {
            m_Bones[i].UpdateCombinedMatrix(nullptr);
        }
        else
        {
            const _float4x4* pParentCombined = nullptr;
            for (uint32_t j = 0; j < i; ++j)
            {
                if (m_Bones[j].GetName() == parentName)
                {
                    pParentCombined = m_Bones[j].GetCombinedTransformationMatrix();
                    break;
                }
            }
            m_Bones[i].UpdateCombinedMatrix(pParentCombined);
        }

        memcpy(&m_cbPerBone.matBone[i], m_Bones[i].GetCombinedTransformationMatrix(), sizeof(_float4x4));
    }
}

void CComEntityModel::BindBoneMatrix() const
{
    auto pContext = CGameInstance::Get().GetGraphicDeviceContext();
    auto pCbPerBone = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBone");
    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    if (SUCCEEDED(pContext->Map(pCbPerBone->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
    {
        memcpy(mappedSubResource.pData, &m_cbPerBone, sizeof(m_cbPerBone));
        pContext->Unmap(pCbPerBone->GetCBuffer().Get(), 0);
    }
    pContext->VSSetConstantBuffers(4, 1, pCbPerBone->GetCBuffer().GetAddressOf());
}

HRESULT CComEntityModel::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx, uint32_t iMeshIdx)
{
    const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Entity");
    const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Entity");

    {
        const auto& viBuffer = m_pResEntityViBuffer;
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

        //{
        //    auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
        //    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        //    if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        //    {

        //        E::CB_PER_OBJECT cbPerObject{};
        //        cbPerObject.matWorld = *GetTransform().GetWorldMatrix();
        //        XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedWorldMatrix() * ctx.matViewProj);

        //        memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
        //        pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
        //    }
        //    pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
        //    pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
        //}
        {
            //const auto& texArray = E::CGameInstance::GetConst().GetResourceFirst<E::CResTexture2DArray>("MC_ENTITY_TEX_64_64", "TEXTURE_ARRAY");
            //pContext->PSSetShaderResources(8, 1, texArray->GetSRV().GetAddressOf());

            const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
            pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
        }
        if (1)
        {
            const auto& rasterizer = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL);
            pContext->RSSetState(rasterizer->GetRasterizerState().Get());
        }
        pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
    }
	return S_OK;
}

UPtr<CComEntityModel> CComEntityModel::Create()
{
    auto pInstance = ToUPtr(new CComEntityModel{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CComEntityModel");
        return nullptr;
    }
    return pInstance;
}

UPtr<CPrototype> CComEntityModel::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CComEntityModel{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CComEntityModel");
        return nullptr;
    }
    return pInstance;
}