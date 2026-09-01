#include "ResExperenceOrbVIBuffer.h"

#include "GameInstance.h"
#include "ResTexture2D.h"

NS_USING(Engine)

CResExperenceOrbVIBuffer::CResExperenceOrbVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CResVIBuffer{ sPath, pDevice, pContext }
{
}

CResExperenceOrbVIBuffer::~CResExperenceOrbVIBuffer()
{
}

HRESULT CResExperenceOrbVIBuffer::Load(const std::any& arg)
{
    //auto argDesc = std::any_cast<DESC>(&arg);
    //if (!argDesc)
    //{
    //    return E_FAIL;
    //}
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {
        constexpr uint32_t iNumVertices = 4;
        std::vector<VTX_EXP_ORB> vertices =
        {
            {_float3{-0.5f, 0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{0.f, 0.f}},
            {_float3{0.5f, 0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{1.f, 0.f}},
            {_float3{0.5f, -0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{1.f, 1.f}},
            {_float3{-0.5f, -0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{0.f, 1.f}}
        };

        std::vector<uint16_t> indices{ 0, 1, 2, 0, 2, 3 };


        m_iNumVertices = (uint32_t)vertices.size();
        m_iVertexStride = sizeof(VTX_EXP_ORB);

        D3D11_BUFFER_DESC vertexDesc{
            .ByteWidth = m_iVertexStride * m_iNumVertices,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };

        D3D11_SUBRESOURCE_DATA vertexSubResource{
            .pSysMem = vertices.data()
        };

        m_iNumIndices = (uint32_t)indices.size();
        m_iIndexStride = sizeof(uint16_t); // 2바이트면 충분?
        D3D11_BUFFER_DESC indexDesc{
            .ByteWidth = m_iIndexStride * m_iNumIndices,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
        };

        D3D11_SUBRESOURCE_DATA indexSubResource{
            .pSysMem = indices.data()
        };
        m_eIndexFormat = DXGI_FORMAT_R16_UINT;
        if (FAILED(CreateVertexBuffer(vertexDesc, &vertexSubResource)))
        {
            m_eState = STATE::LOADFAIL;
            return E_FAIL;
        }

        if (FAILED(CreateIndexBuffer(indexDesc, &indexSubResource)))
        {
            m_eState = STATE::LOADFAIL;
            return E_FAIL;
        }
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResExperenceOrbVIBuffer::Unload(const std::any& arg)
{
    return S_OK;
}

SPtr<CResExperenceOrbVIBuffer> CResExperenceOrbVIBuffer::Create()
{
    return ToSPtr(new CResExperenceOrbVIBuffer{ "", CGameInstance::Get().GetGraphicDevice() , CGameInstance::Get().GetGraphicDeviceContext() });
}
