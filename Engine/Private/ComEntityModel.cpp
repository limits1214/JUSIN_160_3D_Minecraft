#include "ComEntityModel.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CameraObject.h"
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

    m_pResEntityViBuffer = CGameInstance::Get().GetResourceFirst<CResEnttVIBuffer>(pDesc->viBufferId.first, pDesc->viBufferId.second);

    auto resources = CGameInstance::Get().GetResourceFirst<CResEnttGeo>(pDesc->geometryId.first, pDesc->geometryId.second);
    const auto& geometry = resources->GetGeometry();

    for (const auto& bone : geometry.bones)
    {
        auto b = CEntityModelBone{ bone.name };
        b.SetPivot(bone.pivot);
        b.SetParentName(bone.parent);
        m_Bones.push_back(b);
    }

    for (uint32_t i = 0; i < m_Bones.size(); ++i)
    {
        const std::string& parentName = m_Bones[i].GetParentName();
        if (!parentName.empty())
        {
            for (uint32_t j = 0; j < i; ++j)
            {
                if (m_Bones[j].GetName() == parentName)
                {
                    m_Bones[i].SetParentIndex(j);
                    break;
                }
            }
        }
    }
	return S_OK;
}

void CComEntityModel::UpdateBoneMatrix(_float fTimeDelta)
{
    m_fElapsed += fTimeDelta * 0.1f;

    // TODO 애니메이션 으로 뺄 예정
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

    if (auto cam = CGameInstance::Get().GetCameraObject("GAME"))
    {
        CEntityModelBone* headBone{};
        for (auto& bone : m_Bones)
        {
            if (bone.GetName() == "head")
            {
                headBone = &bone;
                break;
            }
        }

        // 카메라 → 엔티티 방향 벡터
        const _float3& camPos = cam->GetTransform().GetPosition();
        const _float3& entityPos = GetGameObject()->GetTransform().GetPosition();

        const _float3& headPivot = headBone->GetPivot(); // 로컬 공간 기준
        XMVECTOR vHeadWorld = XMLoadFloat3(&entityPos) + XMLoadFloat3(&headPivot);

        XMVECTOR vCam = XMLoadFloat3(&camPos);
        XMVECTOR vEntity = XMLoadFloat3(&entityPos);

        XMVECTOR vDir = XMVector3Normalize(vCam - vHeadWorld); // 엔티티 → 카메라

        {
            _vector vLook = vDir;

            _vector vRight = XMVector3Normalize(
                XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

            _vector vUp = XMVector3Cross(vLook, vRight);

            //_float3 scale = GetScale();
            //SetState(STATE::RIGHT, XMVector3Normalize(vRight) * scale.x);
            //SetState(STATE::UP, XMVector3Normalize(vUp) * scale.y);
            //SetState(STATE::LOOK, XMVector3Normalize(vLook) * scale.z);

            _matrix matRot = XMMatrixIdentity();
            matRot.r[0] = vRight;
            matRot.r[1] = vUp;
            matRot.r[2] = vLook;

            if (headBone)
                headBone->UpdateTransformationMatrix(matRot);
        }
        

        // pitch, yaw 계산
        //float pitch = asinf(XMVectorGetY(vDir));
        //float yaw = atan2f(XMVectorGetX(vDir), XMVectorGetZ(vDir));

        //// 엔티티 yaw 기준으로 상대화 (relative_to = entity)
        //const _float3& entityRot = GetGameObject()->GetTransform().GetRotationEuler();
        //yaw -= XMConvertToRadians(entityRot.y);

        //if (headBone)
        //    headBone->UpdateTransformationMatrix(
        //        XMMatrixRotationX(pitch) * XMMatrixRotationY(yaw));
    }

    ;
    //if (auto cam = CGameInstance::Get().GetCameraObject("GAME"))
    //{
    //    const _float3& camRot = cam->GetTransform().GetRotationEuler();

    //    // relative_to.rotation = "entity" 이므로 엔티티 yaw를 빼줘야 함
    //    
    //    const _float3& entityRot = GetGameObject()->GetTransform().GetRotationEuler();

    //    float rotX = XMConvertToRadians(camRot.x);               // target_x_rotation
    //    float rotY = XMConvertToRadians(camRot.y - entityRot.y); // target_y_rotation - entity yaw

    //    auto* headBone = FindBone("head");
    //    if (headBone)
    //        headBone->UpdateTransformationMatrix(
    //            XMMatrixRotationX(rotX) * XMMatrixRotationY(rotY));
    //}


    for (uint32_t i = 0; i < m_Bones.size(); ++i)
    {
        auto& bone = m_Bones[i];

        int32_t parentIdx = bone.GetParentIndex(); // -1이면 루트
        bone.UpdateCombinedMatrix(parentIdx >= 0
            ? m_Bones[parentIdx].GetCombinedTransformationMatrix()
            : nullptr);

        memcpy(&m_cbPerBone.matBone[i], bone.GetCombinedTransformationMatrix(), sizeof(_float4x4));
    }

    //for (uint32_t i = 0; i < m_Bones.size(); ++i)
    //{
    //    if (i == 0)
    //    {
    //        m_Bones[i].UpdateCombinedMatrix(nullptr);
    //    }
    //    else
    //    {
    //        m_Bones[i].UpdateCombinedMatrix(m_Bones[i-1].GetCombinedTransformationMatrix());
    //    }

    //    memcpy(&m_cbPerBone.matBone[i], m_Bones[i].GetCombinedTransformationMatrix(), sizeof(_float4x4));
    //}

    //// 2. 부모 → 자식 순으로 combined 계산
    //for (uint32_t i = 0; i < m_Bones.size(); ++i)
    //{
    //    const std::string& parentName = m_Bones[i].GetParentName();
    //    if (parentName.empty())
    //    {
    //        m_Bones[i].UpdateCombinedMatrix(nullptr);
    //    }
    //    else
    //    {
    //        const _float4x4* pParentCombined = nullptr;
    //        for (uint32_t j = 0; j < i; ++j)
    //        {
    //            if (m_Bones[j].GetName() == parentName)
    //            {
    //                pParentCombined = m_Bones[j].GetCombinedTransformationMatrix();
    //                break;
    //            }
    //        }
    //        m_Bones[i].UpdateCombinedMatrix(pParentCombined);
    //    }
    //    memcpy(&m_cbPerBone.matBone[i], m_Bones[i].GetCombinedTransformationMatrix(), sizeof(_float4x4));
    //}
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
        
        {
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