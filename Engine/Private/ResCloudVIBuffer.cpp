#include "ResCloudVIBuffer.h"

#include "GameInstance.h"
#include "ResTexture2D.h"

NS_USING(Engine)

CResCloudVIBuffer::CResCloudVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CResVIBuffer{ sPath, pDevice, pContext }
{
}

CResCloudVIBuffer::~CResCloudVIBuffer()
{
}

HRESULT CResCloudVIBuffer::Load(const std::any& arg)
{
    auto argDesc = std::any_cast<DESC>(&arg);
    if (!argDesc)
        return E_FAIL;
    if (m_eState == STATE::LOADED)
        return S_OK;

    m_eState = STATE::LOADING;
    {
        std::vector<VTX_CLOUD> vertices =
        {
            // 앞면 (Z+)
            {_float3{-0.5f,  0.5f,  0.5f}},
            {_float3{-0.5f, -0.5f,  0.5f}},
            {_float3{ 0.5f, -0.5f,  0.5f}},
            {_float3{ 0.5f,  0.5f,  0.5f}},

            // 뒷면 (Z-)
            {_float3{-0.5f,  0.5f, -0.5f}},
            {_float3{ 0.5f,  0.5f, -0.5f}},
            {_float3{ 0.5f, -0.5f, -0.5f}},
            {_float3{-0.5f, -0.5f, -0.5f}},

            // 오른쪽 (X+)
            {_float3{ 0.5f,  0.5f, -0.5f}},
            {_float3{ 0.5f,  0.5f,  0.5f}},
            {_float3{ 0.5f, -0.5f,  0.5f}},
            {_float3{ 0.5f, -0.5f, -0.5f}},

            // 왼쪽 (X-)
            {_float3{-0.5f,  0.5f,  0.5f}},
            {_float3{-0.5f,  0.5f, -0.5f}},
            {_float3{-0.5f, -0.5f, -0.5f}},
            {_float3{-0.5f, -0.5f,  0.5f}},

            // 위쪽 (Y+)
            {_float3{-0.5f,  0.5f,  0.5f}},
            {_float3{ 0.5f,  0.5f,  0.5f}},
            {_float3{ 0.5f,  0.5f, -0.5f}},
            {_float3{-0.5f,  0.5f, -0.5f}},

            // 아래쪽 (Y-)
            {_float3{-0.5f, -0.5f, -0.5f}},
            {_float3{ 0.5f, -0.5f, -0.5f}},
            {_float3{ 0.5f, -0.5f,  0.5f}},
            {_float3{-0.5f, -0.5f,  0.5f}},
        };

        constexpr float DROP_SCALE = 1.0f;

        for (auto& v : vertices)
        {
            v.pos.x *= DROP_SCALE;
            v.pos.y *= DROP_SCALE;
            v.pos.z *= DROP_SCALE;
        }

        std::vector<uint16_t> indices;
        for (uint16_t i = 0; i < 6; i++)  // 면 6개
        {
            uint16_t base = i * 4;
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }

        m_iNumVertices = (uint32_t)vertices.size();
        m_iVertexStride = sizeof(VTX_CLOUD);

        D3D11_BUFFER_DESC vertexDesc{
            .ByteWidth = m_iVertexStride * m_iNumVertices,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };
        D3D11_SUBRESOURCE_DATA vertexSubResource{ .pSysMem = vertices.data() };

        m_iNumIndices = (uint32_t)indices.size();
        m_iIndexStride = sizeof(uint16_t);
        D3D11_BUFFER_DESC indexDesc{
            .ByteWidth = m_iIndexStride * m_iNumIndices,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
        };
        D3D11_SUBRESOURCE_DATA indexSubResource{ .pSysMem = indices.data() };

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

HRESULT CResCloudVIBuffer::Unload(const std::any& arg)
{
    return S_OK;
}

SPtr<CResCloudVIBuffer> CResCloudVIBuffer::Create()
{
    return ToSPtr(new CResCloudVIBuffer{ "", CGameInstance::Get().GetGraphicDevice() , CGameInstance::Get().GetGraphicDeviceContext() });
}
