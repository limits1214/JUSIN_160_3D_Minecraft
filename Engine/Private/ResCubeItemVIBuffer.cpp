#include "ResCubeItemVIBuffer.h"

#include "GameInstance.h"
#include "ResTexture2D.h"

NS_USING(Engine)

CResCubeItemVIBuffer::CResCubeItemVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CResVIBuffer{ sPath, pDevice, pContext }
{
}

CResCubeItemVIBuffer::~CResCubeItemVIBuffer()
{
}

HRESULT CResCubeItemVIBuffer::Load(const std::any& arg)
{
    auto argDesc = std::any_cast<DESC>(&arg);
    if (!argDesc)
        return E_FAIL;
    if (m_eState == STATE::LOADED)
        return S_OK;

    m_eState = STATE::LOADING;
    {
        std::vector<VTX_ITEM> vertices =
        {
            // 앞면 (Z+)
            {_float3{-0.5f,  0.5f,  0.5f}, _float3{ 0.f,  0.f,  1.f}, _float2{0.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Z)]},
            {_float3{-0.5f, -0.5f,  0.5f}, _float3{ 0.f,  0.f,  1.f}, _float2{0.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Z)]},
            {_float3{ 0.5f, -0.5f,  0.5f}, _float3{ 0.f,  0.f,  1.f}, _float2{1.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Z)]},
            {_float3{ 0.5f,  0.5f,  0.5f}, _float3{ 0.f,  0.f,  1.f}, _float2{1.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Z)]},

            // 뒷면 (Z-)
            {_float3{-0.5f,  0.5f, -0.5f}, _float3{ 0.f,  0.f, -1.f}, _float2{0.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Z)]},
            {_float3{ 0.5f,  0.5f, -0.5f}, _float3{ 0.f,  0.f, -1.f}, _float2{1.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Z)]},
            {_float3{ 0.5f, -0.5f, -0.5f}, _float3{ 0.f,  0.f, -1.f}, _float2{1.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Z)]},
            {_float3{-0.5f, -0.5f, -0.5f}, _float3{ 0.f,  0.f, -1.f}, _float2{0.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Z)]},

            // 오른쪽 (X+)
            {_float3{ 0.5f,  0.5f, -0.5f}, _float3{ 1.f,  0.f,  0.f}, _float2{0.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_X)]},
            {_float3{ 0.5f,  0.5f,  0.5f}, _float3{ 1.f,  0.f,  0.f}, _float2{1.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_X)]},
            {_float3{ 0.5f, -0.5f,  0.5f}, _float3{ 1.f,  0.f,  0.f}, _float2{1.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_X)]},
            {_float3{ 0.5f, -0.5f, -0.5f}, _float3{ 1.f,  0.f,  0.f}, _float2{0.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_X)]},

            // 왼쪽 (X-)
            {_float3{-0.5f,  0.5f,  0.5f}, _float3{-1.f,  0.f,  0.f}, _float2{0.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_X)]},
            {_float3{-0.5f,  0.5f, -0.5f}, _float3{-1.f,  0.f,  0.f}, _float2{1.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_X)]},
            {_float3{-0.5f, -0.5f, -0.5f}, _float3{-1.f,  0.f,  0.f}, _float2{1.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_X)]},
            {_float3{-0.5f, -0.5f,  0.5f}, _float3{-1.f,  0.f,  0.f}, _float2{0.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_X)]},

            // 위쪽 (Y+)
            {_float3{-0.5f,  0.5f,  0.5f}, _float3{ 0.f,  1.f,  0.f}, _float2{0.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Y)]},
            {_float3{ 0.5f,  0.5f,  0.5f}, _float3{ 0.f,  1.f,  0.f}, _float2{1.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Y)]},
            {_float3{ 0.5f,  0.5f, -0.5f}, _float3{ 0.f,  1.f,  0.f}, _float2{1.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Y)]},
            {_float3{-0.5f,  0.5f, -0.5f}, _float3{ 0.f,  1.f,  0.f}, _float2{0.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::POS_Y)]},

            // 아래쪽 (Y-)
            {_float3{-0.5f, -0.5f, -0.5f}, _float3{ 0.f, -1.f,  0.f}, _float2{0.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Y)]},
            {_float3{ 0.5f, -0.5f, -0.5f}, _float3{ 0.f, -1.f,  0.f}, _float2{1.f, 0.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Y)]},
            {_float3{ 0.5f, -0.5f,  0.5f}, _float3{ 0.f, -1.f,  0.f}, _float2{1.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Y)]},
            {_float3{-0.5f, -0.5f,  0.5f}, _float3{ 0.f, -1.f,  0.f}, _float2{0.f, 1.f}, argDesc->texIndices[ETOUI(FACE_DIR::NEG_Y)]},
        };

        constexpr float DROP_SCALE = 0.25f;

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
        m_iVertexStride = sizeof(VTX_ITEM);

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

HRESULT CResCubeItemVIBuffer::Unload(const std::any& arg)
{
    return S_OK;
}

SPtr<CResCubeItemVIBuffer> CResCubeItemVIBuffer::Create()
{
    return ToSPtr(new CResCubeItemVIBuffer{ "", CGameInstance::Get().GetGraphicDevice() , CGameInstance::Get().GetGraphicDeviceContext() });
}
