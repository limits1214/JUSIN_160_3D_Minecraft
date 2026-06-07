#include "DropBlock.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CollBox.h"

NS_USING(Engine)

CDropBlock::CDropBlock()
{
}

CDropBlock::CDropBlock(const CDropBlock& rhs)
    : CItemObject{ rhs }
{

}

CDropBlock::~CDropBlock()
{
}

HRESULT CDropBlock::Initialize(void* pArg)
{
    auto pDesc = static_cast<DESC*>(pArg);

    m_viBufferID = pDesc->viBufferId;
    if (FAILED(CItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    //m_pCenterCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.49f, 0.25f });

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
        m_pResInstancedBuffer = res;
    }

    return S_OK;
}

void CDropBlock::PriorityUpdate(E::_float fTimeDelta)
{
}

void CDropBlock::Update(E::_float fTimeDelta)
{
    m_vecInstancedData.clear();

    for (auto& item : m_vecDropItems)
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
        E::CGameInstance::Get().AddColliderGroup("Coll_DropBlockCenter", item.boxCollider.get());

        VTX_DROP_BLOCK_INSTANCED_DATA inst{};
        inst.matWorld = item.matWorld;
        //memcpy(&inst.texIndexs, item.texIndexs.data(), item.texIndexs.size());
        memcpy(&inst.texIndexs, item.texIndexs.data(), sizeof(uint32_t) * item.texIndexs.size());
        m_vecInstancedData.push_back(inst);
    }
}

void CDropBlock::LateUpdate(E::_float fTimeDelta)
{
    // E::CGameInstance::Get().AddColliderGroup("Coll_DropItemCenter", m_pCenterCollider.get());
    // m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

    E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
    //GetTransform().SetParentWorldMatrix(*m_pComAnimateTransform->GetWorldMatrix());
    //GetTransform().Update();

    //_matrix tmpWorld;
    //if (m_bAnimation)
    //{
    //    tmpWorld = XMLoadFloat4x4(m_pComAnimateTransform->GetCombinedWorldMatrix()) * XMLoadFloat4x4(GetTransform().GetCombinedWorldMatrix());
    //}
    //else
    //{
    //    tmpWorld = XMLoadFloat4x4(GetTransform().GetCombinedWorldMatrix());
    //}
}

HRESULT CDropBlock::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    //{
    //	auto pResCBuf = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerQuadItemAnim");
    //	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    //	if (SUCCEEDED(pContext->Map(pResCBuf->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
    //	{
    //		E::CB_PER_QUADITEM_ANIM cbPerQuadItemAnim{};
    //		memcpy(mappedSubResource.pData, &cbPerQuadItemAnim, sizeof(cbPerQuadItemAnim));
    //		pContext->Unmap(pResCBuf->GetCBuffer().Get(), 0);
    //	}
    //	pContext->VSSetConstantBuffers(5, 1, pResCBuf->GetCBuffer().GetAddressOf());
    //}

    const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DropBlock");
    const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DropBlock");
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
        auto pCbPerObject = m_pResInstancedBuffer;
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;

        if (SUCCEEDED(pContext->Map(pCbPerObject->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        {
            std::memcpy(mappedSubResource.pData, m_vecInstancedData.data(), sizeof(VTX_DROP_BLOCK_INSTANCED_DATA) * m_vecInstancedData.size());
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

void CDropBlock::AnimateTransformUpdate(InstancedDropBlockDesc& item, E::_float fTimeDelta)
{
    item.fBobTime += fTimeDelta * BOB_SPEED;
    item.fBobYRot += fTimeDelta * XMConvertToRadians(ROT_SPEED);
    item.fBobYOffset = sinf(item.fBobTime) * BOB_AMPLITUDE;
}
void CDropBlock::VelocityUpdate(InstancedDropBlockDesc& item, E::_float fTimeDelta)
{
    XMVECTOR vVel = XMLoadFloat3(&item.vVelocity);
    XMVECTOR vWishDir = XMVectorZero();

    // 가속
    float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
    float fAddSpeed = m_fSpeed - fCurrSpeed;
    if (fAddSpeed > 0.f)
    {
        float fAccelSpeed = std::min(18.f * m_fSpeed * fTimeDelta, fAddSpeed);
        vVel += vWishDir * fAccelSpeed;
    }

    // 수평 속도 제한
    XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
    float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
    if (fHorizSpeed > m_fSpeed)
    {
        vHoriz = XMVector3Normalize(vHoriz) * m_fSpeed;
        vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
    }

    // 마찰
    if (item.bOnGround)
    {
        float fSpeed = XMVectorGetX(XMVector3Length(XMVectorSetY(vVel, 0.f)));
        if (fSpeed > 0.f)
        {
            float fNewSpeed = std::max(fSpeed - fSpeed * 15.f * fTimeDelta, 0.f);
            float vy = XMVectorGetY(vVel);
            vVel = XMVectorSetY(vVel * (fNewSpeed / fSpeed), vy);
        }
    }

    // 중력
    if (!item.bOnGround)
        vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);

    // AABB 충돌
    const XMFLOAT3 halfExtents = { 0.25f, 0.49f, 0.25f };
    XMFLOAT3 c = item.vPos;

    // Y
    float velY = XMVectorGetY(vVel);
    float prevY = c.y;
    c.y += velY * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        if (velY < 0.f)
        {
            c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
            item.bOnGround = true;
        }
        else c.y = prevY;
        vVel = XMVectorSetY(vVel, 0.f);
    }
    else item.bOnGround = false;

    // X
    float px = c.x;
    c.x += XMVectorGetX(vVel) * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        c.x = px;
        vVel = XMVectorSetX(vVel, 0.f);
    }

    // Z
    float pz = c.z;
    c.z += XMVectorGetZ(vVel) * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        c.z = pz;
        vVel = XMVectorSetZ(vVel, 0.f);
    }

    item.vPos = c;
    XMStoreFloat3(&item.vVelocity, vVel);
}


void CDropBlock::AddDropItem(_float3 vPos, _float3 vVelocity, const std::vector<uint32_t>& vecTexindex)
{
    InstancedDropBlockDesc Desc{};
    Desc.vPos = vPos;
    Desc.vVelocity = vVelocity;
    Desc.texIndexs = vecTexindex;
    Desc.boxCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.49f, 0.25f });
    Desc.boxCollider->SetInnerPointer(this);
    Desc.boxCollider->SetInnerHint("CDropBlock");
    m_vecDropItems.push_back(Desc);
    CollHint hint{};
    hint.iter = std::prev(m_vecDropItems.end());
    Desc.boxCollider->SetInnerHint2(&hint, sizeof(hint));
}

UPtr<CDropBlock> CDropBlock::Create()
{
    auto pInstance = ToUPtr(new CDropBlock{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CDropBlock");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CDropBlock::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CDropBlock{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CDropBlock");
        return nullptr;
    }

    return pInstance;
}
