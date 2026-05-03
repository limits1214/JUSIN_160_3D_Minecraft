#include "VoxelManager.h"
#include "Resources.h"
#include "GameInstance.h"
#include "CameraObject.h"

NS_USING(Engine)

CVoxelManager::CVoxelManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{pDevice}
    , m_pContext{pContext}
{
}

CVoxelManager::~CVoxelManager()
{
}

HRESULT CVoxelManager::Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{
    const auto& vs = E::CGameInstance::GetConst().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_QuadCol");
    const auto& ps = E::CGameInstance::GetConst().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_QuadCol");
   
    pContext->IASetInputLayout(vs->GetInputLayout().Get());
    pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);
    if (1)
    {
        const auto& rasterizer = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_WIREFRAME_NOCULL);
        pContext->RSSetState(rasterizer->GetRasterizerState().Get());
    }

    for (const auto& [key, val] : m_mapChucnks)
    {
        val->BindBuffer(pContext, ctx);
    }
   
    return S_OK;
}

void CVoxelManager::UpdateGUI()
{
    auto test1 = encodeChunkCoord(0, 0);
    auto test2 = encodeChunkCoord(110, 120);
    auto test3 = encodeChunkCoord(-110, 120);
    auto test4 = encodeChunkCoord(-110, -120);
    auto test5 = encodeChunkCoord(110, -120);


    auto res1 = decodeChunkCoord(test1);
    auto res2 = decodeChunkCoord(test2);
    auto res3 = decodeChunkCoord(test3);
    auto res4 = decodeChunkCoord(test4);
    auto res5 = decodeChunkCoord(test5);

    if (ImGui::Button("SetChunkLoadCenter 0 0 "))
    {
        SetChunkLoadCenter(0, 0);
    }

    if (ImGui::Button("SetChunkLoadCenter Cam"))
    {
        if (auto cam = CGameInstance::Get().GetCameraObject("GAME"))
        {
            float fx = cam->GetTransform().GetPosition().x;
            float fz = cam->GetTransform().GetPosition().z;
            auto ix = (uint32_t)floor(fx/ VOXEL_CHUNK_X_SIZE);
            auto iz = (uint32_t)floor(fz / VOXEL_CHUNK_Z_SIZE);

            SetChunkLoadCenter(ix, iz);
        }
        
    }

    volatile int x = 0;
}

void CVoxelManager::Update(_float fTimeDelta)
{
    if (!m_ChunkLoadPending.empty())
    {
        //{
        //    std::lock_guard<std::mutex> lock(m_Mutex);
        //    m_ChunkLoadPending.push_back(chunkCoord);
        //}
        //auto coord = m_ChunkLoadPending.back();
        //m_ChunkLoadPending.pop_back();

        //const auto& [a,b] = decodeChunkCoord(coord);
        //ChunkLoad(a, b);
    }
}

void CVoxelManager::StateUpdate(const VOXEL_MANAGER_STATE_UPDATE_DESC& desc)
{
}

void CVoxelManager::SetChunkLoadCenter(int32_t x, int32_t z)
{
    int32_t minX = -m_iRenderDistance + x;
    int32_t maxX = m_iRenderDistance + x;
    int32_t minZ = -m_iRenderDistance + z;
    int32_t maxZ = m_iRenderDistance + z;

   
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        std::vector<uint64_t> delQ{};
        for (const auto& [key, val] : m_mapChucnks)
        {
            const auto& [x, z] = decodeChunkCoord(key);

            if (x < minX || x > maxX || z < minZ || z > maxZ)
            {
                delQ.push_back(key);
            }
        }

       
        std::for_each(delQ.begin(), delQ.end(), [&](const auto& key) {
            
            //auto pendingiter = std::find(m_ChunkLoadPending.begin(), m_ChunkLoadPending.end(), key);
            //if (pendingiter != m_ChunkLoadPending.end())
            //{
            //    m_ChunkLoadPending.erase(pendingiter);
            //}

            m_mapChucnks.erase(key);
            
            });
    }


    if (m_iRenderDistance == 0)
    {
        auto iter = m_mapChucnks.find(encodeChunkCoord(x, z));
        if (iter == m_mapChucnks.end())
        {
            ChunkLoad(x, z);
        }
    }
    else
    {
        // x
        for (int32_t i = -m_iRenderDistance + x; i <= m_iRenderDistance + x; ++i)
        {
            // z
            for (int32_t j = -m_iRenderDistance + z; j <= m_iRenderDistance + z; ++j)
            {
                auto chunkCoord = encodeChunkCoord(i, j);
                auto iter = m_mapChucnks.find(chunkCoord);
                if (iter == m_mapChucnks.end())
                {
                    ChunkLoad(i, j);
                }
            }
        }
    }

    
}

HRESULT CVoxelManager::ChunkLoad(int32_t x, int32_t z)
{
    auto chunkCoord = encodeChunkCoord(x, z);

    CChunk::DESC desc{};
    desc.iX = x;
    desc.iZ = z;
    desc.iChunkCoord = chunkCoord;
    auto chunk = CChunk::Create(desc);
    auto pCaching = chunk.get();

    m_mapChucnks.emplace(chunkCoord, std::move(chunk));

    CGameInstance::Get().WorkerEnqueue("CHUNK_LOADING", [=]() {
        //std::lock_guard<std::mutex> lock(m_Mutex);
        //++m_iEnqueuedCnt;

        pCaching->BufferLoad(m_Mutex);

        //--m_iEnqueuedCnt;
        });
	return S_OK;
}

HRESULT CVoxelManager::Initialize()
{
    m_NoiseHeight.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_NoiseHeight.SetFrequency(0.01f);
    return S_OK;
}

UPtr<CVoxelManager> CVoxelManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = ToUPtr(new CVoxelManager{ pDevice , pContext });
    if (FAILED(pInstance->Initialize()))
    {
        return nullptr;
    }
    return pInstance;
}
