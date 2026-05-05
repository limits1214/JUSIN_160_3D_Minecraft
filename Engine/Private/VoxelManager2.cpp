#include "pch.h"

#include "VoxelManager2.h"

#include "Resources.h"
NS_USING(Engine)

// only main thread
HRESULT CVoxelManager2::QueuingChunkInRangeCreate(const CHUNK_IN_RANGE_CREATE_DESC& desc)
{
    m_ChunkInRangeCreateQueue.push(desc);
    return S_OK;
}

// only main thread
HRESULT CVoxelManager2::QueuingChunkOutRangeRelease(const CHUNK_OUT_RANGE_RELEASE_DESC& desc)
{
    m_ChunkOutRangeReleaseQueue.push(desc);
    return S_OK;
}

// only main thread
HRESULT CVoxelManager2::QueuingChunkRebuild(const CHUNK_REBUILD_DESC& desc)
{
    m_ChunkRebuildQueue.push(desc);
    return S_OK;
}

CVoxelManager2::CVoxelManager2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{pDevice}
	, m_pContext{pContext}
{
}

CVoxelManager2::~CVoxelManager2()
{
}

HRESULT CVoxelManager2::Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
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

    pContext->PSSetShaderResources(9, 1, m_pResBlocksTexutreArray->GetSRV().GetAddressOf());
    pContext->PSSetSamplers(9, 1, m_pResSamplerPointWrap->GetSamplerState().GetAddressOf());

    for (const auto& [key, val] : m_mapChucnks)
    {
        val->Draw(pContext, ctx);
    }

    return S_OK;
}

void CVoxelManager2::UpdateGUI()
{
    if (ImGui::Button("chunk load 000"))
    {
        CHUNK_IN_RANGE_CREATE_DESC desc{};
        desc.iCenterX = 0;
        desc.iCenterY = 0;
        desc.iCenterZ = 0;
        QueuingChunkInRangeCreate(desc);
    }
}

void CVoxelManager2::Update(_float fTimeDelta)
{
    _bool bRebuildProcessing = m_setCurrentProcess.find(PROCESS::CHUNK_REBUILD) != m_setCurrentProcess.end();
    _bool bInRangeCreateProcessing = m_setCurrentProcess.find(PROCESS::CHUNK_IN_RANGE_CREATE) != m_setCurrentProcess.end();
    _bool bOutRangeReleaseProcessing = m_setCurrentProcess.find(PROCESS::CHUNK_OUT_RANGE_RELEASE) != m_setCurrentProcess.end();
    _bool bHasRebuildQueue = !m_ChunkRebuildQueue.empty();
    _bool bHasInRangeCreateQueue = !m_ChunkInRangeCreateQueue.empty();
    _bool bHasOutRangeReleasedQueue = !m_ChunkOutRangeReleaseQueue.empty();

    // inrangeCreate, outrangeRelease 는 동시에 겹처서 실행하지 않는다.
    
    // 리빌드 큐가 존재하면
    if (bHasRebuildQueue)
    {
        
        // if rebuild target is maked create or release, do not processing
    }

    // create큐가 존재하는데 현제 릴리즈 프로세싱이 아니여야함
    if (bHasInRangeCreateQueue && !bOutRangeReleaseProcessing && !bInRangeCreateProcessing)
    {
        CHUNK_IN_RANGE_CREATE_DESC createDesc = m_ChunkInRangeCreateQueue.front();
        m_ChunkInRangeCreateQueue.pop();
        m_setCurrentProcess.insert(PROCESS::CHUNK_IN_RANGE_CREATE);
        m_eProcessChunkInRangeCreateState = PROCESS_CHUNK_IN_RANGE_CREATE_STATE::COLLECT_CANDIDATE;
        m_futInRangeChunkCreateCollectCandidate = CGameInstance::Get().WorkerEnqueueWithFuture("FUT_PROCESS_CHUNK_IN_RANGE_CREATE_STATE_COLLECT_CANDIDATE", [this, createDesc ]()->std::vector<std::pair<uint64_t, UPtr<CChunk2>>> {
            
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
                        if (m_mapChucnks.find(chunkCoord) != m_mapChucnks.end())
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

            std::vector<std::pair<uint64_t, UPtr<CChunk2>>>  retVec{};
            for (auto& p : list)
            {
                uint64_t key = encodeChunkCoord(p.x, p.y, p.z);
                CChunk2::DESC chunkDesc{};
                chunkDesc.iX = p.x;
                chunkDesc.iY = p.y;
                chunkDesc.iZ = p.z;
                chunkDesc.iChunkCoord = key;
                retVec.push_back({ key, std::move(CChunk2::Create(chunkDesc)) });
            }
            
            return retVec;
            });
    }
    // release큐가 존재하는데 현재 크리에이스 프로세싱이 아니여야함
    else if (bHasOutRangeReleasedQueue && !bOutRangeReleaseProcessing && !bInRangeCreateProcessing)
    {

    }

    if (bRebuildProcessing)
    {

    }

    if (bInRangeCreateProcessing)
    {
        if (m_eProcessChunkInRangeCreateState == PROCESS_CHUNK_IN_RANGE_CREATE_STATE::COLLECT_CANDIDATE)
        {
            if (m_futInRangeChunkCreateCollectCandidate.valid())
            {
                if (m_futInRangeChunkCreateCollectCandidate.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    std::vector<std::pair<uint64_t, UPtr<CChunk2>>> movedChunks = m_futInRangeChunkCreateCollectCandidate.get();
                    if (movedChunks.empty())
                    {
                        // 프로세스 종료
                        m_eProcessChunkInRangeCreateState = PROCESS_CHUNK_IN_RANGE_CREATE_STATE::NON;
                        m_setCurrentProcess.erase(PROCESS::CHUNK_IN_RANGE_CREATE);
                    }
                    else
                    {
                        std::vector<CChunk2*> chunkCaching{};
                        for (auto& [coord, pChunk] : movedChunks)
                        {
                            CChunk2* pCache = pChunk.get();
                            const auto& [_, bInserted] = m_mapChucnks.emplace(coord, std::move(pChunk));
                            if (!bInserted)
                            {
                                // TODO: MSGBOX
                            }
                            else
                            {
                                chunkCaching.push_back(pCache);
                            }
                        }

                        m_eProcessChunkInRangeCreateState = PROCESS_CHUNK_IN_RANGE_CREATE_STATE::BLOCK_FILLING;
                        for (auto* pChunk : chunkCaching)
                        {
                            std::future<CChunk2*> fut = CGameInstance::Get().WorkerEnqueueWithFuture("FUT_PROCESS_CHUNK_IN_RANGE_CREATE_STATE_BLOCK_FILLING", [pChunk]()->CChunk2* {
                                if (FAILED(pChunk->BlockFilling()))
                                {
                                    // TODO: MSGBOX
                                }
                                return pChunk;
                                });
                            m_futInRangeChunkCreateBlockFillings.push_back(std::move(fut));
                        }
                    }
                }
            }
            else
            {
                // TODO: MSGBOX
            }
        }
        else if (m_eProcessChunkInRangeCreateState == PROCESS_CHUNK_IN_RANGE_CREATE_STATE::BLOCK_FILLING)
        {
            size_t validCnt = m_futInRangeChunkCreateBlockFillings.size();
            size_t cnt{};
            for (auto& fut : m_futInRangeChunkCreateBlockFillings)
            {
                if (fut.valid())
                {
                    if (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        ++cnt;
                    }
                }
                else
                {
                    // TODO: MSGBOX
                }
            }

            if (cnt == validCnt)
            {
                m_eProcessChunkInRangeCreateState = PROCESS_CHUNK_IN_RANGE_CREATE_STATE::REGIST_NEIGHBOR;
                std::vector<CChunk2*> chunkCaching{};
                // set neighbor pointer in main thread
                for (auto& fut : m_futInRangeChunkCreateBlockFillings)
                {
                    CChunk2* pChunk = fut.get();
                    chunkCaching.push_back(pChunk);
                }
                m_futInRangeChunkCreateBlockFillings.clear();


                m_eProcessChunkInRangeCreateState = PROCESS_CHUNK_IN_RANGE_CREATE_STATE::MESSING;
                for (auto* pChunk : chunkCaching)
                {
                    std::future<CChunk2*> fut = CGameInstance::Get().WorkerEnqueueWithFuture("FUT_PROCESS_CHUNK_IN_RANGE_CREATE_STATE_MESSING", [pChunk]()->CChunk2* {
                        if (FAILED(pChunk->Messing()))
                        {
                            // TODO: MSGBOX
                        }
                        return pChunk;
                        });
                    m_futInRangeChunkCreateMessing.push_back(std::move(fut));
                }
            }
            else
            {
                // TODO: MSGBOX
            }
        }
        else if (m_eProcessChunkInRangeCreateState == PROCESS_CHUNK_IN_RANGE_CREATE_STATE::MESSING)
        {
            for (auto iter = m_futInRangeChunkCreateMessing.begin(); iter != m_futInRangeChunkCreateMessing.end();)
            {
                if (iter->valid())
                {
                    if (iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        if (FAILED(iter->get()->GenBuffer()))
                        {
                            // TODO: MSGBOX
                        }
                        iter = m_futInRangeChunkCreateMessing.erase(iter);
                    }
                    else
                    {
                        ++iter;
                    }
                }
                else
                {
                    // TODO: MSGBOX
                    ++iter;
                }
            }

            if (m_futInRangeChunkCreateMessing.empty())
            {
                //프로세스 종료
                m_eProcessChunkInRangeCreateState = PROCESS_CHUNK_IN_RANGE_CREATE_STATE::NON;
                m_setCurrentProcess.erase(PROCESS::CHUNK_IN_RANGE_CREATE);
            }
        }
    }
    else if (bOutRangeReleaseProcessing)
    {

    }

}


HRESULT CVoxelManager2::Initialize()
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
        if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Block", "./Resources/Shader/Block/Block.hlsl"))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
            m_pResVertexShader = res;
        }
        if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Block", "./Resources/Shader/Block/Block.hlsl"))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
            m_pResPixelShader = res;
        }
    }
    {
        {
            //0
            auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/dirt.png");
            if (FAILED(pTexture->Load()))
            {
                return E_FAIL;
            }
            CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
        }

        {
            //1
            auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/stone.png");
            if (FAILED(pTexture->Load()))
            {
                return E_FAIL;
            }
            CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
        }

        {
            //2
            auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/sand.png");
            if (FAILED(pTexture->Load()))
            {
                return E_FAIL;
            }
            CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
        }
    }

    {
        CResTexture2DArray::DESC desc{};
        desc.textureId = { "VOXEL_MANAGER_TEX", "TEXTURES" };
        auto pTextureArray = CResTexture2DArray::Create();
        if (FAILED(pTextureArray->Load(desc)))
        {
            return E_FAIL;
        }
        m_pResBlocksTexutreArray = pTextureArray;
        CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURE_ARRAY", pTextureArray);
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

UPtr<CVoxelManager2> CVoxelManager2::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = ToUPtr(new CVoxelManager2{ pDevice , pContext });
    if (FAILED(pInstance->Initialize()))
    {
        return nullptr;
    }
    return pInstance;
}
