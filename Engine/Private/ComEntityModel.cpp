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
        m_BonesLookup.emplace(bone.name, m_Bones.size());
        m_Bones.push_back(b);
    }

    for (uint32_t i = 0; i < m_Bones.size(); ++i)
    {
        const std::string& parentName = m_Bones[i].GetParentName();
        if (!parentName.empty())
        {
            auto it = m_BonesLookup.find(parentName);
            if (it != m_BonesLookup.end())
                m_Bones[i].SetParentIndex((uint32_t)it->second);
        }
    }


    for (uint32_t i = 0; i < m_Bones.size(); ++i)
    {
        auto& bone = m_Bones[i];

        int32_t parentIdx = bone.GetParentIndex(); // -1이면 루트

        if (parentIdx >= 0)
        {
            _float3 localPivot;
            XMStoreFloat3(&localPivot, XMLoadFloat3(&bone.GetPivot()) - XMLoadFloat3(&m_Bones[parentIdx].GetPivot()));
            bone.SetLocalPivot(localPivot);
        }
        else
        {
            bone.SetLocalPivot(bone.GetPivot());
        }
        

        bone.UpdateTransformationMatrix(XMMatrixIdentity());
    }



    //for (uint32_t i = 0; i < m_Bones.size(); ++i)
    //{
    //    auto& bone = m_Bones[i];
    //    bone.UpdateTransformationMatrix(XMMatrixIdentity());
    //}
    // Initialize() 끝부분에 추가
    for (uint32_t i = 0; i < m_Bones.size(); ++i)
    {
        auto& bone = m_Bones[i];
        int32_t parentIdx = bone.GetParentIndex();

        // Bind Pose Combined Matrix 계산
        bone.UpdateCombinedMatrix(parentIdx >= 0 ?
            m_Bones[parentIdx].GetCombinedTransformationMatrix() : nullptr);

        // Offset Matrix 계산해서 저장 (중요!)
        XMMATRIX bindGlobal = XMLoadFloat4x4(bone.GetCombinedTransformationMatrix());
        XMMATRIX offset = XMMatrixInverse(nullptr, bindGlobal);
        //
        _float4x4 tmp;
        XMStoreFloat4x4(&tmp, offset);
        bone.SetOffsetMatrix(tmp);
    }
	return S_OK;
}

void CComEntityModel::UpdateBoneMatrix(_float fTimeDelta)
{
    m_fElapsed += fTimeDelta * 0.1f;

    // TODO 애니메이션 으로 뺄 예정
    



    if (false)
    {
        for (auto& bone : m_Bones)
        {
            XMMATRIX local = XMMatrixIdentity();
            float flap = (sinf(m_fElapsed * 15.0f) * 0.5f + 0.5f) * 45.0f; // 0 ~ 45도
            if (bone.GetName() == "body")
            {
                // "rotation": ["-this", 0.0, 0.0]
                // body의 현재 X 회전 상쇄 → 일단 0으로 고정 (지형 기울기 없으면 identity)
                local = XMMatrixIdentity();
            }
            else if (bone.GetName() == "wing0")
            {
                float rotZ = XMConvertToRadians(flap);
                local = XMMatrixRotationZ(rotZ);
            }
            else if (bone.GetName() == "wing1")
            {
                // 0도일 때 몸에 붙고, -45도일 때 위로 펼쳐짐 (왼쪽은 음수가 바깥쪽일 경우)
     // 만약 -45도가 몸 안쪽이라면 여기도 그냥 flap을 써야 합니다.
                float rotZ = XMConvertToRadians(-flap);
                local = XMMatrixRotationZ(rotZ);
            }
            else  if (bone.GetName() == "leg0" || bone.GetName() == "leg3")
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
   
        if (auto cam = CGameInstance::Get().GetActiveGameCamera())
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
            if (headBone)
            {
                // 카메라 → 엔티티 방향 벡터
                const _float3& camPos = cam->GetTransform().GetPosition();

                auto entityWorld = GetGameObject()->GetTransform().GetLoadedWorldMatrix();
                //auto a = entityWorld.r[0];
                //XMLoadFloat3(entityWorld->m[0])
                //auto entityPos = GetGameObject()->GetTransform().GetPosition();

                //_matrix mat = XMMatrixIdentity();
                //mat.r[0] = XMVector3Normalize(entityWorld.r[0]);
                //mat.r[1] = XMVector3Normalize(entityWorld.r[1]);
                //mat.r[2] = XMVector3Normalize(entityWorld.r[2]);
                //mat.r[3] = entityWorld.r[3];
                //mat.r[3] = XMVectorSet(entityPos.x, entityPos.y, entityPos.z, 1.f);


                XMVECTOR vHeadWorld = XMVector3TransformCoord(XMLoadFloat3(&headBone->GetPivot()),
                    entityWorld);

                XMVECTOR vCam = XMLoadFloat3(&camPos);

                XMVECTOR vDir = XMVector3Normalize(vCam - vHeadWorld); // 엔티티 → 카메라

                {
                    _vector vLook = vDir;

                    _vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
                    //_vector fallback = XMVectorSet(1.f, 0.f, 0.f, 0.f);
                    //float   dot = XMVectorGetX(XMVector3Dot(vLook, worldUp));
                    //_vector refUp = (fabsf(dot) > 0.99f) ? fallback : worldUp;

                    _vector vRight = XMVector3Normalize(XMVector3Cross(worldUp, vLook));
                    _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

                    //_float3 scale = GetGameObject()->GetTransform().GetScale();
                    //SetState(STATE::RIGHT, XMVector3Normalize(vRight) * scale.x);
                    //SetState(STATE::UP, XMVector3Normalize(vUp) * scale.y);
                    //SetState(STATE::LOOK, XMVector3Normalize(vLook) * scale.z);

                    _matrix matRot = XMMatrixIdentity();
                    matRot.r[0] = XMVectorSetW(vRight, 0.f);
                    matRot.r[1] = XMVectorSetW(vUp, 0.f);
                    matRot.r[2] = XMVectorSetW(vLook, 0.f);


                    headBone->UpdateTransformationMatrix(matRot);
                }
            }

        }
    }

    
    for (uint32_t i = 0; i < m_Bones.size(); ++i)
    {
        auto& bone = m_Bones[i];

        bone.BuildTransformMatrix();

        int32_t parentIdx = bone.GetParentIndex(); // -1이면 루트
        bone.UpdateCombinedMatrix(parentIdx >= 0
            ? m_Bones[parentIdx].GetCombinedTransformationMatrix()
            : nullptr);

        auto offsetMatrix = XMLoadFloat4x4(bone.GetOffsetMatrix());
        XMMATRIX offseted = offsetMatrix * XMLoadFloat4x4(bone.GetCombinedTransformationMatrix());
        XMStoreFloat4x4(&m_cbPerBone.matBone[i], offseted);
    }
}

void CComEntityModel::BindBoneMatrix(ID3D11DeviceContext* pContext) const
{
    auto pCbPerBone = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBone");
    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    if (SUCCEEDED(pContext->Map(pCbPerBone->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
    {
        memcpy(mappedSubResource.pData, &m_cbPerBone.matBone[0], sizeof(_float4x4) * m_Bones.size());
        pContext->Unmap(pCbPerBone->GetCBuffer().Get(), 0);
    }
    pContext->VSSetConstantBuffers(4, 1, pCbPerBone->GetCBuffer().GetAddressOf());
}

void CComEntityModel::ResetBonesChannel()
{
    for (auto& bone : m_Bones)
    {
        bone.ResetChannels();
    }
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