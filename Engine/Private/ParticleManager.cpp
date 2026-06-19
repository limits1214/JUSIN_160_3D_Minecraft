#include "pch.h"

#include "ParticleManager.h"

#include "Resources.h"
#include "GameInstance.h"

NS_USING(Engine)

float Randf(float min, float max)
{
    return min +
        (max - min) *
        (rand() / (float)RAND_MAX);
}

void CParticleManager::AddParticleRenderDestruct(CBlock3 block, _float3 pos, uint32_t iCnt)
{
    auto texId = CBlock3::GetTexType(block.GetType(), FACE_DIR::POS_X);
    for (int i = 0; i < iCnt; ++i)
    {
        ATTRIBUTE att{};
        att.bAlive = true;
        att.fLifeTime = 2.f;
        att.iTexId = PackTexId(9, static_cast<uint32_t>(texId));
        att.vPos = { pos };
        att.vAcceleration = { 0.f, -9.8f, 0.f };
        //att.vAcceleration = { 0.f, 0.f, 0.f };

        auto tmp = rand() % 10 / 10.f;
        auto tmp2 = rand() % 10 / 10.f;
        att.vUv = { tmp, tmp2 };
        att.vUvSize = { 0.1f, 0.1f };
        att.vSize = { 0.1f, 0.1f };

        att.vColor = { 1.f, 1.f, 1.f, 1.f };
        uint32_t baseColorABGR = CBlock3::GetBaseColor(block.GetType());
        if (baseColorABGR != 0xFF)
        {
            _float4 blockTint;
            blockTint.x = ((baseColorABGR >> 0) & 0xFF) / 255.f; // R
            blockTint.y = ((baseColorABGR >> 8) & 0xFF) / 255.f; // G
            blockTint.z = ((baseColorABGR >> 16) & 0xFF) / 255.f; // B
            blockTint.w = ((baseColorABGR >> 24) & 0xFF) / 255.f; // A
            att.vColor = blockTint;
        }
        
        XMVECTOR dir =
            XMVectorSet(
                Randf(-1.f, 1.f),
                Randf(0.5f, 1.5f),
                Randf(-1.f, 1.f),
                0.f);

        dir = XMVector3Normalize(dir);

        float speed = Randf(1.f, 4.f);

        dir *= speed;

        XMStoreFloat3(&att.vVelocity, dir);
        AddParticle(PARTICLE_TYPE::BLOCK_DESTRUCT, att);
    }
}

CParticleManager::CParticleManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice {pDevice}
    , m_pContext {pContext}
{
}

CParticleManager::~CParticleManager()
{
}

void CParticleManager::UpdateGUI()
{
    ImGui::Begin("ParticleManager");

    if (ImGui::Button("TEST"))
    {
        
        for (int i = 0; i < 130; ++i)
        {
            ATTRIBUTE att{};
            att.bAlive = true;
            att.fLifeTime = 10.f;
            att.iTexId = PackTexId(9, 1);
            att.vPos = {};
            att.vAcceleration = { 0.f, -9.8f, 0.f };
            //att.vAcceleration = { 0.f, 0.f, 0.f };

            auto tmp = rand() % 10 / 10.f;
            auto tmp2 = rand() % 10 / 10.f;
            att.vUv = { tmp, tmp2 };
            att.vUvSize = { 0.1f, 0.1f };
            att.vSize = { 0.1f, 0.1f };
            att.vColor = { 1.f, 1.f, 1.f, 1.f };
            XMVECTOR dir =
                XMVectorSet(
                    Randf(-1.f, 1.f),
                    Randf(0.5f, 1.5f),
                    Randf(-1.f, 1.f),
                    0.f);

            dir = XMVector3Normalize(dir);

            float speed = Randf(1.f, 4.f);

            dir *= speed;

            XMStoreFloat3(&att.vVelocity, dir);
            AddParticle(PARTICLE_TYPE::BLOCK_DESTRUCT, att);
        }
    }


    ImGui::End();
}

void CParticleManager::Update(_float fTimeDelta)
{
    for (auto& v : m_arrVertices)
    {
        v.clear();
    }

    auto& particles = m_arrParticles[ETOUI(PARTICLE_TYPE::BLOCK_DESTRUCT)];
    auto& vertices = m_arrVertices[ETOUI(PARTICLE_TYPE::BLOCK_DESTRUCT)];

    size_t i = 0;
    while (i < particles.size())
    {
        ATTRIBUTE& att = particles[i];

        att.fAge += fTimeDelta;
        XMStoreFloat3(&att.vVelocity, XMLoadFloat3(&att.vVelocity) + XMLoadFloat3(&att.vAcceleration) * fTimeDelta);
        XMStoreFloat3(&att.vPos, XMLoadFloat3(&att.vPos) + XMLoadFloat3(&att.vVelocity) * fTimeDelta);

        if (att.fAge > att.fLifeTime || !att.bAlive)
        {
            if (i != particles.size() - 1)
            {
                std::swap(particles[i], particles.back());
            }
            particles.pop_back();
        }
        else
        {
            VTX_POINT_PARTICLE vtx{};
            vtx.texIndex = att.iTexId;
            vtx.color = att.vColor;
            vtx.pos = att.vPos;
            vtx.size = att.vSize;
            vtx.uvSize = att.vUvSize;
            vtx.texCoord = att.vUv;

            
            int32_t blockX = static_cast<int32_t>(std::floor(att.vPos.x));
            int32_t blockY = static_cast<int32_t>(std::floor(att.vPos.y));
            int32_t blockZ = static_cast<int32_t>(std::floor(att.vPos.z));

            auto optblock = CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ);
            if (optblock)
            {
                vtx.light = optblock.value().GetLight();
            }
            vertices.push_back(vtx);

            ++i;
        }
    }
}


HRESULT CParticleManager::Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{

    {
        const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Particle");
        const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Particle");
        const auto& gs = E::CGameInstance::Get().GetResourceFirst<E::CResGeometryShader>(TAG_RES_GRP_PERMANENT_SHADER, "GS_Particle");
        //const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>("MC_ITEM_VIBuffer", "CubeItemDirt");
        const auto& viBuffer = m_pResVIBuffer;

        pContext->IASetInputLayout(vs->GetInputLayout().Get());
        pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
        pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);
        pContext->GSSetShader(gs->GetGeometryShader().Get(), nullptr, 0);

        if (!m_arrVertices[ETOUI(PARTICLE_TYPE::BLOCK_DESTRUCT)].empty())
        {
            D3D11_MAPPED_SUBRESOURCE subResource{};
            if (SUCCEEDED(pContext->Map(m_pResVIBuffer->GetVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
            {
                memcpy(subResource.pData, m_arrVertices[ETOUI(PARTICLE_TYPE::BLOCK_DESTRUCT)].data(), m_arrVertices[ETOUI(PARTICLE_TYPE::BLOCK_DESTRUCT)].size() * sizeof(VTX_POINT_PARTICLE));
                pContext->Unmap(m_pResVIBuffer->GetVertexBuffer().Get(), 0);
            }
        }

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
        //pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
        pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

        {
            const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
            pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
        }
        {
            const auto& rasterizer = CGameInstance::Get().GetResourceFirst<CResRasterizerState>(
                TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_BACKCULL);
            pContext->RSSetState(rasterizer->GetRasterizerState().Get());
        }
        pContext->Draw((uint32_t)m_arrVertices[ETOUI(PARTICLE_TYPE::BLOCK_DESTRUCT)].size(), 0);

        pContext->GSSetShader(nullptr, nullptr, 0);

    }



    return S_OK;
}




HRESULT CParticleManager::RenderX(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{
    //if (!m_vecAttributes.empty())
    //{
    //    if (m_iVbOffset >= PARTICLE_MAX_CNT)
    //    {
    //        m_iVbOffset = 0;
    //    }

    //    D3D11_MAPPED_SUBRESOURCE subResource{};
    //    if (SUCCEEDED(pContext->Map(
    //        m_pResVIBuffer->GetVertexBuffer().Get(), 0,
    //        m_iVbOffset ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD,
    //        0, &subResource)))
    //    {
    //        uint32_t iNumInBatch{};

    //        for (const auto& att : m_vecAttributes)
    //        {
    //            if (att.bAlive)
    //            {
    //                // 복사
    //                {
    //                    VTX_POINT_PARTICLE* v = (VTX_POINT_PARTICLE*)subResource.pData;
    //                    v += m_iVbOffset + iNumInBatch;

    //                    // TODO
    //                    v->pos = {};
    //                    v->texIndex = {};

    //                }
    //                ++iNumInBatch;


    //                if (iNumInBatch == m_iVbBatchSize)
    //                {
    //                    pContext->Unmap(m_pResVIBuffer->GetVertexBuffer().Get(), 0);

    //                    pContext->Draw(m_iVbBatchSize, m_iVbOffset);

    //                    m_iVbOffset += m_iVbBatchSize;

    //                    if (m_iVbOffset >= PARTICLE_MAX_CNT)
    //                    {
    //                        m_iVbOffset = 0;
    //                    }

    //                    if (FAILED(pContext->Map(
    //                        m_pResVIBuffer->GetVertexBuffer().Get(), 0,
    //                        m_iVbOffset ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD,
    //                        0, &subResource)))
    //                    {
    //                        return E_FAIL;
    //                    }


    //                    iNumInBatch = 0;
    //                }
    //            }
    //        }//end for


    //        pContext->Unmap(m_pResVIBuffer->GetVertexBuffer().Get(), 0);

    //        if (iNumInBatch)
    //        {
    //            pContext->Draw(iNumInBatch, m_iVbOffset);
    //        }

    //        m_iVbOffset += m_iVbBatchSize;


    //    }

    //}

    return S_OK;
}


HRESULT CParticleManager::AddParticle(PARTICLE_TYPE eType, const ATTRIBUTE& particle)
{
    if (m_arrParticles[ETOUI(eType)].size() >= PARTICLE_MAX_CNT)
    {
        return S_OK;
    }
   
    m_arrParticles[ETOUI(eType)].push_back(particle);
    return S_OK;
}


HRESULT CParticleManager::Initialize()
{
    {
        if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Particle", "./Resources/Shader/Particle/Particle.hlsl"))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
        }
        if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Particle", "./Resources/Shader/Particle/Particle.hlsl"))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
        }
        if (auto res = CGameInstance::Get().AddResourceT<E::CResGeometryShader>(TAG_RES_GRP_PERMANENT_SHADER, "GS_Particle", "./Resources/Shader/Particle/Particle.hlsl"))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
        }
    }

    {
        m_pResVIBuffer = CResDynamicVIBuffer::Create();
        CResDynamicVIBuffer::DESC Desc{};
        Desc.iNumVertices = PARTICLE_MAX_CNT;
        Desc.iVertexStride = sizeof(VTX_POINT_PARTICLE);
        Desc.vertexDesc = {
            .ByteWidth = Desc.iNumVertices * Desc.iVertexStride,
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
        Desc.ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

        if (FAILED(m_pResVIBuffer->Load(Desc)))
        {
            return E_FAIL;
        }
    }

    return S_OK;
}

UPtr<CParticleManager> CParticleManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = ToUPtr(new CParticleManager{ pDevice, pContext });

    if (FAILED(pInstance->Initialize()))
    {
        return nullptr;
    }

    return pInstance;
}
