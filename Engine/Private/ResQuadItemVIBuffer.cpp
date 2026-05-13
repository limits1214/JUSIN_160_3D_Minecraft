#include "ResQuadItemVIBuffer.h"

#include "GameInstance.h"
#include "ResTexture2D.h"

NS_USING(Engine)

CResQuadItemVIBuffer::CResQuadItemVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CResVIBuffer{ sPath, pDevice, pContext }
{
}

CResQuadItemVIBuffer::~CResQuadItemVIBuffer()
{
}

HRESULT CResQuadItemVIBuffer::Load(const std::any& arg)
{
    auto argDesc = std::any_cast<DESC>(&arg);
    if (!argDesc)
    {
        return E_FAIL;
    }
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {


        //{ p0, normal, uv0, PackTexId(6, argDesc->resourceIdx) }
        constexpr uint32_t iNumVertices = 4;
        std::vector<VTX_ITEM> vertices =
        {
            {_float3{-0.5f, 0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{0.f, 0.f},     argDesc->texIndex},
            {_float3{0.5f, 0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{1.f, 0.f},      argDesc->texIndex},
            {_float3{0.5f, -0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{1.f, 1.f},     argDesc->texIndex},
            {_float3{-0.5f, -0.5f, 0.f},_float3{0.f, 0.f, -1.f},  _float2{0.f, 1.f},    argDesc->texIndex}
        };

        std::vector<uint16_t> indices{ 0, 1, 2, 0, 2, 3 };


        m_iNumVertices = (uint32_t)vertices.size();
        m_iVertexStride = sizeof(VTX_ITEM);

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

HRESULT CResQuadItemVIBuffer::Unload(const std::any& arg)
{
    return S_OK;
}

SPtr<CResQuadItemVIBuffer> CResQuadItemVIBuffer::Create()
{
    return ToSPtr(new CResQuadItemVIBuffer{ "", CGameInstance::Get().GetGraphicDevice() , CGameInstance::Get().GetGraphicDeviceContext() });
}
