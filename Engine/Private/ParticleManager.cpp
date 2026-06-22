#include "pch.h"

#include "ParticleManager.h"

#include "Resources.h"
#include "GameInstance.h"

NS_USING(Engine)


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

void CParticleManager::AddParticleRenderDeathSmoke(_float3 pos, uint32_t iCnt)
{
    for (int i = 0; i < iCnt; ++i)
    {
        ATTRIBUTE att{};
        att.bAlive = true;
        att.fLifeTime = 4.f;
        att.iTexId = PackTexId(10, 0);
        att.vPos = { pos };
        att.vAcceleration = { 0.f, -1.8f, 0.f };
        //att.vAcceleration = { 0.f, 0.f, 0.f };

        //auto tmp = rand() % 10 / 10.f;
        //auto tmp2 = rand() % 10 / 10.f;
        att.vUv = { 8.f * (7) / 128.f, 0.f / 128.f };
        //8.f / 128.f;
        att.vUvSize = { 8.f / 128.f, 8.f / 128.f };
        //att.vSize = { 8.f / 128.f, 8.f / 128.f };
        auto tmpSize = Randf(0.1f, 1.5f);
        att.vSize = { tmpSize, tmpSize };
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
        AddParticle(PARTICLE_TYPE::PARTICLES_ATLAS_DEATH_SMOKE, att);
    }
}

void CParticleManager::AddParticleRenderExplodeSmoke(_float3 pos, uint32_t iCnt)
{
    for (uint32_t i = 0; i < iCnt; ++i)
    {
        ATTRIBUTE att{};
        att.bAlive = true;
        att.fLifeTime = 0.05f * 15.f;
        att.iTexId = PackTexId(10, 0);
        att.vPos = pos;
        att.vAcceleration = { 0.f, -1.8f, 0.f };

        att.vUv = { 8.f * (15) / 128.f, 80.f / 128.f };
        att.vUvSize = { 8.f / 128.f, 8.f / 128.f };
        auto tmpSize = Randf(0.1f, 2.5f);
        att.vSize = { tmpSize, tmpSize };
        att.vColor = { 1.f, 1.f, 1.f, 1.f };
        XMVECTOR dir =
            XMVectorSet(
                Randf(-1.f, 1.f),
                Randf(-1.f, 1.f),
                Randf(-1.f, 1.f),
                0.f);

        dir = XMVector3Normalize(dir);

        float speed = Randf(4.f, 8.f);
        dir *= speed;

        XMStoreFloat3(&att.vVelocity, dir);
        AddParticle(PARTICLE_TYPE::PARTICLES_ATLAS_EXPLODE_SMOKE, att);
    }
}

void CParticleManager::AddParticleRenderTNTFusing(_float3 pos, uint32_t iCnt)
{
    for (int i = 0; i < iCnt; ++i)
    {
        ATTRIBUTE att{};
        att.bAlive = true;
        att.fLifeTime = 0.7f;
        att.iTexId = PackTexId(10, 0);
        att.vPos = { pos };
        
        att.vVelocity = { Randf(-0.1f, 0.1f), +0.8f, Randf(-0.1f, 0.1f) };
        //att.vAcceleration = { 0.f, 0.f, 0.f };

        //auto tmp = rand() % 10 / 10.f;
        //auto tmp2 = rand() % 10 / 10.f;
        att.vUv = { 8.f * (7) / 128.f, 0.f / 128.f };
        //8.f / 128.f;
        att.vUvSize = { 8.f / 128.f, 8.f / 128.f };
        //att.vSize = { 8.f / 128.f, 8.f / 128.f };
        auto tmpSize = Randf(0.1f, 0.1f);
        att.vSize = { tmpSize, tmpSize };
        //att.vColor = { 1.f, 1.f, 1.f, 1.f };

        float c = Randf(0.0f, 0.8f);
        float a = Randf(0.0f, 0.8f);

        att.vColor = { c, c, c, 1.f };


        //XMVECTOR dir =
        //    XMVectorSet(
        //        Randf(-1.f, 1.f),
        //        Randf(0.5f, 1.5f),
        //        Randf(-1.f, 1.f),
        //        0.f);

        //dir = XMVector3Normalize(dir);

        //float speed = Randf(1.f, 4.f);

        //dir *= speed;

        //XMStoreFloat3(&att.vVelocity, dir);
        AddParticle(PARTICLE_TYPE::PARTICLES_ATLAS_TNT_FUSING, att);
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

    if (ImGui::Button("TEST SMOKE"))
    {
        ATTRIBUTE att{};
        att.bAlive = true;
        att.fLifeTime = 4.f;
        att.iTexId = PackTexId(10, 0);
        att.vPos = {};
        att.vAcceleration = { 0.f, -9.8f, 0.f };
        //att.vAcceleration = { 0.f, 0.f, 0.f };

        //auto tmp = rand() % 10 / 10.f;
        //auto tmp2 = rand() % 10 / 10.f;
        att.vUv = { 8.f * (7) / 128.f, 0.f / 128.f };
        //8.f / 128.f;
        att.vUvSize = { 8.f / 128.f, 8.f / 128.f };
        //att.vSize = { 8.f / 128.f, 8.f / 128.f };
        auto tmpSize = Randf(0.1f, 0.5f);
        att.vSize = { tmpSize, tmpSize };
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
        AddParticle(PARTICLE_TYPE::PARTICLES_ATLAS_DEATH_SMOKE, att);
    }

    if (ImGui::Button("TEST EXPLODE"))
    {
        ATTRIBUTE att{};
        att.bAlive = true;
        att.fLifeTime = 0.05f * 15.f;
        att.iTexId = PackTexId(10, 0);
        att.vPos = {};
        att.vAcceleration = { 0.f, -1.8f, 0.f };

        att.vUv = { 8.f * (15) / 128.f, 80.f / 128.f };
        att.vUvSize = { 8.f / 128.f, 8.f / 128.f };
        auto tmpSize = Randf(0.1f, 2.5f);
        att.vSize = { tmpSize, tmpSize };
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
        AddParticle(PARTICLE_TYPE::PARTICLES_ATLAS_EXPLODE_SMOKE, att);
    }

    if (ImGui::Button("TEST TORCH"))
    {
        ATTRIBUTE att{};
        att.bAlive = true;
        att.fLifeTime = 4.f;
        att.iTexId = PackTexId(10, 0);
        att.vPos = {};
        //att.vAcceleration = { 0.f, -1.8f, 0.f };

        att.vUv = { 8.f * (15) / 128.f, 24.f / 128.f };
        att.vUvSize = { 8.f / 128.f, 8.f / 128.f };
        auto tmpSize = Randf(1.0f, 1.0f);
        att.vSize = { tmpSize, tmpSize };
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

        //XMStoreFloat3(&att.vVelocity, dir);
        AddParticle(PARTICLE_TYPE::PARTICLES_ATLAS_TORCH, att);
    }

    if (ImGui::Button("TEST FUSING"))
    {
        AddParticleRenderTNTFusing({}, 1);
    }
    ImGui::End();
}

void CParticleManager::Update(_float fTimeDelta)
{
    for (auto& v : m_arrVertices)
    {
        v.clear();
    }

    // BLOCK_DESTRUCT
    Update_BLOCK_DESTRUCT(fTimeDelta);

    // PARTICLES_ATLAS_DEATH
    Update_PARTICLES_ATLAS_DEATH_SMOKE(fTimeDelta);

    // PARTICLES_ATLAS_EXPLODE_SMOKE
    Update_PARTICLES_ATLAS_EXPLODE_SMOKE(fTimeDelta);

    // PARTICLES_ATLAS_TORCH
    Update_PARTICLES_ATLAS_TORCH(fTimeDelta);

    Update_PARTICLES_ATLAS_TNT_FUSING(fTimeDelta);
}

void CParticleManager::Update_BLOCK_DESTRUCT(_float fTimeDelta)
{
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

void CParticleManager::Update_PARTICLES_ATLAS_DEATH_SMOKE(_float fTimeDelta)
{
    auto& particles = m_arrParticles[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_DEATH_SMOKE)];
    auto& vertices = m_arrVertices[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_DEATH_SMOKE)];

    size_t i = 0;
    while (i < particles.size())
    {
        ATTRIBUTE& att = particles[i];

        att.fAge += fTimeDelta;
        XMStoreFloat3(&att.vVelocity, XMLoadFloat3(&att.vVelocity) + XMLoadFloat3(&att.vAcceleration) * fTimeDelta);
        XMStoreFloat3(&att.vPos, XMLoadFloat3(&att.vPos) + XMLoadFloat3(&att.vVelocity) * fTimeDelta);


        const float fTileWidth = 8.0f / 128.0f;
        const float fTotalTime = 0.5f * 8.f;
        float fPlayTime = std::min(att.fAge, fTotalTime);
        uint32_t currentStep = static_cast<uint32_t>(fPlayTime / 0.5f);
        uint32_t idx = 7 - currentStep;
        if (currentStep >= 7) {
            idx = 0;
        }

        att.vUv = { idx * fTileWidth, 0.f };


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
void CParticleManager::Update_PARTICLES_ATLAS_EXPLODE_SMOKE(_float fTimeDelta)
{
    auto& particles = m_arrParticles[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_EXPLODE_SMOKE)];
    auto& vertices = m_arrVertices[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_EXPLODE_SMOKE)];

    size_t i = 0;
    while (i < particles.size())
    {
        ATTRIBUTE& att = particles[i];

        att.fAge += fTimeDelta;
        XMStoreFloat3(&att.vVelocity, XMLoadFloat3(&att.vVelocity) + XMLoadFloat3(&att.vAcceleration) * fTimeDelta);
        XMStoreFloat3(&att.vPos, XMLoadFloat3(&att.vPos) + XMLoadFloat3(&att.vVelocity) * fTimeDelta);


        const float fTileWidth = 8.0f / 128.0f;
        const float fTotalTime = 0.05f * 15.f;
        float fPlayTime = std::min(att.fAge, fTotalTime);
        uint32_t currentStep = static_cast<uint32_t>(fPlayTime / 0.05f);
        uint32_t idx = currentStep;
        if (currentStep >= 15) {
            idx = 0;
        }

        att.vUv = { idx * fTileWidth, 80.f / 128.0f };


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

void CParticleManager::Update_PARTICLES_ATLAS_TORCH(_float fTimeDelta)
{
    auto& particles = m_arrParticles[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_TORCH)];
    auto& vertices = m_arrVertices[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_TORCH)];

    size_t i = 0;
    while (i < particles.size())
    {
        ATTRIBUTE& att = particles[i];

        att.fAge += fTimeDelta;
        XMStoreFloat3(&att.vVelocity, XMLoadFloat3(&att.vVelocity) + XMLoadFloat3(&att.vAcceleration) * fTimeDelta);
        XMStoreFloat3(&att.vPos, XMLoadFloat3(&att.vPos) + XMLoadFloat3(&att.vVelocity) * fTimeDelta);


        const float fTileWidth = 8.0f / 128.0f;
        const float fTotalTime = 1.f * 4.f;
        float fPlayTime = std::min(att.fAge, fTotalTime);
        uint32_t currentStep = static_cast<uint32_t>(fPlayTime / 1.f);
        uint32_t idx = currentStep % 2;
        //if (currentStep >= 15) {
        //    idx = 0;
        //}

        att.vUv = { idx * fTileWidth, 24.f / 128.0f };
        att.vSize = { att.vSize.x - (1.f / 4.f * fTimeDelta) , att.vSize.y - (1.f / 4.f * fTimeDelta) };
        //att.vSize.x
        ;


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

void CParticleManager::Update_PARTICLES_ATLAS_TNT_FUSING(_float fTimeDelta)
{
    auto& particles = m_arrParticles[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_TNT_FUSING)];
    auto& vertices = m_arrVertices[ETOUI(PARTICLE_TYPE::PARTICLES_ATLAS_TNT_FUSING)];

    size_t i = 0;
    while (i < particles.size())
    {
        ATTRIBUTE& att = particles[i];

        att.fAge += fTimeDelta;
        XMStoreFloat3(&att.vVelocity, XMLoadFloat3(&att.vVelocity) + XMLoadFloat3(&att.vAcceleration) * fTimeDelta);
        XMStoreFloat3(&att.vPos, XMLoadFloat3(&att.vPos) + XMLoadFloat3(&att.vVelocity) * fTimeDelta);


        const float fTileWidth = 8.0f / 128.0f;
        const float fTotalTime = 0.1f * 8.f;
        float fPlayTime = std::min(att.fAge, fTotalTime);
        uint32_t currentStep = static_cast<uint32_t>(fPlayTime / 0.1f);
        uint32_t idx = 7 - currentStep;
        if (currentStep >= 7) {
            idx = 0;
        }

        att.vUv = { idx * fTileWidth, 0.f };


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
    const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Particle");
    const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Particle");
    const auto& gs = E::CGameInstance::Get().GetResourceFirst<E::CResGeometryShader>(TAG_RES_GRP_PERMANENT_SHADER, "GS_Particle");

    pContext->IASetInputLayout(vs->GetInputLayout().Get());
    pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);
    pContext->GSSetShader(gs->GetGeometryShader().Get(), nullptr, 0);

    {
        const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(
            TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP_NOMIP);
        pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
    }
    {
        const auto& rasterizer = CGameInstance::Get().GetResourceFirst<CResRasterizerState>(
            TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_BACKCULL);
        pContext->RSSetState(rasterizer->GetRasterizerState().Get());
    }

    if (FAILED(RenderPaticle(pContext, PARTICLE_TYPE::BLOCK_DESTRUCT)))
    {
        return E_FAIL;
    }

    if (FAILED(RenderPaticle(pContext, PARTICLE_TYPE::PARTICLES_ATLAS_DEATH_SMOKE)))
    {
        return E_FAIL;
    }

    if (FAILED(RenderPaticle(pContext, PARTICLE_TYPE::PARTICLES_ATLAS_EXPLODE_SMOKE)))
    {
        return E_FAIL;
    }

    if (FAILED(RenderPaticle(pContext, PARTICLE_TYPE::PARTICLES_ATLAS_TORCH)))
    {
        return E_FAIL;
    }

    if (FAILED(RenderPaticle(pContext, PARTICLE_TYPE::PARTICLES_ATLAS_TNT_FUSING)))
    {
        return E_FAIL;
    }

    pContext->GSSetShader(nullptr, nullptr, 0);
    return S_OK;
}

HRESULT CParticleManager::RenderPaticle(ID3D11DeviceContext* pContext, PARTICLE_TYPE eType)
{
    if (!m_arrVertices[ETOUI(eType)].empty())
    {
        D3D11_MAPPED_SUBRESOURCE subResource{};
        if (SUCCEEDED(pContext->Map(m_pResVIBuffer->GetVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
        {
            memcpy(subResource.pData, m_arrVertices[ETOUI(eType)].data(), m_arrVertices[ETOUI(eType)].size() * sizeof(VTX_POINT_PARTICLE));
            pContext->Unmap(m_pResVIBuffer->GetVertexBuffer().Get(), 0);
        }

        ID3D11Buffer* vertexBuffers[] = {
            m_pResVIBuffer->GetVertexBuffer().Get()
        };
        uint32_t strides[] = {
            m_pResVIBuffer->GetVertexStride()
        };
        uint32_t offsets[] = {
            0
        };
        pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
        //pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
        pContext->IASetPrimitiveTopology(m_pResVIBuffer->GetPrimitiveType());


        pContext->Draw((uint32_t)m_arrVertices[ETOUI(eType)].size(), 0);
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
