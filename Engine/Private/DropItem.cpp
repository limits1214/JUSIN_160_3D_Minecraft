#include "DropItem.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CollBox.h"

NS_USING(Engine)

CDropItem::CDropItem()
{
}

CDropItem::CDropItem(const CDropItem& rhs)
    : CItemObject{ rhs }
{

}


CDropItem::~CDropItem()
{
}

HRESULT CDropItem::Initialize(void* pArg)
{
	auto pDesc = static_cast<DESC*>(pArg);

	m_viBufferID = pDesc->viBufferId;
    if (FAILED(CItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    m_pCenterCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.49f, 0.25f });


    {
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Transform", "Com_AnimateTransform", nullptr, &m_pComAnimateTransform)))
        {
            return E_FAIL;
        }
    }

	return S_OK;
}

void CDropItem::PriorityUpdate(E::_float fTimeDelta)
{
}

void CDropItem::Update(E::_float fTimeDelta)
{
	if (m_bGravity)
	{
		VelocityUpdate(fTimeDelta);
	}

    if (m_bAnimation)
    {
        AnimateTransformUpdate(fTimeDelta);
    }
}

void CDropItem::LateUpdate(E::_float fTimeDelta)
{
    E::CGameInstance::Get().AddColliderGroup("Coll_DropItemCenter", m_pCenterCollider.get());
    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

    E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
    //GetTransform().SetParentWorldMatrix(*m_pComAnimateTransform->GetWorldMatrix());
    GetTransform().Update();
}

HRESULT CDropItem::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	{
		auto pResCBuf = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerQuadItemAnim");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pResCBuf->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
			E::CB_PER_QUADITEM_ANIM cbPerQuadItemAnim{};
			memcpy(mappedSubResource.pData, &cbPerQuadItemAnim, sizeof(cbPerQuadItemAnim));
			pContext->Unmap(pResCBuf->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(5, 1, pResCBuf->GetCBuffer().GetAddressOf());
	}
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Item");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Item");
	//const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>("MC_ITEM_VIBuffer", "CubeItemDirt");
	const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>(m_viBufferID.first, m_viBufferID.second);

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
		auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
            _matrix tmpWorld;

            if (m_bAnimation)
            {
                tmpWorld = XMLoadFloat4x4(m_pComAnimateTransform->GetCombinedWorldMatrix()) * XMLoadFloat4x4(GetTransform().GetCombinedWorldMatrix());
            }
            else
            {
                tmpWorld = XMLoadFloat4x4(GetTransform().GetCombinedWorldMatrix());
            }

			E::CB_PER_OBJECT cbPerObject{};
            XMStoreFloat4x4(&cbPerObject.matWorld, tmpWorld);
			//cbPerObject.matWorld = *m_pComAnimateTransform->GetCombinedWorldMatrix()  * *GetTransform().GetCombinedWorldMatrix();
			XMStoreFloat4x4(&cbPerObject.matWVP, tmpWorld  * ctx.matViewProj);

			memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
			pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
		pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
	}

	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

void CDropItem::VelocityUpdate(E::_float fTimeDelta)
{
	XMVECTOR vVel = XMLoadFloat3(&m_vVelocity);

    XMVECTOR vWishDir = XMVectorZero();

    // 2. 가속
    float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
    float fAddSpeed = m_fSpeed - fCurrSpeed;
    if (fAddSpeed > 0.f)
    {
        float fAccel = m_bOnGround ? 18.f : 18.f;
        float fAccelSpeed = std::min(fAccel * m_fSpeed * fTimeDelta, fAddSpeed);
        vVel += vWishDir * fAccelSpeed;
    }

    // bunnyhop remove
    XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
    float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
    if (fHorizSpeed > m_fSpeed)
    {
        vHoriz = XMVector3Normalize(vHoriz) * m_fSpeed;
        vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
    }

    // 3. 마찰 (입력 없을 때만)
    bool bHasInput = XMVectorGetX(XMVector3LengthSq(vWishDir)) > 0.f;
    if (m_bOnGround && !bHasInput)
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
    if (!m_bOnGround)
        vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);

    // 점프
    //if (m_bKeyPressingSpace && m_bOnGround)
    //{
    //    vVel = XMVectorSetY(vVel, 7.f);
    //    m_bOnGround = false;
    //}

    // AABB 충돌
    const XMFLOAT3 halfExtents = { 0.25f, 0.49f, 0.25f };
    XMFLOAT3 pos = GetTransform().GetPosition();
    XMFLOAT3 c = { pos.x, pos.y + 0.f, pos.z };

    // Y
    float velY = XMVectorGetY(vVel);
    float prevY = c.y;  // 이동 전 저장
    c.y += velY * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        if (velY < 0.f)  // 내려가다 충돌 → 땅
        {
            c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;// + 0.001f 딱붙어서 안움직이는현상
            m_bOnGround = true;
        }
        else  // 올라가다 천장
        {
            c.y = prevY;  // ← 그냥 이전 위치로 복구
        }
        vVel = XMVectorSetY(vVel, 0.f);
    }
    else m_bOnGround = false;

    // X
    float px = c.x;
    c.x += XMVectorGetX(vVel) * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        c.x = px; vVel = XMVectorSetX(vVel, 0.f);
    }

    // Z
    float pz = c.z;
    c.z += XMVectorGetZ(vVel) * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        c.z = pz; vVel = XMVectorSetZ(vVel, 0.f);
    }

    GetTransform().SetPosition(_float3{ c.x, c.y - 0.f, c.z });

	XMStoreFloat3(&m_vVelocity, vVel);
}

void CDropItem::AnimateTransformUpdate(E::_float fTimeDelta)
{
    m_fBobTime += fTimeDelta * BOB_SPEED;
    m_fRotAngle += fTimeDelta * ROT_SPEED;

    float bobOffset = sinf(m_fBobTime) * BOB_AMPLITUDE;
    m_pComAnimateTransform->SetPosition(_float3{ 0.f, bobOffset, 0.f });

    m_pComAnimateTransform->SetRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotAngle);

    m_pComAnimateTransform->Update();
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
