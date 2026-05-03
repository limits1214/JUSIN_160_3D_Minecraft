#include "VoxelManager.h"
#include "Resources.h"
#include "GameInstance.h"

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

    volatile int x = 0;
}

void CVoxelManager::Update(_float fTimeDelta)
{
}

void CVoxelManager::StateUpdate(const VOXEL_MANAGER_STATE_UPDATE_DESC& desc)
{
}

void CVoxelManager::SetChunkLoadCenter(int32_t x, int32_t z)
{
    auto chunkCoord = encodeChunkCoord(x, z);
    auto iter = m_mapChucnks.find(chunkCoord);
    if (iter != m_mapChucnks.end())
    {
        // TODO: 청크 존재한다면  청크 렌더 디스턴스 확인
    }
    else
    {
        // TODO: 청크 없다면 청크 렌더 디스턴스만큼 넣기
        
        if (m_iRenderDistance == 0)
        {
            
        }
        else
        {
            for (int32_t i = -m_iRenderDistance; i <= m_iRenderDistance; ++i)
            {
                for (int32_t j = -m_iRenderDistance; j <= m_iRenderDistance; ++j)
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
    

    m_mapChucnks.emplace(chunkCoord, std::move(chunk));
	return S_OK;
}

UPtr<CVoxelManager> CVoxelManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return ToUPtr(new CVoxelManager{ pDevice , pContext });
}
