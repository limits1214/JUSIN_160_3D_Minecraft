#include "VoxelManager3.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"

#include "SkeletonEntity.h"


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
            if (block.GetType() != CBlock3::TYPE::AIR)
            {
                outResult.pChunk = pChunk;
                outResult.iX = lx;
                outResult.iY = ly;
                outResult.iZ = lz;
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
    const auto& vs = m_pResVertexShader;
    const auto& ps = m_pResPixelShader;

    pContext->IASetInputLayout(vs->GetInputLayout().Get());
    pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);
    if (0)
    {
        const auto& rasterizer = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_WIREFRAME_NOCULL);
        pContext->RSSetState(rasterizer->GetRasterizerState().Get());
    }

    pContext->PSSetSamplers(9, 1, m_pResSamplerPointWrap->GetSamplerState().GetAddressOf());

    for (const auto& [key, val] : m_mapChunks)
    {
        val->Draw(pContext, ctx);
    }

    return S_OK;
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
                    const auto& [cx, cy, cz] = res.pChunk->GetCoord();

                    int32_t worldBX = VOXEL_CHUNK_X_SIZE3 * cx + res.iX;
                    int32_t worldBY = VOXEL_CHUNK_Y_SIZE3 * cy + res.iY;
                    int32_t worldBZ = VOXEL_CHUNK_Z_SIZE3 * cz + res.iZ;

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
                                E::ETOUI(0), &Desc))
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
                RECT rect;
                GetClientRect(CGameInstance::Get().GetHwnd(), &rect);

                E::_float4x4 P;
                XMStoreFloat4x4(&P, cam->GetProj());

                E::_vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                E::_vector rayDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

                E::_matrix V = cam->GetView();
                auto detV = XMMatrixDeterminant(V);
                E::_matrix invView = XMMatrixInverse(&detV, V);

                rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
                rayDir = XMVector3TransformNormal(rayDir, invView);
                _float3 vecrayOrigin;
                _float3 vecrayDir;

                XMStoreFloat3(&vecrayOrigin, rayOrigin);
                XMStoreFloat3(&vecrayDir, XMVector3Normalize(rayDir));


                BLOCK_RAY_RESULT res;
                if (BlockRaycast(vecrayOrigin, vecrayDir, 5.f, res))
                {
                    const auto& [cx, cy, cz] = res.pChunk->GetCoord();

                    auto worldBX = VOXEL_CHUNK_X_SIZE3 * cx + res.iX;
                    auto worldBY = VOXEL_CHUNK_Y_SIZE3 * cy + res.iY;
                    auto worldBZ = VOXEL_CHUNK_Z_SIZE3 * cz + res.iZ;

                    CBlock3 block{};
                    block.SetType(CBlock3::TYPE::AIR);

                    SetBlock(worldBX, worldBY, worldBZ, block);
                }
            }
        }

        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            if (auto cam = E::CGameInstance::Get().GetActiveGameCamera())
            {
                RECT rect;
                GetClientRect(CGameInstance::Get().GetHwnd(), &rect);

                E::_float4x4 P;
                XMStoreFloat4x4(&P, cam->GetProj());

                E::_vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                E::_vector rayDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

                E::_matrix V = cam->GetView();
                auto detV = XMMatrixDeterminant(V);
                E::_matrix invView = XMMatrixInverse(&detV, V);

                rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
                rayDir = XMVector3TransformNormal(rayDir, invView);
                _float3 vecrayOrigin;
                _float3 vecrayDir;

                XMStoreFloat3(&vecrayOrigin, rayOrigin);
                XMStoreFloat3(&vecrayDir, XMVector3Normalize(rayDir));


                BLOCK_RAY_RESULT res;
                if (BlockRaycast(vecrayOrigin, vecrayDir, 5.f, res))
                {
                    const auto& [cx, cy, cz] = res.pChunk->GetCoord();

                    auto worldBX = VOXEL_CHUNK_X_SIZE3 * cx + res.iX;
                    auto worldBY = VOXEL_CHUNK_Y_SIZE3 * cy + res.iY;
                    auto worldBZ = VOXEL_CHUNK_Z_SIZE3 * cz + res.iZ;
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



                    CBlock3 block{};
                    block.SetType(CBlock3::TYPE::GRASS);

                    SetBlock(worldBX, worldBY, worldBZ, block);
                }
            }
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
    UpdateCheckBlockEdit();
    UpdateCheckQuduedQuadMessingChunk();
    UpdateCheckQuadMessingEndFutures();

    if (m_queueFutBlockFilling.empty()
        && m_queuedQuadMessingChunks.empty()
        && m_queueFutQuadMessing.empty()
        && m_queueFutEdit.empty())
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
    CGameInstance::Get().WorkerEnqueue("TMP", [=]() {
        
        
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


            std::future<uint64_t> fut = CGameInstance::Get().WorkerEnqueueWithFuture("FUT_BLOCK_FILLING", [this, chunkCoord]()->uint64_t {
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
                if (chunkFindIter->second->GetMessingState() == CChunk3::MESSING_STATE::ING)
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
            std::future<std::vector<uint64_t>> fut = CGameInstance::Get().WorkerEnqueueWithFuture("FUT_QUAD_MESSING", [this, targetCoords]()->std::vector<uint64_t> {
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
    for (auto iter = m_EditShadow.begin(); iter != m_EditShadow.end(); )
    {
        uint64_t chunkIdx = iter->first;
        auto chunkFindIter = m_mapChunks.find(chunkIdx);
        if (chunkFindIter != m_mapChunks.end())
        {
            if (!chunkFindIter->second->GetMessingQueued())
            {
                const auto& [cx, cy, cz] = decodeChunkCoord(chunkIdx);
                std::vector<uint64_t> messingReqVec{};

                //CChunk3::BlockIndexing(0, 0, 0);
                //CChunk3::BlockIndexing(VOXEL_CHUNK_X_SIZE3-1, 0, 0);

                //CChunk3::BlockIndexing(0, 0, VOXEL_CHUNK_Z_SIZE3 - 1);
                //CChunk3::BlockIndexing(VOXEL_CHUNK_X_SIZE3 - 1, 0, VOXEL_CHUNK_Z_SIZE3 - 1);

                // pos_x
                {
                    
                    uint64_t targetIdx = encodeChunkCoord(cx + 1, cy, cz);
                    auto targetIter = m_mapChunks.find(targetIdx);
                    if (targetIter != m_mapChunks.end())
                    {
                        if (targetIter->second->GetMessingQueued())
                        {
                            ++iter;
                            continue;
                        }
                        else
                        {
                            messingReqVec.push_back(targetIdx);
                        }
                    }
                }

                // neg_x
                {
                    uint64_t targetIdx = encodeChunkCoord(cx - 1, cy, cz);
                    auto targetIter = m_mapChunks.find(targetIdx);
                    if (targetIter != m_mapChunks.end())
                    {
                        if (targetIter->second->GetMessingQueued())
                        {
                            ++iter;
                            continue;
                        }
                        else
                        {
                            messingReqVec.push_back(targetIdx);
                        }
                    }
                }

                // pos_z
                {
                    uint64_t targetIdx = encodeChunkCoord(cx, cy, cz + 1);
                    auto targetIter = m_mapChunks.find(targetIdx);
                    if (targetIter != m_mapChunks.end())
                    {
                        if (targetIter->second->GetMessingQueued())
                        {
                            ++iter;
                            continue;
                        }
                        else
                        {
                            messingReqVec.push_back(targetIdx);
                        }
                    }
                }

                // neg_z
                {
                    uint64_t targetIdx = encodeChunkCoord(cx, cy, cz - 1);
                    auto targetIter = m_mapChunks.find(targetIdx);
                    if (targetIter != m_mapChunks.end())
                    {
                        if (targetIter->second->GetMessingQueued())
                        {
                            ++iter;
                            continue;
                        }
                        else
                        {
                            messingReqVec.push_back(targetIdx);
                        }
                    }
                }

                std::unordered_map<uint32_t, CBlock3>& blockChanges = iter->second;

                for (const auto [blockIdx, block] : blockChanges)
                {
                    auto tmp = chunkFindIter->second->GetBlock(blockIdx);
                    chunkFindIter->second->SetBlock(blockIdx, block.GetType());
                }

                messingReqVec.push_back(chunkIdx);

                QueueingQuadMessing(messingReqVec, true);
                iter = m_EditShadow.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        else
        {
            iter = m_EditShadow.erase(iter);
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
    m_NoiseHeight.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_NoiseHeight.SetFrequency(0.03f); // 전체적인 지형의 크기 (낮을수록 거대함)

    // Fractal 설정 (핵심!)
    m_NoiseHeight.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_NoiseHeight.SetFractalOctaves(5);     // 층을 얼마나 쌓을지 (4~6 추천)
    m_NoiseHeight.SetFractalLacunarity(2.0f); // 층 사이의 주파수 배율
    m_NoiseHeight.SetFractalGain(0.3f);       // 층 사이의 영향력 배율


    //
    {
        m_pResVertexShader = CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Block");
        m_pResPixelShader = CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Block");
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
