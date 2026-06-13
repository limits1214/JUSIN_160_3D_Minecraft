#include "Cloud.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CameraObject.h"

NS_USING(Engine)

CCloud::CCloud()
{
}
CCloud::CCloud(const CCloud& rhs)
{

}
CCloud::~CCloud()
{
}

HRESULT CCloud::Initialize(void* pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    m_iNumElements = 20000;

    if (auto res = CResDynamicBuffer::Create())
    {
        CResDynamicBuffer::DESC Desc{};
        Desc.desc = {
            .ByteWidth = ((uint32_t)sizeof(InstanceData) * m_iNumElements),
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

    m_pResVIBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResCloudVIBuffer>("MC_VIBuffer", "Cloud");

    m_TimerForCloudCalc.Set_GoalTime(0.2f);
    return S_OK;
}

void CCloud::PriorityUpdate(E::_float fTimeDelta)
{
}

void CCloud::Update(E::_float fTimeDelta)
{
    // 바람에 의해 흐르는 오프셋 속도 조절
    m_fCloudOffset += m_fCloudSpeed * fTimeDelta;
}

void CCloud::LateUpdate(E::_float fTimeDelta)
{
   
    m_TimerForCloudCalc.AppendCurrTime(fTimeDelta);
    if (m_TimerForCloudCalc.Get_Finished())
    {
        m_TimerForCloudCalc.Reset();
        if (m_futCloudCalc.valid())
        {
            return;
        }
       
        auto vCamPos = CGameInstance::Get().GetActiveGameCamera()->GetTransform().GetPosition();
        m_futCloudCalc = CGameInstance::Get().WorkerEnqueueWithFuture("CLOUD", [=]()->std::vector<CCloud::InstanceData>
            {
                std::vector<CCloud::InstanceData> instancedData;
                instancedData.reserve(m_InstanceData.size());

                instancedData.clear();
                //  구름의 두께(Y스케일) 및 가로세로 크기를 4.0f 정방형으로 잡습니다.
                XMMATRIX matScale = XMMatrixScaling(4.0f, 1.5f, 4.0f);

                _float fTileSize = 4.0f;
                _float fCloudHeight = 120.0f;
                int iHalfGrid = 64; // 카메라 중심으로 앞뒤좌우 64칸 스캔 (총 128x128 영역)

                // 1. 카메라의 실시간 월드 좌표 획득 및 그리드 인덱스 스냅
                //XMVECTOR vCamPos = CGameInstance::Get().GetActiveGameCamera()->GetTransform().GetLoadedPostion();
                int iCamGridX = (int)floorf(vCamPos.x / fTileSize);
                int iCamGridZ = (int)floorf(vCamPos.z / fTileSize);

                auto& noise = CGameInstance::Get().GetVoxelNoiseByType(NOISE_TYPE::CLOUD);

                // 2. 카메라를 중심축으로 하는 가상의 128x128 바둑판 실시간 검색
                for (int z = iCamGridZ - iHalfGrid; z < iCamGridZ + iHalfGrid; ++z)
                {
                    for (int x = iCamGridX - iHalfGrid; x < iCamGridX + iHalfGrid; ++x)
                    {
                        //  메모리 오버플로우 원천 차단 안전장치 (외부 루프까지 완전히 탈출하도록 수정)
                        if (instancedData.size() >= m_iNumElements)
                            goto EXIT_LOOP;

                        _float fNoiseX = (_float)x * 0.35f + (m_fCloudOffset * 0.005f);
                        _float fNoiseZ = (_float)z * 0.35f;

                        _float fNoiseVal = noise.GetNoise(fNoiseX, 0.0f, fNoiseZ);

                        //  [밀도 조건 수식] 
                        // 0.0f ~ 0.1f 사이로 잡으면 구름들이 듬성듬성 끊기지 않고 이쁘게 군집을 이룹니다.
                        if (fNoiseVal > 0.1f)
                        {
                            XMFLOAT3 worldPos;
                            worldPos.x = (_float)x * fTileSize;
                            worldPos.y = fCloudHeight;
                            worldPos.z = (_float)z * fTileSize;

                            XMMATRIX matTranslation = XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
                            XMMATRIX matWorld = matScale * matTranslation;

                            InstanceData data;
                            XMStoreFloat4x4(&data.matWorld, matWorld);
                            instancedData.push_back(data);
                        }
                    }
                }

            EXIT_LOOP:
                
                return instancedData;
            });
    }

    if (m_futCloudCalc.valid())
    {
        if (m_futCloudCalc.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            m_InstanceData = m_futCloudCalc.get();
            m_futCloudCalc = {};
        }
    }


    
    
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CCloud::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    if (m_InstanceData.empty())
        return S_OK;

    const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Cloud");
    const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Cloud");
    const auto& viBuffer = m_pResVIBuffer;

    // 데이터 갱신
    {
        auto pCbPerObject = m_pResInstancedBuffer;
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;

        if (SUCCEEDED(pContext->Map(pCbPerObject->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        {
            std::memcpy(mappedSubResource.pData, m_InstanceData.data(), sizeof(InstanceData) * m_InstanceData.size());
            pContext->Unmap(pCbPerObject->GetBuffer().Get(), 0);
        }
    }

    // 파이프라인 바인딩 및 렌더링
    pContext->IASetInputLayout(vs->GetInputLayout().Get());
    pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

    ID3D11Buffer* vertexBuffers[] = {
        viBuffer->GetVertexBuffer().Get(),
        m_pResInstancedBuffer->GetBuffer().Get()
    };
    uint32_t strides[] = {
        viBuffer->GetVertexStride(),
        (uint32_t)sizeof(InstanceData),
    };
    uint32_t offsets[] = { 0, 0 };

    pContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
    pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
    pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

    {
        const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
        pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
    }
    {
        const auto& rasterizer = E::CGameInstance::GetConst().GetResourceFirst<E::CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL);
        pContext->RSSetState(rasterizer->GetRasterizerState().Get());
    }

    pContext->DrawIndexedInstanced((UINT)viBuffer->GetNumIndices(), (UINT)m_InstanceData.size(), 0, 0, 0);
    return S_OK;
}

UPtr<CCloud> CCloud::Create()
{
    auto pInstance = ToUPtr(new CCloud{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CCloud");
        return nullptr;
    }
    return pInstance;
}

UPtr<CPrototype> CCloud::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CCloud{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CCloud");
        return nullptr;
    }
    return pInstance;
}