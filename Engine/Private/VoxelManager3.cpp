#include "VoxelManager3.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"

#include "SkeletonEntity.h"
#include "CollBox.h"

NS_USING(Engine)
static inline int32_t FloorDiv(int32_t a, int32_t b)
{
    return a / b - (a % b != 0 && (a ^ b) < 0);
}

std::optional<CBlock3> CVoxelManager3::GetBlockByChunkCoord(uint64_t chunkIdx, int32_t cbx, int32_t cby, int32_t cbz) const
{
    // Shadow 먼저 확인
    auto shadowIter = m_EditShadow.find(chunkIdx);
    if (shadowIter != m_EditShadow.end())
    {
        uint32_t blockIdx = CChunk3::BlockIndexing(cbx, cby, cbz);
        auto blockIter = shadowIter->second.find(blockIdx);
        if (blockIter != shadowIter->second.end())
            return blockIter->second;
    }

    auto chunkIter = m_mapChunks.find(chunkIdx);
    if (chunkIter != m_mapChunks.end())
        return chunkIter->second->GetBlock(cbx, cby, cbz);

    return std::nullopt;
}

std::optional<CBlock3> CVoxelManager3::GetBlock(int32_t wbx, int32_t wby, int32_t wbz) const
{
    int32_t cx = FloorDiv(wbx, (int32_t)VOXEL_CHUNK_X_SIZE3);
    int32_t cy = FloorDiv(wby, (int32_t)VOXEL_CHUNK_Y_SIZE3);
    int32_t cz = FloorDiv(wbz, (int32_t)VOXEL_CHUNK_Z_SIZE3);

    uint32_t cbx = (uint32_t)(wbx - cx * (int32_t)VOXEL_CHUNK_X_SIZE3);
    uint32_t cby = (uint32_t)(wby - cy * (int32_t)VOXEL_CHUNK_Y_SIZE3);
    uint32_t cbz = (uint32_t)(wbz - cz * (int32_t)VOXEL_CHUNK_Z_SIZE3);

    return GetBlockByChunkCoord(encodeChunkCoord(cx, cy, cz), cbx, cby, cbz);
}

void CVoxelManager3::SetBlock(int32_t wbx, int32_t wby, int32_t wbz, CBlock3 block)
{
    int32_t cx = FloorDiv(wbx, (int32_t)VOXEL_CHUNK_X_SIZE3);
    int32_t cy = FloorDiv(wby, (int32_t)VOXEL_CHUNK_Y_SIZE3);
    int32_t cz = FloorDiv(wbz, (int32_t)VOXEL_CHUNK_Z_SIZE3);

    uint32_t cbx = (uint32_t)(wbx - cx * (int32_t)VOXEL_CHUNK_X_SIZE3);
    uint32_t cby = (uint32_t)(wby - cy * (int32_t)VOXEL_CHUNK_Y_SIZE3);
    uint32_t cbz = (uint32_t)(wbz - cz * (int32_t)VOXEL_CHUNK_Z_SIZE3);

    uint32_t blockIdx = CChunk3::BlockIndexing(cbx, cby, cbz);
    uint64_t chunkIdx = encodeChunkCoord(cx, cy, cz);

    m_EditShadow[chunkIdx][blockIdx] = block;  // unordered_map은 []로 한번에 처리 가능
}

void CVoxelManager3::SetBlocks(std::vector<std::tuple<int32_t, int32_t, int32_t, CBlock3>> blocks)
{
    for (const auto& [wbx, wby, wbz, block] : blocks)
    {
        SetBlock(wbx, wby, wbz, block);
    }
}

_bool CVoxelManager3::BlockRaycast(const _float3& rayOrigin, const _float3& rayDir, float fMaxDist, BLOCK_RAY_RESULT& outResult) const
{    // --- 1. 시작 블록 ---
    int32_t bx = (int32_t)floorf(rayOrigin.x);
    int32_t by = (int32_t)floorf(rayOrigin.y);
    int32_t bz = (int32_t)floorf(rayOrigin.z);

    // 각 축 이동 방향
    int stepX = (rayDir.x >= 0) ? 1 : -1;
    int stepY = (rayDir.y >= 0) ? 1 : -1;
    int stepZ = (rayDir.z >= 0) ? 1 : -1;

    // 다음 블록 경계까지의 t값
    float tMaxX = (rayDir.x != 0) ? ((stepX > 0 ? (bx + 1.f) : bx) - rayOrigin.x) / rayDir.x : FLT_MAX;
    float tMaxY = (rayDir.y != 0) ? ((stepY > 0 ? (by + 1.f) : by) - rayOrigin.y) / rayDir.y : FLT_MAX;
    float tMaxZ = (rayDir.z != 0) ? ((stepZ > 0 ? (bz + 1.f) : bz) - rayOrigin.z) / rayDir.z : FLT_MAX;

    // 블록 하나 이동할 때마다 증가하는 t값
    float tDeltaX = (rayDir.x != 0) ? fabsf(1.f / rayDir.x) : FLT_MAX;
    float tDeltaY = (rayDir.y != 0) ? fabsf(1.f / rayDir.y) : FLT_MAX;
    float tDeltaZ = (rayDir.z != 0) ? fabsf(1.f / rayDir.z) : FLT_MAX;

    FACE_DIR lastFace = FACE_DIR::END;
    float    tCurrent = 0.f; // 현재 블록에 진입한 시점의 t값

    while (true)
    {
        // 최대 거리 초과 시 조기 종료
        if (tCurrent > fMaxDist)
            break;

        CChunk3* pChunk = GetChunkByWorldBlockCoord(bx, by, bz);
        if (pChunk)
        {
            const auto& [cx, cy, cz] = pChunk->GetCoord();
            uint32_t lx = (uint32_t)(bx - cx * (int32_t)VOXEL_CHUNK_X_SIZE3);
            uint32_t ly = (uint32_t)(by - cy * (int32_t)VOXEL_CHUNK_Y_SIZE3);
            uint32_t lz = (uint32_t)(bz - cz * (int32_t)VOXEL_CHUNK_Z_SIZE3);

            
            auto block = GetBlockByChunkCoord(pChunk->GetCoordIdx(), lx, ly, lz).value();
            if (block.GetType() != CBlock3::TYPE::AIR
                && !block.IsWater() )
            {
                //outResult.pChunk = pChunk;
                outResult.iWorldBlockX = bx;
                outResult.iWorldBlockY = by;
                outResult.iWorldBlockZ = bz;
                outResult.iChunkX = cx;
                outResult.iChunkY = cy;
                outResult.iChunkZ = cz;
                outResult.iChunkBlockX = lx;
                outResult.iChunkBlockY = ly;
                outResult.iChunkBlockZ = lz;
                outResult.block = block;
                outResult.eHitFace = lastFace;
                outResult.fDist = tCurrent; // 이 블록에 진입한 t (= 이전 스텝의 tMax)
                return true;
            }
        }

        // 가장 가까운 축 경계로 이동
        if (tMaxX < tMaxY && tMaxX < tMaxZ)
        {
            tCurrent = tMaxX;
            tMaxX += tDeltaX;
            bx += stepX;
            lastFace = (stepX > 0) ? FACE_DIR::NEG_X : FACE_DIR::POS_X;
        }
        else if (tMaxY < tMaxZ)
        {
            tCurrent = tMaxY;
            tMaxY += tDeltaY;
            by += stepY;
            lastFace = (stepY > 0) ? FACE_DIR::NEG_Y : FACE_DIR::POS_Y;
        }
        else
        {
            tCurrent = tMaxZ;
            tMaxZ += tDeltaZ;
            bz += stepZ;
            lastFace = (stepZ > 0) ? FACE_DIR::NEG_Z : FACE_DIR::POS_Z;
        }
    }

    return false;
}

HRESULT CVoxelManager3::QueuingInRangeChunkCreate(const IN_RANGE_CHUNK_CREATE_DESC& desc)
{
    m_queueInRangeChunkCreate.clear();
    m_queueInRangeChunkCreate.push_back(desc);
    return S_OK;
}

HRESULT CVoxelManager3::QueuingOutRangeChunkRelease(const OUT_RANGE_CHUNK_RELEASE_DESC& desc)
{
    m_queueOutRangeChunkRelease.clear();
    m_queueOutRangeChunkRelease.push_back(desc);
    return S_OK;
}

CVoxelManager3::CVoxelManager3(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{pDevice}
    , m_pContext{pContext}
{
}

CVoxelManager3::~CVoxelManager3()
{
}

HRESULT CVoxelManager3::Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{
    pContext->PSSetSamplers(9, 1, m_pResSamplerPointWrap->GetSamplerState().GetAddressOf());

    const auto& solidBlockVS = m_pResSolidBlockVertexShader;
    const auto& solidBlockPS = m_pResSolidBlockPixelShader;

    pContext->IASetInputLayout(solidBlockVS->GetInputLayout().Get());
    pContext->VSSetShader(solidBlockVS->GetVertexShader().Get(), nullptr, 0);
    pContext->PSSetShader(solidBlockPS->GetPixelShader().Get(), nullptr, 0);
   
    const auto& rasterizer = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_BACKCULL);
    pContext->RSSetState(rasterizer->GetRasterizerState().Get());

    //auto pCameraObject = CGameInstance::Get().GetActiveGameCamera("Player");
    auto vecCollGroup = CGameInstance::Get().GetColliderGroup("Coll_PlayerCamera");
    _bool bExists = vecCollGroup && !vecCollGroup->empty();

    std::vector<CChunk3*> vecIntersectedChunk{};
    vecIntersectedChunk.reserve(m_mapChunks.size());

    for (const auto& [idx, pChunk] : m_mapChunks)
    {
        if (bExists)
        {
            if (vecCollGroup->front()->Intersect(*pChunk->GetCollBox()))
            {
                vecIntersectedChunk.push_back(pChunk.get());
                pChunk->DrawSolid(pContext, ctx);
            }
        }
        else
        {
            vecIntersectedChunk.push_back(pChunk.get());
            pChunk->DrawSolid(pContext, ctx);
        }
    }


    const auto& rs = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL);
    pContext->RSSetState(rs->GetRasterizerState().Get());

    for (auto& pChunk : vecIntersectedChunk)
    {
        pChunk->DrawAlphaTest(pContext, ctx);
    }


    const auto& solidWaterVS = m_pResWaterBlockVertexShader;
    const auto& solidWaterPS = m_pResWaterBlockPixelShader;

    pContext->IASetInputLayout(solidWaterVS->GetInputLayout().Get());
    pContext->VSSetShader(solidWaterVS->GetVertexShader().Get(), nullptr, 0);
    pContext->PSSetShader(solidWaterPS->GetPixelShader().Get(), nullptr, 0);
    //TAG_RES_GRP_PERMANENT_STATE, "BS_ALPHA_BLEND"
    //TAG_RES_GRP_PERMANENT_STATE, "DS_NO_DEPTHWRITE"
    //TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL
    const auto& alphaBlend = E::CGameInstance::GetConst().GetResourceFirst<E::CResBlendState>(TAG_RES_GRP_PERMANENT_STATE, "BS_ALPHA_BLEND");
    const auto& alphaDepth = E::CGameInstance::GetConst().GetResourceFirst<E::CResDepthStencilState>(TAG_RES_GRP_PERMANENT_STATE, "DS_NO_DEPTHWRITE");
    

    _float fBlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    pContext->OMSetBlendState(alphaBlend->GetBlendState().Get(), fBlendFactor, 0xffffffff);
    pContext->OMSetDepthStencilState(alphaDepth->GetDepthStencilState().Get(), 0);


    for (auto& pChunk : vecIntersectedChunk)
    {
        pChunk->DrawWater(pContext, ctx);
    }

    pContext->OMSetBlendState(nullptr, fBlendFactor, 0xffffffff); // nullptr 기본값 = 블렌드 Off
    pContext->OMSetDepthStencilState(nullptr, 0);                 // nullptr 기본값 = 깊이 On, 기록 On
    pContext->RSSetState(nullptr);
    return S_OK;
}

void CVoxelManager3::RuntimeOnBlockRemovedLighting(int32_t wbx, int32_t wby, int32_t wbz, bool bIsLightSource, uint8_t oldBlockLight)
{
    constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
    constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };

    // ----------------------------------------------------
    // 1. SkyLight 처리
    // ----------------------------------------------------
    {
        auto optBlock = GetBlock(wbx, wby, wbz);
        if (optBlock.has_value())
        {
            CBlock3 block = optBlock.value();
            auto optAbove = GetBlock(wbx, wby + 1, wbz);

            std::queue<std::pair<XMINT3, uint8_t>> skyLightQ;

            // 직통 하늘 케이스
            if (!optAbove.has_value() || optAbove.value().GetSkyLight() == 15)
            {
                block.SetSkyLight(15);
                SetBlock(wbx, wby, wbz, block);
                skyLightQ.push({ XMINT3{wbx, wby, wbz}, 15 });
            }
            // 동굴 내부 혹은 그늘진 곳에서 블록 파괴 시 주변 스카이라이트 수집
            else
            {
                uint8_t maxSkyLight = 0;
                for (int d = 0; d < 6; ++d)
                {
                    auto nopt = GetBlock(wbx + dx[d], wby + dy[d], wbz + dz[d]);
                    if (nopt.has_value())
                    {
                        maxSkyLight = std::max(maxSkyLight, nopt.value().GetSkyLight());
                    }
                }

                if (maxSkyLight > 1)
                {
                    block.SetSkyLight(maxSkyLight - 1);
                    SetBlock(wbx, wby, wbz, block);
                    skyLightQ.push({ XMINT3{wbx, wby, wbz}, maxSkyLight - 1 });
                }
            }

            if (!skyLightQ.empty())
            {
                RuntimeFloodFillSkyLighting(skyLightQ);
            }
        }
    }

    // ----------------------------------------------------
    // 2. BlockLight 처리
    // ----------------------------------------------------
    {
        auto optBlock = GetBlock(wbx, wby, wbz);
        if (optBlock.has_value())
        {
            CBlock3 block = optBlock.value();

            if (bIsLightSource)
            {
                // 광원 제거 로직 수행
                block.SetBlockLight(0);
                SetBlock(wbx, wby, wbz, block);

                std::queue<std::pair<XMINT3, uint8_t>> blockLightRemovalQ;
                blockLightRemovalQ.push({ XMINT3{wbx, wby, wbz}, oldBlockLight });
                RuntimeRemoveBlockLighting(blockLightRemovalQ);
            }
            else
            {
                // 일반 블록 제거 시 주변 빛 스며들기
                uint8_t maxBlockLight = 0;
                for (int d = 0; d < 6; ++d)
                {
                    auto nopt = GetBlock(wbx + dx[d], wby + dy[d], wbz + dz[d]);
                    if (nopt.has_value())
                    {
                        maxBlockLight = std::max(maxBlockLight, nopt.value().GetBlockLight());
                    }
                }

                if (maxBlockLight > 1)
                {
                    block.SetBlockLight(maxBlockLight - 1);
                    SetBlock(wbx, wby, wbz, block);

                    std::queue<std::pair<XMINT3, uint8_t>> blockLightQ;
                    blockLightQ.push({ XMINT3{wbx, wby, wbz}, maxBlockLight - 1 });
                    RuntimeFloodFillBlockLighting(blockLightQ);
                }
            }
        }
    }
}

void CVoxelManager3::RuntimeRemoveBlockLighting(std::queue<std::pair<XMINT3, uint8_t>>& q)
{
    constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
    constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };
    std::queue<std::pair<XMINT3, uint8_t>> rePropagateQ;

    while (!q.empty())
    {
        auto [wbcoord, light] = q.front(); q.pop();
        auto [wbx, wby, wbz] = wbcoord;

        for (int d = 0; d < 6; ++d)
        {
            int nx = wbx + dx[d];
            int ny = wby + dy[d];
            int nz = wbz + dz[d];
            auto nopt = GetBlock(nx, ny, nz);
            if (!nopt.has_value()) continue;
            CBlock3 nBlock = nopt.value();

            uint8_t nLight = nBlock.GetBlockLight();

            // 내가 전파했던 어두운 자식 빛들을 순차적으로 0으로 끔
            if (nLight != 0 && nLight == light - 1)
            {
                nBlock.SetBlockLight(0);
                SetBlock(nx, ny, nz, nBlock);
                q.push({ XMINT3{nx, ny, nz}, nLight });
            }
            // 나를 밝혀주던 다른 독립적인 광원 줄기를 만나면 재전파 큐에 백업
            else if (nLight >= light)
            {
                rePropagateQ.push({ XMINT3{nx, ny, nz}, nLight });
            }
        }
    }

    // 빛 청소가 완전히 끝난 후, 살아남은 다른 광원들로부터 빛을 다시 복구
    if (!rePropagateQ.empty())
    {
        RuntimeFloodFillBlockLighting(rePropagateQ);
    }
}

void CVoxelManager3::RuntimeFloodFillBlockLighting(std::queue<std::pair<XMINT3, uint8_t>>& q)
{
    constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
    constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };

    while (!q.empty())
    {
        // 버그 수정: 큐에서 세컨드 인자(curLight)를 온전히 추출하여 전파에 사용
        auto [wbcoord, curLight] = q.front(); q.pop();
        auto [wbx, wby, wbz] = wbcoord;
        auto curBlockOpt = GetBlock(wbx, wby, wbz);

        _bool bIgnore{ false };
        if (curBlockOpt.has_value() && (curBlockOpt.value().GetType() == CBlock3::TYPE::TORCH_ON || CBlock3::IsNeedAlphaTest(curBlockOpt.value().GetType())))
        {
            bIgnore = true;
        }

        if (curLight <= 1) continue;

        for (int d = 0; d < 6; ++d)
        {
            int nx = wbx + dx[d];
            int ny = wby + dy[d];
            int nz = wbz + dz[d];

            auto nopt = GetBlock(nx, ny, nz);
            if (!nopt.has_value()) continue;

            CBlock3 nBlock = nopt.value();

            if (!bIgnore)
            {
                if (nBlock.IsOpaque()) continue;

            }

            uint8_t newLight = curLight - 1;

            if (newLight > nBlock.GetBlockLight())
            {
                nBlock.SetBlockLight(newLight);
                SetBlock(nx, ny, nz, nBlock);
                q.push({ XMINT3{nx, ny, nz} , newLight });
            }
        }
    }
}

void CVoxelManager3::RuntimeFloodFillSkyLighting(std::queue<std::pair<XMINT3, uint8_t>>& q)
{
    // index 규칙: 0:우, 1:좌, 2:상, 3:하, 4:전, 5:후 (하늘 아래 방향은 d == 3)
    constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
    constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };

    while (!q.empty())
    {
        auto [wbcoord, curLight] = q.front(); q.pop();
        auto [wbx, wby, wbz] = wbcoord;


        // 물의 감쇠량(3) 때문에 내 빛이 2나 3이어도 전파 연산이 필요할 수 있습니다.
        // 따라서 0만 확실하게 걸러냅니다.
        if (curLight == 0) continue;


        for (int d = 0; d < 6; ++d)
        {
            int nx = wbx + dx[d];
            int ny = wby + dy[d];
            int nz = wbz + dz[d];

            auto nopt = GetBlock(nx, ny, nz);
            if (!nopt.has_value()) continue;

            CBlock3 nBlock = nopt.value();
            //if (nBlock.IsOpaque()) continue;

            if (!CBlock3::IsNeedAlphaTest(nBlock.GetType()))
            {
                if (nBlock.IsOpaque()) continue;
            }

            // -----------------------------------------------------------------
            // [수정] 물과 공기에 따른 기본 감쇠 차등 적용
            // -----------------------------------------------------------------
            uint8_t attenuation = nBlock.IsWater() ? 3 : 1;
            uint8_t newLight = (curLight > attenuation) ? (curLight - attenuation) : 0;

            // [수정] 수직 아래 방향 전파 특수 규칙 (다음 칸이 물이 아닐 때만 15 직하강)
            if (d == 3 && curLight == 15 && !nBlock.IsWater())
            {
                newLight = 15;
            }

            // 목적지 블록의 기존 빛보다 새로 전파할 빛이 더 밝을 때만 갱신
            if (newLight > nBlock.GetSkyLight())
            {
                nBlock.SetSkyLight(newLight);
                SetBlock(nx, ny, nz, nBlock);
                q.push({ XMINT3{nx, ny, nz} , newLight });
            }
        }
    }
}

void CVoxelManager3::RuntimeRemoveSkyLighting(std::queue<std::pair<XMINT3, uint8_t>>& q)
{
    constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
    constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };
    std::queue<std::pair<XMINT3, uint8_t>> rePropagateQ;

    while (!q.empty())
    {
        auto [wbcoord, light] = q.front(); q.pop();
        auto [wbx, wby, wbz] = wbcoord;

        for (int d = 0; d < 6; ++d)
        {
            int nx = wbx + dx[d];
            int ny = wby + dy[d];
            int nz = wbz + dz[d];
            auto nopt = GetBlock(nx, ny, nz);
            if (!nopt.has_value()) continue;
            CBlock3 nBlock = nopt.value();

            uint8_t nLight = nBlock.GetSkyLight();

            // -----------------------------------------------------------------
            // [수정] 물과 공기에 따른 예상 전파 빛 수치 계산
            // -----------------------------------------------------------------
            uint8_t attenuation = nBlock.IsWater() ? 3 : 1;
            uint8_t expectedLight = (light > attenuation) ? (light - attenuation) : 0;

            // 하늘 직사광선(15) 줄기 판정 (다음 칸이 물이 아닐 때만 15가 유지됨)
            bool bIsSkyColumn = (dy[d] == -1 && light == 15 && nLight == 15 && !nBlock.IsWater());

            // 내가 전파했던 하위 빛이 맞다면 (예상한 수치와 일치하거나 직하강 줄기라면)
            if (nLight != 0 && (nLight == expectedLight || bIsSkyColumn))
            {
                nBlock.SetSkyLight(0);
                SetBlock(nx, ny, nz, nBlock);
                q.push({ XMINT3{nx, ny, nz}, nLight });
            }
            // 나를 비춰주던 다른 살아있는 스카이라이트 줄기를 만난 경우
            else if (nLight >= light)
            {
                rePropagateQ.push({ XMINT3{nx, ny, nz}, nLight });
            }
        }
    }

    // 제거 루프 완료 후 살아남은 빛들을 다시 사방으로 퍼트려 빈자리를 메웁니다.
    RuntimeFloodFillSkyLighting(rePropagateQ);
}

void CVoxelManager3::WorkerFloodFillBlockLighting(std::unordered_set<uint64_t>& chunkIdxLookupBundle, std::queue<std::pair<XMINT3, uint8_t>>& q)
{
    constexpr int dx[] = { 1, -1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1, -1, 0, 0 };
    constexpr int dz[] = { 0, 0, 0, 0, 1, -1 };

    while (!q.empty())
    {
        auto [curWorldPos, expectedLight] = q.front();
        q.pop();

        // 1. 현재 좌표의 청크 인덱스 계산
        int32_t cx = FloorDiv(curWorldPos.x, 32);
        int32_t cy = FloorDiv(curWorldPos.y, 256);
        int32_t cz = FloorDiv(curWorldPos.z, 32);

        uint64_t chunkIdx = encodeChunkCoord(cx, cy, cz);
        if (chunkIdxLookupBundle.find(chunkIdx) == chunkIdxLookupBundle.end()) continue;

        auto chunkIter = m_mapChunks.find(chunkIdx);
        if (chunkIter == m_mapChunks.end()) continue;

        CChunk3* pCurChunk = chunkIter->second.get(); // 현재 청크 포인터 캐싱

        int32_t lx = curWorldPos.x - cx * 32;
        int32_t lz = curWorldPos.z - cz * 32;
        CBlock3& curBlock = pCurChunk->GetBlock(lx, curWorldPos.y, lz);

        if (curBlock.GetBlockLight() > expectedLight) continue;

        uint8_t curLight = curBlock.GetBlockLight();
        if (curLight <= 1) continue; // 빛이 1 이하면 더 이상 전파할 수 없음 (감쇠하면 0이 되므로)

        // 2. 6방향 주변 검사
        for (int d = 0; d < 6; ++d)
        {
            int nx = curWorldPos.x + dx[d];
            int ny = curWorldPos.y + dy[d];
            int nz = curWorldPos.z + dz[d];

            if (ny < 0 || ny >= 256) continue;

            // 주변 칸의 청크 좌표 계산
            int32_t ncx = FloorDiv(nx, 32);
            int32_t ncy = FloorDiv(ny, 256);
            int32_t ncz = FloorDiv(nz, 32);
            uint64_t nChunkIdx = encodeChunkCoord(ncx, ncy, ncz);

            CChunk3* pNextChunk = nullptr;

            // [최적화 핵심]: 다음 전파할 칸이 현재 청크와 같다면 find를 생략하고 캐싱된 포인터 사용
            if (nChunkIdx == chunkIdx)
            {
                pNextChunk = pCurChunk;
            }
            else
            {
                //if (chunkIdxLookupBundle.find(nChunkIdx) == chunkIdxLookupBundle.end()) continue;
                auto nChunkIter = m_mapChunks.find(nChunkIdx);
                if (nChunkIter == m_mapChunks.end()) continue;
                pNextChunk = nChunkIter->second.get();
            }

            int32_t nlx = nx - ncx * 32;
            int32_t nlz = nz - ncz * 32;
            CBlock3& nBlock = pNextChunk->GetBlock(nlx, ny, nlz);

            if (nBlock.IsOpaque()) continue;

            // 블록 라이트는 방향 관계없이 무조건 1씩 감쇠
            uint8_t newLight = curLight - 1;

            if (newLight > nBlock.GetBlockLight())
            {
                nBlock.SetBlockLight(newLight);
                q.push({ { nx, ny, nz }, newLight });
            }
        }
    }
}

void CVoxelManager3::WorkerFloodFillSkyLighting(std::unordered_set<uint64_t>& chunkIdxLookupBundle, std::queue<std::pair<XMINT3, uint8_t>>& q)
{
    constexpr int dx[] = { 1, -1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1, -1, 0, 0 }; // d == 2는 위(+1), d == 3은 아래(-1)
    constexpr int dz[] = { 0, 0, 0, 0, 1, -1 };

    while (!q.empty())
    {
        auto [curWorldPos, expectedLight] = q.front();
        q.pop();

        int32_t cx = FloorDiv(curWorldPos.x, 32);
        int32_t cy = FloorDiv(curWorldPos.y, 256);
        int32_t cz = FloorDiv(curWorldPos.z, 32);

        uint64_t chunkIdx = encodeChunkCoord(cx, cy, cz);
        if (chunkIdxLookupBundle.find(chunkIdx) == chunkIdxLookupBundle.end()) continue;

        auto chunkIter = m_mapChunks.find(chunkIdx);
        if (chunkIter == m_mapChunks.end()) continue;

        int32_t lx = curWorldPos.x - cx * 32;
        int32_t lz = curWorldPos.z - cz * 32;
        CBlock3& curBlock = chunkIter->second->GetBlock(lx, curWorldPos.y, lz);

        if (curBlock.GetSkyLight() > expectedLight) continue;

        uint8_t curLight = curBlock.GetSkyLight();
        if (curLight == 0) continue;

        // 6방향 주변 검사
        for (int d = 0; d < 6; ++d)
        {
            int nx = curWorldPos.x + dx[d];
            int ny = curWorldPos.y + dy[d];
            int nz = curWorldPos.z + dz[d];

            if (ny < 0 || ny >= 256) continue;

            int32_t ncx = FloorDiv(nx, 32);
            int32_t ncy = FloorDiv(ny, 256);
            int32_t ncz = FloorDiv(nz, 32);

            uint64_t nChunkIdx = encodeChunkCoord(ncx, ncy, ncz);

            auto nChunkIter = m_mapChunks.find(nChunkIdx);
            if (nChunkIter == m_mapChunks.end()) continue;

            int32_t nlx = nx - ncx * 32;
            int32_t nlz = nz - ncz * 32;
            CBlock3& nBlock = nChunkIter->second->GetBlock(nlx, ny, nlz);

            if (nBlock.IsOpaque()) continue;

            // -----------------------------------------------------------------
            // [수정 핵심] 다음 칸이 물(WATER)인지 공기(AIR)인지에 따른 감쇠 계산
            // -----------------------------------------------------------------
            uint8_t newLight = 0;

            // d == 3 (아래 방향)이고 현재 내 빛이 만땅(15)이면서, '다음 칸이 물이 아닐 때'만 직하강 노감쇠 적용
            if (d == 3 && curLight == 15 && !nBlock.IsWater())
            {
                newLight = 15;
            }
            else
            {
                // 다음 번져갈 칸이 물이면 3 감쇠, 일반 공기면 1 감쇠
                uint8_t attenuation = nBlock.IsWater() ? 3 : 1;

                if (curLight > attenuation)
                    newLight = curLight - attenuation;
                else
                    newLight = 0; // uint8 언더플로우 방지 (안전장치)
            }
            // -----------------------------------------------------------------

            if (newLight > nBlock.GetSkyLight())
            {
                nBlock.SetSkyLight(newLight);
                q.push({ { nx, ny, nz }, newLight });
            }
        }
    }
}

void CVoxelManager3::RuntimeOnBlockPlacedLighting(int32_t wbx, int32_t wby, int32_t wbz, uint8_t placedBlockEmitLight, uint8_t oldSkyLight, uint8_t oldBlockLight)
{
    constexpr int dx[] = { 1,-1, 0, 0, 0, 0 };
    constexpr int dy[] = { 0, 0, 1,-1, 0, 0 };
    constexpr int dz[] = { 0, 0, 0, 0, 1,-1 };

    auto optBlock = GetBlock(wbx, wby, wbz);
    if (!optBlock.has_value()) return;
    CBlock3 block = optBlock.value();

    // ----------------------------------------------------
    // 1. 새 블록 자체의 기본 조명 설정 (블록 내부는 빛을 가두거나 발산함)
    // ----------------------------------------------------
    block.SetSkyLight(0);
    block.SetBlockLight(placedBlockEmitLight); // 광원이면 자신의 밝기, 일반 블록이면 0
    SetBlock(wbx, wby, wbz, block);

    // ----------------------------------------------------
    // 2. SkyLight 처리 (빛 차단 및 사방 전파)
    // ----------------------------------------------------
    if (oldSkyLight > 0)
    {
        std::queue<std::pair<XMINT3, uint8_t>> skyLightRemovalQ;
        // 이 자리에 있었던 기존 SkyLight 값을 기준으로 주변 빛 청소 시작
        skyLightRemovalQ.push({ XMINT3{wbx, wby, wbz}, oldSkyLight });

        // SkyLight용 Removal 함수가 필요합니다. (아래 3번 참고)
        RuntimeRemoveSkyLighting(skyLightRemovalQ);
    }

    // ----------------------------------------------------
    // 3. BlockLight 처리 (기존 빛 차단 OR 새 광원 전파)
    // ----------------------------------------------------
    // Case A: 새로 설치된 블록이 광원인 경우 -> 사방으로 빛 확산
    if (placedBlockEmitLight > 0)
    {
        std::queue<std::pair<XMINT3, uint8_t>> blockLightQ;
        blockLightQ.push({ XMINT3{wbx, wby, wbz}, placedBlockEmitLight });
        RuntimeFloodFillBlockLighting(blockLightQ);
    }
    // Case B: 일반 블록이 설치되어 기존의 빛 줄기를 막은 경우 -> 주변 빛 청소
    else if (oldBlockLight > 0)
    {
        std::queue<std::pair<XMINT3, uint8_t>> blockLightRemovalQ;
        blockLightRemovalQ.push({ XMINT3{wbx, wby, wbz}, oldBlockLight });
        RuntimeRemoveBlockLighting(blockLightRemovalQ);
    }
}

void CVoxelManager3::Update(_float fTimeDelta)
{
    if (m_bDbgPicking)
    {
        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
        {
            if (auto cam = E::CGameInstance::Get().GetActiveGameCamera())
            {
                RECT rect;
                GetClientRect(CGameInstance::Get().GetHwnd(), &rect);

                E::_float4x4 P;
                XMStoreFloat4x4(&P, cam->GetProj());

                POINT mousePos;
                GetCursorPos(&mousePos);
                ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

                float ndcX = (2.0f * mousePos.x / (rect.right - rect.left)) - 1.0f;
                float ndcY = 1.0f - (2.0f * mousePos.y / (rect.bottom - rect.top));

                // 프로젝션 역변환
                _vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                _vector rayDir = XMVectorSet(
                    ndcX / P._11,
                    ndcY / P._22,
                    1.0f,
                    0.0f
                );

                // 뷰 역변환
                _matrix V = cam->GetView();
                auto detV = XMMatrixDeterminant(V);
                _matrix invView = XMMatrixInverse(&detV, V);

                rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
                rayDir = XMVector3TransformNormal(rayDir, invView);
                rayDir = XMVector3Normalize(rayDir);
                _float3 vecrayOrigin;
                _float3 vecrayDir;
                XMStoreFloat3(&vecrayOrigin, rayOrigin);
                XMStoreFloat3(&vecrayDir, XMVector3Normalize(rayDir));



                BLOCK_RAY_RESULT res;
                if (BlockRaycast(vecrayOrigin, vecrayDir, 15.f, res))
                {
                    //const auto& [cx, cy, cz] = res.pChunk->GetCoord();

                    int32_t worldBX = res.iWorldBlockX;
                    int32_t worldBY = res.iWorldBlockY;
                    int32_t worldBZ = res.iWorldBlockZ;

                    _vector verts[8] = {
                        XMVectorSet(worldBX,       worldBY,       worldBZ,       0),
                        XMVectorSet(worldBX + 1.f, worldBY,       worldBZ,       0),
                        XMVectorSet(worldBX + 1.f, worldBY + 1.f, worldBZ,       0),
                        XMVectorSet(worldBX,       worldBY + 1.f, worldBZ,       0),
                        XMVectorSet(worldBX,       worldBY,       worldBZ + 1.f, 0),
                        XMVectorSet(worldBX + 1.f, worldBY,       worldBZ + 1.f, 0),
                        XMVectorSet(worldBX + 1.f, worldBY + 1.f, worldBZ + 1.f, 0),
                        XMVectorSet(worldBX,       worldBY + 1.f, worldBZ + 1.f, 0),
                    };

                    struct Face { int i0, i1, i2, i3; _float3 normal; };
                    Face faces[6] = {
                        { 0, 3, 2, 1, {  0,  0, -1 } },  // -Z
                        { 4, 5, 6, 7, {  0,  0,  1 } },  // +Z
                        { 0, 1, 5, 4, {  0, -1,  0 } },  // -Y
                        { 3, 7, 6, 2, {  0,  1,  0 } },  // +Y
                        { 0, 4, 7, 3, { -1,  0,  0 } },  // -X
                        { 1, 2, 6, 5, {  1,  0,  0 } },  // +X
                    };

                    float tNearest = FLT_MAX;
                    _float3 hitNormal = {};

                    for (auto& face : faces)
                    {
                        float t = 0.f;
                        if (DirectX::TriangleTests::Intersects(
                            rayOrigin, rayDir,
                            verts[face.i0], verts[face.i1], verts[face.i2], t))
                        {
                            if (t < tNearest) { tNearest = t; hitNormal = face.normal; }
                        }
                        if (DirectX::TriangleTests::Intersects(
                            rayOrigin, rayDir,
                            verts[face.i0], verts[face.i2], verts[face.i3], t))
                        {
                            if (t < tNearest) { tNearest = t; hitNormal = face.normal; }
                        }
                    }

                    if (tNearest < FLT_MAX)
                    {
                        _float3 ro, rd;
                        XMStoreFloat3(&ro, rayOrigin);
                        XMStoreFloat3(&rd, rayDir);

                        _float3 hitPos = {
                            ro.x + rd.x * tNearest,
                            ro.y + rd.y * tNearest,
                            ro.z + rd.z * tNearest
                        };

                        OutputDebugStringA(std::format(
                            "Hit: ({:.2f}, {:.2f}, {:.2f}) Normal: ({:.0f}, {:.0f}, {:.0f})\n",
                            hitPos.x, hitPos.y, hitPos.z,
                            hitNormal.x, hitNormal.y, hitNormal.z
                        ).c_str());


                        {
                            E::CSkeletonEntity::DESC Desc{};
                            Desc.sObjectTag = "Skeleton";
                            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_SkeletonEntity",
                                "E::ETOUI(0)", &Desc))
                            {
                                if (auto skeleton = CGameInstance::Get().GetGameObjectByHandle(handle.value()))
                                {
                                    skeleton->GetTransform().SetPosition(XMVectorSet(hitPos.x, hitPos.y, hitPos.z, 1.f));
                                    skeleton->GetTransform().SetScale(XMVectorSet(0.1f, 0.1f, 0.1f, 1.f));
                                }
                                int x = 0;
                            }
                        }
                    }
                }

            }
        }
        
        
    }

    if (m_bDbgBlockPicking)
    {
        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
        {
            if (auto cam = E::CGameInstance::Get().GetActiveGameCamera())
            {
                const auto& [rayOrigin2, rayDir2] = cam->GetRay();

                BLOCK_RAY_RESULT res;
                if (BlockRaycast(rayOrigin2, rayDir2, 5.f, res))
                {
                    //const auto& [cx, cy, cz] = res.pChunk->GetCoord();

                    auto worldBX = res.iWorldBlockX;
                    auto worldBY = res.iWorldBlockY;
                    auto worldBZ = res.iWorldBlockZ;

                    

                    uint8_t oldBlockLight = res.block->GetBlockLight(); // 파괴 전 빛 값 백업
                    bool bIsLightSource = CBlock3::GetBlockLightByType(res.block->GetType()) > 0;

                    CBlock3 block{};
                    block.SetType(CBlock3::TYPE::AIR);
                    SetBlock(worldBX, worldBY, worldBZ, block);

                    RuntimeOnBlockRemovedLighting(worldBX, worldBY, worldBZ, bIsLightSource, oldBlockLight);
                }
            }
        }

        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            if (auto cam = E::CGameInstance::Get().GetActiveGameCamera())
            {
                const auto& [rayOrigin2, rayDir2] = cam->GetRay();
                BLOCK_RAY_RESULT res;
                if (BlockRaycast(rayOrigin2, rayDir2, 5.f, res))
                {
                    //const auto& [cx, cy, cz] = res.pChunk->GetCoord();

                    auto worldBX = res.iWorldBlockX;
                    auto worldBY = res.iWorldBlockY;
                    auto worldBZ = res.iWorldBlockZ;
                    if (res.eHitFace == FACE_DIR::POS_X)
                    {
                        worldBX += 1;
                    }
                    else if (res.eHitFace == FACE_DIR::NEG_X)
                    {
                        worldBX -= 1;
                    }
                    else if (res.eHitFace == FACE_DIR::POS_Y)
                    {
                        worldBY += 1;
                    }
                    else if (res.eHitFace == FACE_DIR::NEG_Y)
                    {
                        worldBY -= 1;
                    }
                    else if (res.eHitFace == FACE_DIR::POS_Z)
                    {
                        worldBZ += 1;
                    }
                    else if (res.eHitFace == FACE_DIR::NEG_Z)
                    {
                        worldBZ -= 1;
                    }



                    //CBlock3 block{};
                    //if (CGameInstance::Get().KeyPressing(DIK_L))
                    //{
                    //    block.SetType(CBlock3::TYPE::SAND);
                    //}
                    //else
                    //{
                    //    block.SetType(CBlock3::TYPE::DIRT);
                    //}


                    //SetBlock(worldBX, worldBY, worldBZ, block);





                    uint8_t oldSkyLight = 0;
                    uint8_t oldBlockLight = 0;
                    auto optPrev = GetBlock(worldBX, worldBY, worldBZ);
                    if (optPrev.has_value())
                    {
                        oldSkyLight = optPrev.value().GetSkyLight();
                        oldBlockLight = optPrev.value().GetBlockLight();
                    }

                    // 2. 실제 블록 배치 (예: 돌 블록이나 횃불 등)
                    CBlock3 newBlock{};
                    if (CGameInstance::Get().KeyPressing(DIK_L))
                    {
                        newBlock.SetType(CBlock3::TYPE::LOG_BIRCH);
                    }
                    else
                    {
                        newBlock.SetType(CBlock3::TYPE::COBBLED_DEEPSLATE);
                    }
                    SetBlock(worldBX, worldBY, worldBZ, newBlock);

                    // 3. 조명 함수 호출 (배치된 블록의 광원 수치도 함께 )
                    uint8_t placedBlockEmitLight = CBlock3::GetBlockLightByType(newBlock.GetType());
                    RuntimeOnBlockPlacedLighting(worldBX, worldBY, worldBZ, placedBlockEmitLight, oldSkyLight, oldBlockLight);
                }
            }
        }
    }

    // Chunk Update
    {
        for (auto& [idx, pChunk] : m_mapChunks)
        {
            pChunk->Update(fTimeDelta);
        }
    }




    ////////////////////
    _bool bHasInRangeChuneCreate = !m_queueInRangeChunkCreate.empty();
    _bool bHasOutRangeChunkRelease= !m_queueOutRangeChunkRelease.empty();
    if (bHasInRangeChuneCreate)
    {
        IN_RANGE_CHUNK_CREATE_DESC createDesc = m_queueInRangeChunkCreate.front();
        m_queueInRangeChunkCreate.pop_front();

        StartProcessInRangeChunkCreate(createDesc);
    }

    if (bHasOutRangeChunkRelease)
    {
        OUT_RANGE_CHUNK_RELEASE_DESC releaseDesc = m_queueOutRangeChunkRelease.front();
        m_queueOutRangeChunkRelease.pop_front();

        StartProcessOutRangeChunkRelease(releaseDesc);
    }

    UpdateCheckBlockFillingFutures();
    UpdateCheckLightingFutures();
    UpdateCheckBlockEdit();
    UpdateCheckQuduedQuadMessingChunk();
    UpdateCheckQuadMessingEndFutures();

    if (m_queueFutBlockFilling.empty()
        && m_queuedQuadMessingChunks.empty()
        && m_queueFutQuadMessing.empty()
        )
    {
        // TODO: flag check
        for (auto iter = m_mapChunks.begin(); iter != m_mapChunks.end();)
        {
            if (iter->second->GetDead())
            {
                iter = m_mapChunks.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
}

void CVoxelManager3::UpdateGUI()
{
    auto test = CChunk3::BlockIndexing(10, 10, 10);
    auto testres = CChunk3::BlockIndexDecoding(test);


    if (ImGui::Button("dbg block picking"))
    {
        m_bDbgBlockPicking = !m_bDbgBlockPicking;
    }

    if (ImGui::Button("dbg picking"))
    {
        m_bDbgPicking = !m_bDbgPicking;
    }

    if (ImGui::Button("SetChunkLoadCenter Cam"))
    {
        if (auto cam = CGameInstance::Get().GetActiveGameCamera())
        {
            float fx = cam->GetTransform().GetPosition().x;
            float fy = cam->GetTransform().GetPosition().y;
            float fz = cam->GetTransform().GetPosition().z;
            auto ix = (int32_t)floor(fx / VOXEL_CHUNK_X_SIZE3);
            auto iy = (int32_t)floor(fy / VOXEL_CHUNK_Y_SIZE3);
            auto iz = (int32_t)floor(fz / VOXEL_CHUNK_Z_SIZE3);

            int32_t cx = FloorDiv((int32_t)fx, (int32_t)VOXEL_CHUNK_X_SIZE3);
            int32_t cy = FloorDiv((int32_t)fy, (int32_t)VOXEL_CHUNK_Y_SIZE3);
            int32_t cz = FloorDiv((int32_t)fz, (int32_t)VOXEL_CHUNK_Z_SIZE3);

            IN_RANGE_CHUNK_CREATE_DESC desc{};
            desc.iCenterX = ix;
            desc.iCenterY = 0;
            desc.iCenterZ = iz;
            QueuingInRangeChunkCreate(desc);
        }
    }

    if (ImGui::Button("Release Cam"))
    {
        if (auto cam = CGameInstance::Get().GetActiveGameCamera())
        {
            float fx = cam->GetTransform().GetPosition().x;
            float fy = cam->GetTransform().GetPosition().y;
            float fz = cam->GetTransform().GetPosition().z;
            auto ix = (int32_t)floor(fx / VOXEL_CHUNK_X_SIZE3);
            auto iy = (int32_t)floor(fy / VOXEL_CHUNK_Y_SIZE3);
            auto iz = (int32_t)floor(fz / VOXEL_CHUNK_Z_SIZE3);

            int32_t cx = FloorDiv((int32_t)fx, (int32_t)VOXEL_CHUNK_X_SIZE3);
            int32_t cy = FloorDiv((int32_t)fy, (int32_t)VOXEL_CHUNK_Y_SIZE3);
            int32_t cz = FloorDiv((int32_t)fz, (int32_t)VOXEL_CHUNK_Z_SIZE3);

            OUT_RANGE_CHUNK_RELEASE_DESC desc{};
            desc.iCenterX = ix;
            desc.iCenterY = 0;
            desc.iCenterZ = iz;
            QueuingOutRangeChunkRelease(desc);
        }
    }
}

uint64_t CVoxelManager3::encodeChunkCoord(int32_t x, int32_t y, int32_t z) 
{
	// X: 21비트 (±1,048,575 청크 ≈ ±33.5 million 블록)
	// Y: 22비트 (±2,097,151 청크 ≈ ±67 million 블록) → Y축은 보통 더 넓게
	// Z: 21비트

	constexpr int64_t X_OFFSET = 1 << 20;   // 2^20 = 1048576
	constexpr int64_t Y_OFFSET = 1 << 21;
	constexpr int64_t Z_OFFSET = 1 << 20;

	uint64_t ux = static_cast<uint64_t>(x + X_OFFSET) & 0x1FFFFF;   // 21비트
	uint64_t uy = static_cast<uint64_t>(y + Y_OFFSET) & 0x3FFFFF;   // 22비트
	uint64_t uz = static_cast<uint64_t>(z + Z_OFFSET) & 0x1FFFFF;   // 21비트

	return (ux << 43) | (uy << 21) | uz;
}

std::tuple<int32_t, int32_t, int32_t> CVoxelManager3::decodeChunkCoord(uint64_t key) 
{
	constexpr int64_t X_OFFSET = 1 << 20;
	constexpr int64_t Y_OFFSET = 1 << 21;
	constexpr int64_t Z_OFFSET = 1 << 20;

	int32_t x = static_cast<int32_t>((key >> 43) & 0x1FFFFF) - X_OFFSET;
	int32_t y = static_cast<int32_t>((key >> 21) & 0x3FFFFF) - Y_OFFSET;
	int32_t z = static_cast<int32_t>(key & 0x1FFFFF) - Z_OFFSET;

	return { x, y, z };
}
CChunk3* CVoxelManager3::GetChunkByChunkCoord(int32_t x, int32_t y, int32_t z) const
{
	auto idx = encodeChunkCoord(x, y, z);
	auto iter = m_mapChunks.find(idx);
	if (iter == m_mapChunks.end())
	{
		return nullptr;
	}

    if (iter->second->GetDead())
    {
        return nullptr;
    }

	return iter->second.get();
}

CChunk3* CVoxelManager3::GetChunkByWorldBlockCoord(int32_t x, int32_t y, int32_t z) const
{
    auto tmp1 = FloorDiv(x, VOXEL_CHUNK_X_SIZE3);
    auto tmp2 = FloorDiv(y, VOXEL_CHUNK_Y_SIZE3);
    auto tmp3 = FloorDiv(z, VOXEL_CHUNK_Z_SIZE3);
	int32_t cx = (int32_t)floor(x / VOXEL_CHUNK_X_SIZE3);
	int32_t cy = (int32_t)floor(y / VOXEL_CHUNK_Y_SIZE3);
	int32_t cz = (int32_t)floor(z / VOXEL_CHUNK_Z_SIZE3);
	return GetChunkByChunkCoord(tmp1, tmp2, tmp3);
}

HRESULT CVoxelManager3::StartProcessInRangeChunkCreate(const IN_RANGE_CHUNK_CREATE_DESC& createDesc)
{
    // 생성자체를 워커에서해도 갑자기 폭발하면 워커가 가득차버리니까
    // 근본적인 해결은 아님
    // 추후에 그냥 조금씩 워커에 던지는 방향으로 개발 필요
    if (!m_bCreating)
    {
        m_bCreating = true;
    }
    else
    {
        return S_OK;
    }
    CGameInstance::Get().ChunkLoadWorkerEnqueue("TMP", [=]() {
        
        
        int32_t minX = createDesc.iCenterX - m_iRenderDistance;
        int32_t maxX = createDesc.iCenterX + m_iRenderDistance;
        int32_t minZ = createDesc.iCenterZ - m_iRenderDistance;
        int32_t maxZ = createDesc.iCenterZ + m_iRenderDistance;

        //// Y축 범위 (Vertical Render Distance)
        //int32_t verticalDistance = m_iVerticalRenderDistance;  // 새로 추가 추천
        int32_t minY = createDesc.iCenterY - m_iVerticalRenderDistance;
        int32_t maxY = createDesc.iCenterY + m_iVerticalRenderDistance;

        struct ChunkPos {
            int x, y, z;
            int dist; // 거리 (맨해튼 or 제곱 거리)
        };

        std::vector<ChunkPos> list;

        for (int32_t x = minX; x <= maxX; ++x)
            for (int32_t y = minY; y <= maxY; ++y)
                for (int32_t z = minZ; z <= maxZ; ++z)
                {
                    uint64_t chunkCoord = encodeChunkCoord(x, y, z);
                    if (m_mapChunks.find(chunkCoord) != m_mapChunks.end())
                    {
                        continue;
                    }

                    int dx = x - createDesc.iCenterX;
                    int dy = y - createDesc.iCenterY;
                    int dz = z - createDesc.iCenterZ;

                    int dist = dx * dx + dy * dy + dz * dz; // 제곱 거리 (빠름)

                    list.push_back({ x, y, z, dist });
                }

        // 중심부터 가까운 순으로 정렬
        std::sort(list.begin(), list.end(), [](const ChunkPos& a, const ChunkPos& b) {
            return a.dist < b.dist;
            });

        std::vector<std::future<uint64_t>>  futvec{};
        for (auto& p : list)
        {
            uint64_t chunkCoord = encodeChunkCoord(p.x, p.y, p.z);
            CChunk3::DESC chunkDesc{};
            chunkDesc.iX = p.x;
            chunkDesc.iY = p.y;
            chunkDesc.iZ = p.z;
            chunkDesc.iChunkCoord = chunkCoord;
            m_mapChunks.emplace(chunkCoord, std::move(CChunk3::Create(chunkDesc)));


            std::future<uint64_t> fut = CGameInstance::Get().ChunkLoadWorkerEnqueueWithFuture("FUT_BLOCK_FILLING", [this, chunkCoord]()->uint64_t {
                auto iter = m_mapChunks.find(chunkCoord);
                if (iter != m_mapChunks.end())
                {
                    if (FAILED(iter->second->BlockFilling()))
                    {
                        MSG_BOX("FUT_BLOCK_FILLING FAIL");
                    }

                }
                else
                {
                    MSG_BOX("FUT_BLOCK_FILLING CHUNK NOT FOUND");
                }

                return chunkCoord;
                });

            futvec.push_back(std::move(fut));
        }

        m_queueFutBlockFilling.push_back(std::move(futvec));
        
        
        
        m_bCreating = false;
        });
    
    return S_OK;
}

HRESULT CVoxelManager3::StartProcessOutRangeChunkRelease(const OUT_RANGE_CHUNK_RELEASE_DESC& releaseDesc)
{
    std::vector<uint64_t> delQ;
    {
        int32_t minX = releaseDesc.iCenterX - m_iRenderDistance;
        int32_t maxX = releaseDesc.iCenterX + m_iRenderDistance;
        int32_t minZ = releaseDesc.iCenterZ - m_iRenderDistance;
        int32_t maxZ = releaseDesc.iCenterZ + m_iRenderDistance;

        //// Y축 범위 (Vertical Render Distance)
        //int32_t verticalDistance = m_iVerticalRenderDistance;  // 새로 추가 추천
        int32_t minY = releaseDesc.iCenterY - m_iVerticalRenderDistance;
        int32_t maxY = releaseDesc.iCenterY + m_iVerticalRenderDistance;

        for (const auto& [coord, pChunk] : m_mapChunks)
        {
            if (!pChunk->GetDead())
            {
                auto [x, y, z] = decodeChunkCoord(coord);

                // X, Z, Y 모두 체크
                if (x < minX || x > maxX ||
                    z < minZ || z > maxZ ||
                    y < minY || y > maxY)
                {
                    delQ.push_back(coord);
                    pChunk->SetDead();
                }
            }
            
        }
    }

    std::unordered_set<uint64_t> setIdx{};
    for (const auto& delIdx : delQ)
    {
        const auto& [cx, cy, cz] = decodeChunkCoord(delIdx);

        setIdx.insert(delIdx);

        //POS_X
        {
            uint64_t targetCoord = encodeChunkCoord(cx + 1, cy, cz);
            auto iter = m_mapChunks.find(targetCoord);
            if (iter != m_mapChunks.end() && !iter->second->GetDead())
            {
                setIdx.insert(targetCoord);
            }
        }

        //NEG_X
        {
            uint64_t targetCoord = encodeChunkCoord(cx - 1, cy, cz);
            auto iter = m_mapChunks.find(targetCoord);
            if (iter != m_mapChunks.end() && !iter->second->GetDead())
            {
                setIdx.insert(targetCoord);
            }
        }

        //POS_Z
        {
            uint64_t targetCoord = encodeChunkCoord(cx, cy, cz + 1);
            auto iter = m_mapChunks.find(targetCoord);
            if (iter != m_mapChunks.end() && !iter->second->GetDead())
            {
                setIdx.insert(targetCoord);
            }
        }

        //NEG_Z
        {
            uint64_t targetCoord = encodeChunkCoord(cx, cy, cz - 1);
            auto iter = m_mapChunks.find(targetCoord);
            if (iter != m_mapChunks.end() && !iter->second->GetDead())
            {
                setIdx.insert(targetCoord);
            }
        }
    }

    for (const auto& targetCoord : setIdx)
    {
        std::vector<uint64_t> targetCoords{};
        targetCoords.push_back(targetCoord);
        QueueingQuadMessing(targetCoords);
    }


    return S_OK;
}

HRESULT CVoxelManager3::UpdateCheckBlockFillingFutures()
{
    for (auto iter = m_queueFutBlockFilling.begin(); iter != m_queueFutBlockFilling.end();)
    {
        // 라이팅은 오직 하나의 워커에서만
        if (m_bLighing)
        {
            break;
        }

        _bool bErase = false;

        size_t validCnt = iter->size();
        size_t cnt{};
        std::vector<uint64_t> fillingEndChunks{};
        for ( std::future<uint64_t>& futChunkIdx : *iter)
        {
            if (futChunkIdx.valid())
            {
                if (futChunkIdx.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    ++cnt;
                }
            }
        }

        if (validCnt == cnt)
        {
            bErase = true;

            std::unordered_set<uint64_t> setIdx{};
            
            for (std::future<uint64_t>& futChunkIdx : *iter)
            {
                uint64_t chunkIdx = futChunkIdx.get();
                // check 인접 청크
                // 최적화 필요함, 실제 반영이 필요한인접청크만

                const auto& [cx, cy, cz] = decodeChunkCoord(chunkIdx);

                setIdx.insert(chunkIdx);

                //POS_X, POS_Z
                {
                    uint64_t targetCoord = encodeChunkCoord(cx + 1, cy, cz + 1);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }

                //POS_X, NEG_Z
                {
                    uint64_t targetCoord = encodeChunkCoord(cx + 1, cy, cz - 1);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }

                //NEG_X, POS_Z
                {
                    uint64_t targetCoord = encodeChunkCoord(cx - 1, cy, cz + 1);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }

                //NEG_X, NEG_Z
                {
                    uint64_t targetCoord = encodeChunkCoord(cx - 1, cy, cz - 1);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }

                //POS_X
                {
                    uint64_t targetCoord = encodeChunkCoord(cx + 1, cy, cz);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }

                //NEG_X
                {
                    uint64_t targetCoord = encodeChunkCoord(cx - 1, cy, cz);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }

                //POS_Z
                {
                    uint64_t targetCoord = encodeChunkCoord(cx, cy, cz + 1);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }

                //NEG_Z
                {
                    uint64_t targetCoord = encodeChunkCoord(cx, cy, cz - 1);
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end() && !iter->second->GetDead())
                    {
                        setIdx.insert(targetCoord);
                    }
                }
            }

            if constexpr (false)
            {
                for (const auto& targetCoord : setIdx)
                {
                    std::vector<uint64_t> targetCoords{};
                    targetCoords.push_back(targetCoord);
                    QueueingQuadMessing(targetCoords);
                }
            }

            m_bLighing = true;
            for (const auto& targetCoord : setIdx)
            {
                auto iter = m_mapChunks.find(targetCoord);
                if (iter != m_mapChunks.end())
                {
                    iter->second->SetLightingState(CChunk3::LIGHTING_STATE::ING);
                }
            }


            std::unordered_set<uint64_t> chunkIdxLookupBundle = setIdx;
            std::future<std::unordered_set<uint64_t>> fut = CGameInstance::Get().ChunkLoadWorkerEnqueueWithFuture(
                "FUT_LIGHTING",
                [this, chunkIdxLookupBundle]()->std::unordered_set<uint64_t>
                {
                    std::queue<std::pair<XMINT3, uint8_t>> skyLightSeedQ;
                    std::queue<std::pair<XMINT3, uint8_t>> blockLightSeedQ;

                    // 이번 번들에 묶인 청크들을 순회하며 조명 시작점 수집
                    for (uint64_t chunkIdx : chunkIdxLookupBundle)
                    {
                        auto chunkIter = m_mapChunks.find(chunkIdx);
                        if (chunkIter == m_mapChunks.end()) continue;

                        CChunk3* pChunk = chunkIter->second.get();
                        const auto& [cx, cy, cz] = decodeChunkCoord(chunkIdx);

                        int32_t worldXOffset = cx * 32;
                        int32_t worldZOffset = cz * 32;

                        // 1. 스카이라이트 고속 수직 낙하 스캔 및 지표면/수중 시드 수집
                        for (int32_t x = 0; x < 32; ++x)
                        {
                            for (int32_t z = 0; z < 32; ++z)
                            {
                                uint8_t currentLight = 15; // 하늘 최상단은 무조건 15로 시작

                                for (int32_t y = 255; y >= 0; --y)
                                {
                                    CBlock3& block = pChunk->GetBlock(x, y, z);

                                    if (block.IsOpaque())
                                    {
                                        // 고체 땅을 만나면 바로 직전 칸(공기나 물)을 시드로 집어넣고 아래는 스캔 중단
                                        if (y < 255 && currentLight > 0)
                                        {
                                            skyLightSeedQ.push({ { worldXOffset + x, y + 1, worldZOffset + z }, currentLight });
                                        }
                                        break;
                                    }

                                    // [추가] 물을 만나면 한 칸당 3씩 빛을 깎아내림
                                    if (block.IsWater())
                                    {
                                        if (currentLight > 3)
                                            currentLight -= 3;
                                        else
                                            currentLight = 0;
                                    }
                                    // 공기(AIR)라면 기존의 currentLight 강도를 그대로 유지(감쇠 없음)

                                    // 현재 계산된 빛 값을 블록에 저장
                                    block.SetSkyLight(currentLight);

                                    // [핵심] 물 속이거나, 공기 중에서 빛이 꺾이기 시작하는 지점(감쇠가 일어난 지점)들은 
                                    // 전부 이웃 청크나 옆 칸으로 빛을 전파해야 하므로 FloodFill 큐에 시드로 추가합니다.
                                    if (block.IsWater() || currentLight < 15)
                                    {
                                        if (currentLight > 0)
                                        {
                                            skyLightSeedQ.push({ { worldXOffset + x, y, worldZOffset + z }, currentLight });
                                        }
                                    }

                                    // 최하단 바닥(y == 0)까지 빛이 내려왔다면 마지막으로 시드 추가
                                    if (y == 0 && currentLight > 0)
                                    {
                                        skyLightSeedQ.push({ { worldXOffset + x, 0, worldZOffset + z }, currentLight });
                                    }
                                }
                            }
                        }

                        // 1. 내 청크 내부 자체 광원 수집 (기존 루프 - 심플하게 유지)
                        for (int32_t x = 0; x < 32; ++x)
                        {
                            for (int32_t z = 0; z < 32; ++z)
                            {
                                for (int32_t y = 255; y >= 0; --y)
                                {
                                    CBlock3& block = pChunk->GetBlock(x, y, z);
                                    uint8_t emitLight = CBlock3::GetBlockLightByType(block.GetType());
                                    if (emitLight > 0)
                                    {
                                        block.SetBlockLight(emitLight);
                                        blockLightSeedQ.push({ { worldXOffset + x, y, worldZOffset + z }, emitLight });
                                    }
                                }
                            }
                        }

                        //  주변 8방향 청크의 경계면 라이팅을 그대로 내 시드로 흡수
                        for (int32_t offsetX = -1; offsetX <= 1; ++offsetX)
                        {
                            for (int32_t offsetZ = -1; offsetZ <= 1; ++offsetZ)
                            {
                                if (offsetX == 0 && offsetZ == 0) continue; // 나 자신은 제외

                                int32_t ncx = cx + offsetX;
                                int32_t ncz = cz + offsetZ;
                                uint64_t nChunkIdx = encodeChunkCoord(ncx, cy, ncz); // Y축은 같은 층(cy) 기준

                                // 주변 이웃 청크가 이미 세상에 로드되어 불이 켜져 있는 상태라면?
                                auto nChunkIter = m_mapChunks.find(nChunkIdx);
                                if (nChunkIter != m_mapChunks.end())
                                {
                                    CChunk3* pAdjChunk = nChunkIter->second.get();

                                    // 💡 이웃 청크에서 "우리 청크와 딱 맞닿아 있는 경계 영역"의 로컬 범위를 지정합니다.
                                    // 대각선 꼭짓점 청크라면 딱 1칸짜리 꼭짓점 기둥만, 직선 청크라면 32칸짜리 한 면만 정밀 타격합니다.
                                    int32_t adjXStart = (offsetX == 1) ? 0 : ((offsetX == -1) ? 31 : 0);
                                    int32_t adjXEnd = (offsetX == 1) ? 0 : ((offsetX == -1) ? 31 : 31);
                                    int32_t adjZStart = (offsetZ == 1) ? 0 : ((offsetZ == -1) ? 31 : 0);
                                    int32_t adjZEnd = (offsetZ == 1) ? 0 : ((offsetZ == -1) ? 31 : 31);

                                    for (int32_t ax = adjXStart; ax <= adjXEnd; ++ax)
                                    {
                                        for (int32_t az = adjZStart; az <= adjZEnd; ++az)
                                        {
                                            // 💡 상하 전파 사각지대를 없애기 위해, 내 Y층 기준 위아래 삼중창(ay-1, ay, ay+1) 전파를 수용하도록 설계할 수 있습니다.
                                            // (이웃 청크의 해당 좌표에 빛이 존재한다면, 6방향 플러드 필이 내 청크 안쪽으로 밀고 들어오는 시드가 됩니다.)
                                            for (int32_t ay = 0; ay < 256; ++ay)
                                            {
                                                CBlock3& adjBlock = pAdjChunk->GetBlock(ax, ay, az);
                                                uint8_t adjLight = adjBlock.GetBlockLight();

                                                // 이웃 청크 경계면에 빛이 켜져 있고 투명하다면 내 큐에 시드로 복사!
                                                if (adjLight > 1 && !adjBlock.IsOpaque())
                                                {
                                                    int32_t adjWorldX = ncx * 32 + ax;
                                                    int32_t adjWorldZ = ncz * 32 + az;

                                                    // 이웃 청크의 그 불 켜진 좌표 자체를 전파용 시드 큐에 그대로 집어넣습니다.
                                                    blockLightSeedQ.push({ { adjWorldX, ay, adjWorldZ }, adjLight });
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // 3. 포인터 캐싱 최적화가 완비된 워커 조명 전파 가동
                    WorkerFloodFillSkyLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), skyLightSeedQ);
                    WorkerFloodFillBlockLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), blockLightSeedQ);

                    return chunkIdxLookupBundle;
                }
            );

            //std::unordered_set<uint64_t> chunkIdxLookupBundle = setIdx;
            //std::future<std::unordered_set<uint64_t>> fut = CGameInstance::Get().ChunkLoadWorkerEnqueueWithFuture(
            //    "FUT_LIGHTING",
            //    [this, chunkIdxLookupBundle]()->std::unordered_set<uint64_t>
            //    {
            //        std::queue<std::pair<XMINT3, uint8_t>> skyLightSeedQ;

            //        for (uint64_t chunkIdx : chunkIdxLookupBundle)
            //        {
            //            auto chunkIter = m_mapChunks.find(chunkIdx);
            //            if (chunkIter == m_mapChunks.end()) continue;

            //            CChunk3* pChunk = chunkIter->second.get();
            //            const auto& [cx, cy, cz] = decodeChunkCoord(chunkIdx);

            //            int32_t worldXOffset = cx * 32;
            //            int32_t worldZOffset = cz * 32;

            //            // X, Z 평면을 돌며 수직으로 빛을 먼저 내리꽂습니다.
            //            for (int32_t x = 0; x < 32; ++x)
            //            {
            //                for (int32_t z = 0; z < 32; ++z)
            //                {
            //                    // 맨 위(255)부터 아래로 내려가며 불투명 블록을 찾습니다.
            //                    for (int32_t y = 255; y >= 0; --y)
            //                    {
            //                        CBlock3& block = pChunk->GetBlock(x, y, z);

            //                        if (block.IsOpaque())
            //                        {
            //                            // 불투명 블록(땅)을 만났다면, 바로 그 윗칸(공기)이 사방으로 빛이 번지는 시작점입니다.
            //                            if (y < 255)
            //                            {
            //                                skyLightSeedQ.push({ { worldXOffset + x, y + 1, worldZOffset + z }, 15 });
            //                            }
            //                            break; // 이 칼럼은 땅 내부이므로 수직 낙하 스캔 중단
            //                        }

            //                        // 땅을 만나기 전까지는 큐 연산 없이 다이렉트로 빛을 15로 채웁니다.
            //                        block.SetSkyLight(15);

            //                        // 만약 Y=0 바닥까지 공기라면 맨 바닥을 시드로 등록
            //                        if (y == 0)
            //                        {
            //                            skyLightSeedQ.push({ { worldXOffset + x, 0, worldZOffset + z }, 15 });
            //                        }
            //                    }
            //                }
            //            }
            //        }

            //        // 지표면 주변으로만 번지는 6방향 플러드 필 가동 (연산 횟수가 90% 이상 감소함)
            //        WorkerFloodFillSkyLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), skyLightSeedQ);

            //        return chunkIdxLookupBundle;
            //    }
            //);


            //std::unordered_set<uint64_t> chunkIdxLookupBundle = setIdx;
            //std::future<std::unordered_set<uint64_t>> fut = CGameInstance::Get().ChunkLoadWorkerEnqueueWithFuture(
            //    "FUT_LIGHTING",
            //    [this, chunkIdxLookupBundle]()->std::unordered_set<uint64_t>
            //    {
            //        std::queue<std::pair<XMINT3, uint8_t>> skyLightSeedQ;
            //        std::queue<std::pair<XMINT3, uint8_t>> blockLightSeedQ;

            //        // 이번 번들에 묶인 청크들을 순회하며 조명 시작점 수집
            //        for (uint64_t chunkIdx : chunkIdxLookupBundle)
            //        {
            //            auto chunkIter = m_mapChunks.find(chunkIdx);
            //            if (chunkIter == m_mapChunks.end()) continue;

            //            CChunk3* pChunk = chunkIter->second.get();

            //            // [버그 예방]: GetCoord 대신 기존 검증된 매니저의 디코더 사용
            //            const auto& [cx, cy, cz] = decodeChunkCoord(chunkIdx);

            //            // 청크의 월드 기준 X, Z 시작 오프셋 계산
            //            int32_t worldXOffset = cx * 32;
            //            int32_t worldZOffset = cz * 32;

            //            // 1. 스카이라이트 시드: 청크의 맨 꼭대기 층(Y=255) 평면 전체를 시드로 등록
            //            for (int32_t x = 0; x < 32; ++x)
            //            {
            //                for (int32_t z = 0; z < 32; ++z)
            //                {
            //                    CBlock3& topBlock = pChunk->GetBlock(x, 255, z);
            //                    if (!topBlock.IsOpaque())
            //                    {
            //                        topBlock.SetSkyLight(15);
            //                        skyLightSeedQ.push({ { worldXOffset + x, 255, worldZOffset + z }, 15 });
            //                    }
            //                }
            //            }

            //            // 2. 블록라이트 시드: 청크 전체(Y: 0~255)를 돌며 자체 광원 블록 수집
            //            for (int32_t x = 0; x < 32; ++x)
            //            {
            //                for (int32_t z = 0; z < 32; ++z)
            //                {
            //                    for (int32_t y = 255; y >= 0; --y)
            //                    {
            //                        CBlock3& block = pChunk->GetBlock(x, y, z);
            //                        uint8_t emitLight = 0; // 자체 광원 수치 기믹 생략 시 0 유지

            //                        if (emitLight > 0)
            //                        {
            //                            block.SetBlockLight(emitLight);
            //                            blockLightSeedQ.push({ { worldXOffset + x, y, worldZOffset + z }, emitLight });
            //                        }
            //                    }
            //                }
            //            }
            //        }

            //        // 3. 2중 나눗셈 버그가 수정된 워커 조명 함수 가동
            //        WorkerFloodFillSkyLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), skyLightSeedQ);

            //        return chunkIdxLookupBundle;
            //    }
            //);
            


            //std::unordered_set<uint64_t> chunkIdxLookupBundle = setIdx;
            //std::future<std::unordered_set<uint64_t>> fut = CGameInstance::Get().ChunkLoadWorkerEnqueueWithFuture(
            //    "FUT_LIGHTING",
            //    [this, chunkIdxLookupBundle]()->std::unordered_set<uint64_t>
            //    {
            //        std::queue<std::pair<XMINT3, uint8_t>> skyLightSeedQ;
            //        std::queue<std::pair<XMINT3, uint8_t>> blockLightSeedQ;

            //        // 이번 번들에 묶인 청크들을 순회하며 조명 시작점 수집
            //        for (uint64_t chunkIdx : chunkIdxLookupBundle)
            //        {
            //            auto chunkIter = m_mapChunks.find(chunkIdx);
            //            if (chunkIter == m_mapChunks.end()) continue;

            //            CChunk3* pChunk = chunkIter->second.get();
            //            const auto&[cx, _, cz] = pChunk->GetCoord();
            //            // 청크의 월드 기준 X, Z 시작 오프셋 계산
            //            //int32_t cx = static_cast<int32_t>(chunkIdx >> 32);
            //            //int32_t cz = static_cast<int32_t>(chunkIdx & 0xFFFFFFFF);
            //            int32_t worldXOffset = cx * 32;
            //            int32_t worldZOffset = cz * 32;

            //            // 1. 스카이라이트 시드: 청크의 맨 꼭대기 층(Y=255) 평면 전체를 시드로 등록
            //            for (int32_t x = 0; x < 32; ++x)
            //            {
            //                for (int32_t z = 0; z < 32; ++z)
            //                {
            //                    CBlock3& topBlock = pChunk->GetBlock(x, 255, z);
            //                    if (!topBlock.IsOpaque())
            //                    {
            //                        topBlock.SetSkyLight(15);
            //                        skyLightSeedQ.push({ { worldXOffset + x, 255, worldZOffset + z }, 15 });
            //                    }
            //                }
            //            }

            //            // 2. 블록라이트 시드: 청크 전체(Y: 0~255)를 돌며 자체 광원 블록(횃불 등) 수집
            //            for (int32_t x = 0; x < 32; ++x)
            //            {
            //                for (int32_t z = 0; z < 32; ++z)
            //                {
            //                    for (int32_t y = 255; y >= 0; --y)
            //                    {
            //                        CBlock3& block = pChunk->GetBlock(x, y, z);

            //                        // [프로젝트 규칙에 맞게 광원 블록 정보 추출]
            //                        // 만약 블록 자체 발광 수치를 가져오는 기믹이 있다면 세팅합니다.
            //                        // 예: uint8_t emitLight = block.GetEmitLightValue();
            //                        uint8_t emitLight = 0; // 임시 가이드용 (자체 광원이 없다면 생략 가능)

            //                        if (emitLight > 0)
            //                        {
            //                            block.SetBlockLight(emitLight);
            //                            blockLightSeedQ.push({ { worldXOffset + x, y, worldZOffset + z }, emitLight });
            //                        }
            //                    }
            //                }
            //            }
            //        }

            //        // 3. Y=256 범위 가드가 탑재된 워커 조명 함수 가동
            //        WorkerFloodFillSkyLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), skyLightSeedQ);
            //        //WorkerFloodFillBlockLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), blockLightSeedQ);

            //        return chunkIdxLookupBundle;
            //    }
            //);

            //std::unordered_set<uint64_t> chunkIdxLookupBundle = setIdx;
            //std::future<std::unordered_set<uint64_t>> fut = CGameInstance::Get().ChunkLoadWorkerEnqueueWithFuture("FUT_LIGHTING", [this, chunkIdxLookupBundle]()->std::unordered_set<uint64_t> {
            //    

            //    // 조명 연산을 시작할 시드 큐 준비
            //    std::queue<std::pair<XMINT3, uint8_t>> skyLightSeedQ;
            //    std::queue<std::pair<XMINT3, uint8_t>> blockLightSeedQ;

            //    // 1. 이번 번들에 포함된 청크들로부터 초기 빛 원천(시드)들을 싹 다 긁어모읍니다.
            //    for (uint64_t chunkIdx : chunkIdxLookupBundle)
            //    {
            //        auto chunkIter = m_mapChunks.find(chunkIdx);
            //        if (chunkIter != m_mapChunks.end())
            //        {
            //            // [필수 작업]: CChunk3 내부에 우리가 구상했던 CollectLightingSeeds 구현체를 호출해 줍니다.
            //            // 월드 좌표 기준으로 전파 큐를 채워주는 함수입니다.
            //            chunkIter->second->CollectLightingSeeds(skyLightSeedQ, blockLightSeedQ);
            //        }
            //    }

            //    // 2. 묶어온 청크 범위 내에서만 안전하게 사방 전파 실행
            //    WorkerFloodFillSkyLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), skyLightSeedQ);
            //    WorkerFloodFillBlockLighting(const_cast<std::unordered_set<uint64_t>&>(chunkIdxLookupBundle), blockLightSeedQ);

            //    // 연산이 무사히 완료된 번들 인덱스셋 반환
            //    return chunkIdxLookupBundle;
            //    });

            m_futLighting = std::move(fut);
        }


        if (bErase)
        {
            iter = m_queueFutBlockFilling.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
    return S_OK;
}

HRESULT CVoxelManager3::UpdateCheckQuduedQuadMessingChunk()
{
    for (auto iter = m_queuedQuadMessingChunks.begin(); iter != m_queuedQuadMessingChunks.end(); )
    {
        _bool bNeedContinue = false;
        std::vector<uint64_t>& targetCoords = *iter;
        for (auto innerIter = targetCoords.begin(); innerIter != targetCoords.end();)
        {
            auto chunkFindIter = m_mapChunks.find(*innerIter);
            if (chunkFindIter != m_mapChunks.end())
            {
                if (chunkFindIter->second->GetMessingState() == CChunk3::MESSING_STATE::ING
                    || chunkFindIter->second->GetLightingState() == CChunk3::LIGHTING_STATE::ING)
                {
                    bNeedContinue = true;
                    break;
                }
                else if (chunkFindIter->second->GetDead())
                {
                    innerIter = targetCoords.erase(innerIter);
                }
                else
                {
                    ++innerIter;
                }
            }
            else
            {
                innerIter = targetCoords.erase(innerIter);
            }
        }

        if (bNeedContinue)
        {
            ++iter;
            continue;
        }


        if (!targetCoords.empty())
        {
            std::future<std::vector<uint64_t>> fut = CGameInstance::Get().ChunkLoadWorkerEnqueueWithFuture("FUT_QUAD_MESSING", [this, targetCoords]()->std::vector<uint64_t> {
                for (const auto& targetCoord : targetCoords)
                {
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end())
                    {
                        if (FAILED(iter->second->QuadMessing()))
                        {
                            MSG_BOX("FUT_QUAD_MESSING FAIL");
                        }
                    }
                    else
                    {
                        MSG_BOX("FUT_QUAD_MESSING CHUNK NOT FOUND");
                    }
                }

                return targetCoords;
                });
            m_queueFutQuadMessing.push_back(std::move(fut));
        }
        


        iter = m_queuedQuadMessingChunks.erase(iter);
    }

    return S_OK;
}

HRESULT CVoxelManager3::UpdateCheckQuadMessingEndFutures()
{
    for (auto iter = m_queueFutQuadMessing.begin(); iter != m_queueFutQuadMessing.end();)
    {
        if (iter->valid())
        {
            if (iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                std::vector<uint64_t> targetCoords = iter->get();
                
                for (const auto& targetCoord : targetCoords)
                {
                    auto findIter = m_mapChunks.find(targetCoord);
                    if (findIter != m_mapChunks.end())
                    {
                        findIter->second->SetMessingQueued(false);
                        if (FAILED(findIter->second->CreateBuffer()))
                        {
                            MSG_BOX("UpdateCheckQuadMessingFutures CreateBuffer FAILEd");
                        }
                    }
                }

                iter = m_queueFutQuadMessing.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        else
        {
            MSG_BOX("UpdateCheckQuadMessingFutures nonvalid");
            iter = m_queueFutQuadMessing.erase(iter);
        }
    }
    return S_OK;
}

HRESULT CVoxelManager3::UpdateCheckBlockEdit()
{
    std::unordered_set<uint64_t> setReBuildChunks{};
    std::vector<std::vector<uint64_t>> vecReqMessingChunks{};

    for (auto iter = m_EditShadow.begin(); iter != m_EditShadow.end(); )
    {
        uint64_t chunkIdx = iter->first;
        auto chunkFindIter = m_mapChunks.find(chunkIdx);
        if (chunkFindIter != m_mapChunks.end())
        {
            if (chunkFindIter->second->GetMessingQueued())
            {
                ++iter;
                continue;
            }

            _bool bRePosX{ false };
            _bool bReNegX{ false };
            _bool bRePosZ{ false };
            _bool bReNegZ{ false };

            auto& BlockChanges = iter->second;
            for (const auto& [blockIdx, block] : BlockChanges)
            {
                const auto& [cbx, cby, cbz] = CChunk3::BlockIndexDecoding(blockIdx);

                if (cbx == 0)
                {
                    bReNegX = true;
                }
                else if (cbx == VOXEL_CHUNK_X_SIZE3 - 1)
                {
                    bRePosX = true;
                }

                if (cbz == 0)
                {
                    bReNegZ = true;
                }
                else if (cbz == VOXEL_CHUNK_Z_SIZE3 - 1)
                {
                    bRePosZ = true;
                }
            }

            auto funcHelperChunkMessingQueued = [&](uint64_t chunkIdx)->std::optional<_bool>
                {
                    auto targetIter = m_mapChunks.find(chunkIdx);
                    if (targetIter != m_mapChunks.end())
                    {
                        if (targetIter->second->GetLightingState() == CChunk3::LIGHTING_STATE::ING)
                        {
                            return true;
                        }

                        if (targetIter->second->GetMessingQueued())
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    return std::nullopt;
                };

            const auto& [cx, cy, cz] = decodeChunkCoord(chunkIdx);
            std::vector<uint64_t> messingReqVec{};

            // RF
            if (bRePosX && bRePosZ)
            {
                uint64_t RIdx = encodeChunkCoord(cx + 1, cy, cz);
                uint64_t FIdx = encodeChunkCoord(cx, cy, cz + 1);
                uint64_t RFIdx = encodeChunkCoord(cx + 1, cy, cz + 1);
                if (auto queued = funcHelperChunkMessingQueued(RIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(RIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(FIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(FIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(RFIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(RFIdx);
                }

            }
            // RB
            else if (bRePosX && bReNegZ)
            {
                uint64_t RIdx = encodeChunkCoord(cx + 1, cy, cz);
                uint64_t BIdx = encodeChunkCoord(cx, cy, cz - 1);
                uint64_t RBIdx = encodeChunkCoord(cx + 1, cy, cz - 1);
                if (auto queued = funcHelperChunkMessingQueued(RIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(RIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(BIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(BIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(RBIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(RBIdx);
                }
            }
            // LF
            else if (bReNegX && bRePosZ)
            {
                uint64_t LIdx = encodeChunkCoord(cx - 1, cy, cz);
                uint64_t FIdx = encodeChunkCoord(cx, cy, cz + 1);
                uint64_t LFIdx = encodeChunkCoord(cx - 1, cy, cz + 1);
                if (auto queued = funcHelperChunkMessingQueued(LIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(LIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(FIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(FIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(LFIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(LFIdx);
                }
            }
            // LB
            else if (bReNegX && bReNegZ)
            {
                uint64_t LIdx = encodeChunkCoord(cx - 1, cy, cz);
                uint64_t BIdx = encodeChunkCoord(cx, cy, cz - 1);
                uint64_t LBIdx = encodeChunkCoord(cx - 1, cy, cz - 1);
                if (auto queued = funcHelperChunkMessingQueued(LIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(LIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(BIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(BIdx);
                }
                if (auto queued = funcHelperChunkMessingQueued(LBIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(LBIdx);
                }
            }
            else if (bRePosX)
            {
                uint64_t RIdx = encodeChunkCoord(cx + 1, cy, cz);
                if (auto queued = funcHelperChunkMessingQueued(RIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(RIdx);
                }
            }
            else if (bReNegX)
            {
                uint64_t LIdx = encodeChunkCoord(cx - 1, cy, cz);;
                if (auto queued = funcHelperChunkMessingQueued(LIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(LIdx);
                }
            }
            else if (bRePosZ)
            {
                uint64_t FIdx = encodeChunkCoord(cx, cy, cz + 1);
                if (auto queued = funcHelperChunkMessingQueued(FIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(FIdx);
                }
            }
            else if (bReNegZ)
            {
                uint64_t BIdx = encodeChunkCoord(cx, cy, cz - 1);
                if (auto queued = funcHelperChunkMessingQueued(BIdx))
                {
                    if (queued.value())
                    {
                        ++iter;
                        continue;
                    }
                    else
                        messingReqVec.push_back(BIdx);
                }
            }
            messingReqVec.push_back(chunkIdx);


            std::vector<uint64_t> vecTmp{};
            for (const auto& changeChunkIdx : messingReqVec)
            {
                auto alreadyIter = setReBuildChunks.find(changeChunkIdx);
                if (alreadyIter == setReBuildChunks.end())
                {
                    setReBuildChunks.insert(changeChunkIdx);
                    vecTmp.push_back(changeChunkIdx);
                }
            }

            if (!vecTmp.empty())
            {
                vecReqMessingChunks.push_back(vecTmp);
            }
            


            //setReBuildChunks

            //for (const auto [blockIdx, block] : BlockChanges)
            //{
            //    auto tmp = chunkFindIter->second->GetBlock(blockIdx);
            //    //chunkFindIter->second->SetBlock(blockIdx, block.GetType());
            //    chunkFindIter->second->SetBlock(blockIdx, block);
            //}

            //for (const auto& changeChunkIdx : messingReqVec)
            //{
            //    auto blockChangeTargetIter = m_EditShadow.find(changeChunkIdx);
            //    if (blockChangeTargetIter == m_EditShadow.end())
            //    {

            //    }
            //}
            

            //QueueingQuadMessing(messingReqVec, true);
            //iter = m_EditShadow.erase(iter);

            ++iter;
        }
        else
        {
            iter = m_EditShadow.erase(iter);
        }
    }


    for (const auto& vecChunkIdxs : vecReqMessingChunks)
    {
        for (const auto& chunkIdx : vecChunkIdxs)
        {
            auto iter = m_EditShadow.find(chunkIdx);
            if (iter != m_EditShadow.end())
            {
                for (const auto [blockIdx, block] : iter->second)
                {
                    auto chunkFindIter = m_mapChunks.find(chunkIdx);
                    if (chunkFindIter != m_mapChunks.end())
                    {
                        //auto tmp = chunkFindIter->second->GetBlock(blockIdx);
                        //chunkFindIter->second->SetBlock(blockIdx, block.GetType());
                        chunkFindIter->second->SetBlock(blockIdx, block);
                    }
                }

                m_EditShadow.erase(iter);
            }
        }

        QueueingQuadMessing(vecChunkIdxs, true);
    }
    
    return S_OK;
}

HRESULT CVoxelManager3::UpdateCheckLightingFutures()
{
    if (m_bLighing)
    {
        if (m_futLighting.valid())
        {
            if (m_futLighting.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                m_bLighing = false;
                std::unordered_set<uint64_t> lightedChunkIds = m_futLighting.get();

                for (const auto& targetCoord : lightedChunkIds)
                {
                    auto iter = m_mapChunks.find(targetCoord);
                    if (iter != m_mapChunks.end())
                    {
                        iter->second->SetLightingState(CChunk3::LIGHTING_STATE::DONE);
                        std::vector<uint64_t> toMessingVec{};
                        toMessingVec.push_back(targetCoord);
                        QueueingQuadMessing(toMessingVec);
                    }
                }
            }
        }
        else
        {
            MSG_BOX("LIGHT FUT NOT VALID");
        }
    }

    return S_OK;
}

HRESULT CVoxelManager3::QueueingQuadMessing(std::vector<uint64_t> targetCoords, _bool bPushFront)
{
    for (const auto& chunkIdx : targetCoords)
    {
        auto iter = m_mapChunks.find(chunkIdx);
        if (iter != m_mapChunks.end())
        {
            iter->second->SetMessingQueued(true);
        }
    }




    if (bPushFront)
    {
        m_queuedQuadMessingChunks.push_front(targetCoords);
    }
    else
    {
        m_queuedQuadMessingChunks.push_back(targetCoords);
    }
    
    return S_OK;
}

HRESULT CVoxelManager3::Initialize()
{
    // =================================================================
    // 1. 기본 지형 및 베드락 세팅
    // =================================================================
    m_Noises[ETOUI(NOISE_TYPE::HEIGHT)].SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noises[ETOUI(NOISE_TYPE::HEIGHT)].SetSeed(m_iNoiseSeed);

    //  [핵심 수정 1]: 주파수를 0.03f -> 0.005f 정도로 대폭 낮춥니다.
    // 값이 작아질수록 "어어어엄청 넓고 완만한 대륙"이 형성되어 뾰족함이 사라집니다.
    m_Noises[ETOUI(NOISE_TYPE::HEIGHT)].SetFrequency(0.005f);

    m_Noises[ETOUI(NOISE_TYPE::HEIGHT)].SetFractalType(FastNoiseLite::FractalType_FBm);
    m_Noises[ETOUI(NOISE_TYPE::HEIGHT)].SetFractalOctaves(4); // 5에서 4로 낮추면 지형이 더 매끄러워집니다.
    m_Noises[ETOUI(NOISE_TYPE::HEIGHT)].SetFractalLacunarity(2.0f);

    //  [핵심 수정 2]: Gain(영향력 배율)을 0.3f -> 0.2f 정도로 낮춥니다.
    // 옥타브를 쌓을 때 생기는 자잘한 굴곡(뾰족한 노이즈)의 강도를 줄여서 표면을 부드럽게 만듭니다.
    m_Noises[ETOUI(NOISE_TYPE::HEIGHT)].SetFractalGain(0.2f);


    m_Noises[ETOUI(NOISE_TYPE::BEDROCK)].SetSeed(m_iNoiseSeed + 1);
    m_Noises[ETOUI(NOISE_TYPE::BEDROCK)].SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_Noises[ETOUI(NOISE_TYPE::BEDROCK)].SetFrequency(0.8f);

    // =================================================================
    // 2. 동굴 세팅
    // =================================================================
    m_Noises[ETOUI(NOISE_TYPE::CAVE)].SetSeed(m_iNoiseSeed + 2);
    m_Noises[ETOUI(NOISE_TYPE::CAVE)].SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_Noises[ETOUI(NOISE_TYPE::CAVE)].SetFrequency(0.04f);

    // =================================================================
    // 3.  광물별 개별 3D 노이즈 세팅 (시드 분리 필수!)
    // =================================================================

    //  철광석 (기존 유지)
    m_Noises[ETOUI(NOISE_TYPE::ORE_IRON)].SetSeed(m_iNoiseSeed + 3);
    m_Noises[ETOUI(NOISE_TYPE::ORE_IRON)].SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_Noises[ETOUI(NOISE_TYPE::ORE_IRON)].SetFrequency(0.15f); // 적당한 크기의 덩어리

    //  석탄 (추가)
    m_Noises[ETOUI(NOISE_TYPE::ORE_COAL)].SetSeed(m_iNoiseSeed + 4); // 시드 +4
    m_Noises[ETOUI(NOISE_TYPE::ORE_COAL)].SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    // 석탄은 마인크래프트에서도 덩어리가 매우 크게 나오므로, 주파수를 약간 낮춰서 넓은 구역에 분포시킵니다.
    m_Noises[ETOUI(NOISE_TYPE::ORE_COAL)].SetFrequency(0.12f);

    //  다이아몬드 (추가)
    m_Noises[ETOUI(NOISE_TYPE::ORE_DIAMOND)].SetSeed(m_iNoiseSeed + 5); // 시드 +5
    m_Noises[ETOUI(NOISE_TYPE::ORE_DIAMOND)].SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    // 다이아몬드는 아주 좁은 영역에 1~4개 수준으로 뭉쳐나와야 하므로 주파수를 높여 촘촘하게 만듭니다.
    m_Noises[ETOUI(NOISE_TYPE::ORE_DIAMOND)].SetFrequency(0.22f);




    //  [온도 노이즈 세팅]
    m_Noises[ETOUI(NOISE_TYPE::TEMPERATURE)].SetSeed(m_iNoiseSeed + 10);
    m_Noises[ETOUI(NOISE_TYPE::TEMPERATURE)].SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noises[ETOUI(NOISE_TYPE::TEMPERATURE)].SetFrequency(0.002f); // 아주 넓게 분포하도록 낮게 세팅

    // [습도 노이즈 세팅]
    m_Noises[ETOUI(NOISE_TYPE::HUMIDITY)].SetSeed(m_iNoiseSeed + 21);
    m_Noises[ETOUI(NOISE_TYPE::HUMIDITY)].SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noises[ETOUI(NOISE_TYPE::HUMIDITY)].SetFrequency(0.002f);

    //m_NoiseHeight.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //m_NoiseHeight.SetFrequency(0.03f); // 전체적인 지형의 크기 (낮을수록 거대함)

    //// Fractal 설정 (핵심!)
    //m_NoiseHeight.SetFractalType(FastNoiseLite::FractalType_FBm);
    //m_NoiseHeight.SetFractalOctaves(5);     // 층을 얼마나 쌓을지 (4~6 추천)
    //m_NoiseHeight.SetFractalLacunarity(2.0f); // 층 사이의 주파수 배율
    //m_NoiseHeight.SetFractalGain(0.3f);       // 층 사이의 영향력 배율


    //
    {
        m_pResSolidBlockVertexShader = CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Block");
        m_pResSolidBlockPixelShader = CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Block");

        m_pResWaterBlockVertexShader = CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Water");
        m_pResWaterBlockPixelShader = CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Water");
    }
    {
        m_pResBlocksTexutreArray = CGameInstance::Get().GetResourceFirst<E::CResTexture2DArray>("VOXEL_MANAGER_TEX", "TEXTURE_ARRAY");;
    }
    {
        m_pResSamplerPointWrap = CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
    }



    // test light
    DIRECTIONAL_LIGHT light{};

    light.direction = _float3(0.3f, -1.0f, 0.2f); // 아래로 비추는 방향
    // 필요하면 normalize

    light.ambient = _float4(0.2f, 0.2f, 0.25f, 1.0f); // 약간 푸른 톤
    light.diffuse = _float4(1.f, 1.f, 1.f, 1.0f); // 메인 밝기
    light.specular = _float4(0.5f, 0.5f, 0.5f, 1.0f);  // 적당한 하이라이트
    CGameInstance::Get().SetDirectionalLight("0_Test", light);
    return S_OK;
}

UPtr<CVoxelManager3> CVoxelManager3::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = ToUPtr(new CVoxelManager3{ pDevice , pContext });
    if (FAILED(pInstance->Initialize()))
    {
        return nullptr;
    }
    return pInstance;
}
