#include "DropBlock.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CollBox.h"

NS_USING(Engine)

CDropBlock::CDropBlock()
{
}

CDropBlock::CDropBlock(const CDropBlock& rhs)
    : CDropItemObject{ rhs }
{

}

CDropBlock::~CDropBlock()
{
}

HRESULT CDropBlock::Initialize(void* pArg)
{
    m_RenderPassFlags = ETOUI(RENDERPASS::DEFAULT) | ETOUI(RENDERPASS::SHADOW);
    if (FAILED(CDropItemObject::Initialize(pArg)))
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

    for (auto& item : m_vecDropItemObjects)
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
        E::CGameInstance::Get().AddColliderGroup("Coll_DropItemObject", item.boxCollider.get());

        VTX_DROP_BLOCK_INSTANCED_DATA inst{};

        int32_t blockX = static_cast<int32_t>(std::floor(item.vPos.x));
        int32_t blockY = static_cast<int32_t>(std::floor(item.vPos.y));
        int32_t blockZ = static_cast<int32_t>(std::floor(item.vPos.z));
        if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
        {
            inst.light = optCurrBlock->GetLight();
        }

        if (item.itemInfo.block)
        {
            uint32_t baseColorABGR = CBlock3::GetBaseColor(item.itemInfo.block->GetType());
            if (baseColorABGR != 0xFF)
            {
                _float4 blockTint;
                blockTint.x = ((baseColorABGR >> 0) & 0xFF) / 255.f; // R
                blockTint.y = ((baseColorABGR >> 8) & 0xFF) / 255.f; // G
                blockTint.z = ((baseColorABGR >> 16) & 0xFF) / 255.f; // B
                blockTint.w = ((baseColorABGR >> 24) & 0xFF) / 255.f; // A
                inst.vColor = blockTint;
            }
        }
        
        inst.matWorld = item.matWorld;
        //memcpy(&inst.texIndexs, item.texIndexs.data(), item.texIndexs.size());
        memcpy(&inst.texIndexs, item.texIndexs.data(), sizeof(uint32_t) * item.texIndexs.size());
        m_vecInstancedData.push_back(inst);
    }
}

void CDropBlock::LateUpdate(E::_float fTimeDelta)
{
    E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
}

HRESULT CDropBlock::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{

    StringID vsStrID = ctx.pass == RENDERPASS::SHADOW ? "VS_Shadow_DropBlock" : "VS_DropBlock";
    StringID psStrID = ctx.pass == RENDERPASS::SHADOW ? "PS_Shadow_DropBlock" : "PS_DropBlock";

    const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, vsStrID);
    const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, psStrID);
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

    pContext->DrawIndexedInstanced((UINT)viBuffer->GetNumIndices(), (UINT)m_vecInstancedData.size(), 0, 0, 0);

    return S_OK;
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
