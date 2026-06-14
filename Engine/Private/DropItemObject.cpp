#include "DropItemObject.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CollBox.h"

NS_USING(Engine)

CDropItemObject::CDropItemObject()
{
}

CDropItemObject::CDropItemObject(const CDropItemObject& rhs)
    : CItemObject{rhs}
{
}


CDropItemObject::~CDropItemObject()
{
}

HRESULT CDropItemObject::Initialize(void* pArg)
{
    auto pDesc = static_cast<DESC*>(pArg);

    m_viBufferID = pDesc->viBufferId;
    if (FAILED(CItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    //m_pCenterCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.49f, 0.25f });

    return S_OK;
}

void CDropItemObject::PriorityUpdate(E::_float fTimeDelta)
{
}

void CDropItemObject::Update(E::_float fTimeDelta)
{
}

void CDropItemObject::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CDropItemObject::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

void CDropItemObject::AnimateTransformUpdate(InstancedDropItemObjectDesc& item, E::_float fTimeDelta)
{
    item.fBobTime += fTimeDelta * BOB_SPEED;
    item.fBobYRot += fTimeDelta * XMConvertToRadians(ROT_SPEED);
    item.fBobYOffset = sinf(item.fBobTime) * BOB_AMPLITUDE;
}

void CDropItemObject::VelocityUpdate(InstancedDropItemObjectDesc& item, E::_float fTimeDelta)
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
    const XMFLOAT3 halfExtents = { 0.25f, 0.25f, 0.25f };
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

void CDropItemObject::AddDropItemObject(const CItemObject::ItemInfo& itemInfo, _float3 vPos, _float3 vVelocity, const std::vector<uint32_t>& vecTexindex)
{
    InstancedDropItemObjectDesc Desc{};
    Desc.itemInfo = itemInfo;
    Desc.vPos = vPos;
    Desc.vVelocity = vVelocity;
    Desc.texIndexs = vecTexindex;
    Desc.boxCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.25f, 0.25f });
    Desc.boxCollider->SetInnerPointer(this);
    Desc.boxCollider->SetInnerHint("CDropBlock");
    m_vecDropItemObjects.push_back(Desc);
    CollHint hint{};
    hint.iter = std::prev(m_vecDropItemObjects.end());
    Desc.boxCollider->SetInnerHint2(&hint, sizeof(hint));
}