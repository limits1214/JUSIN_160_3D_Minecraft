#include "ResExtrudedItemVIBuffer.h"

#include "GameInstance.h"
#include "ResTexture2D.h"

NS_USING(Engine)

CResExtrudedItemVIBuffer::CResExtrudedItemVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CResVIBuffer{ sPath, pDevice, pContext }
{
}

CResExtrudedItemVIBuffer::~CResExtrudedItemVIBuffer()
{
}

HRESULT CResExtrudedItemVIBuffer::Load(const std::any& arg)
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
        auto res = std::static_pointer_cast<CResTexture2D>(CGameInstance::Get().GetResource(argDesc->textureId.first)->at(argDesc->textureId.second)[argDesc->resourceIdx]);
        ComPtr<ID3D11Texture2D> texture = res->GetTexture();

        // 1. Staging 텍스처 생성
        D3D11_TEXTURE2D_DESC desc = res->GetTexture2DDesc();
        D3D11_TEXTURE2D_DESC stagingDesc = desc;
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDesc.BindFlags = 0;  // 중요! Staging은 바인드 플래그 없어야 함
        stagingDesc.MiscFlags = 0;

        ComPtr<ID3D11Texture2D> pStaging;
        m_pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStaging);

        // 2. GPU → Staging 복사
        m_pContext->CopyResource(pStaging.Get(), texture.Get());

        // 3. Map
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        m_pContext->Map(pStaging.Get(), 0, D3D11_MAP_READ, 0, &mapped);

        int texWidth = desc.Width;
        int texHeight = desc.Height;
        BYTE* pData = (BYTE*)mapped.pData;

        float pixelSize = 1.f / texWidth;   // 픽셀 하나 크기
        float depth = pixelSize;        // 두께 (1픽셀만큼)


        std::vector<VTX_ITEM> vertices{};
        std::vector<uint16_t> indices{};

        auto AddQuad = [&](
            _float3 p0, _float3 p1, _float3 p2, _float3 p3,
            _float2 uv0, _float2 uv1, _float2 uv2, _float2 uv3,
            _float3 normal)
            {
                uint16_t base = (uint16_t)vertices.size();
                vertices.push_back({ p0, normal, uv0 });
                vertices.push_back({ p1, normal, uv1 });
                vertices.push_back({ p2, normal, uv2 });
                vertices.push_back({ p3, normal, uv3 });
                //PackTexId(6, argDesc->resourceIdx)
                //    PackTexId(6, argDesc->resourceIdx)
                //    PackTexId(6, argDesc->resourceIdx)
                //    PackTexId(6, argDesc->resourceIdx)
                indices.push_back(base + 0);
                indices.push_back(base + 1);
                indices.push_back(base + 2);
                indices.push_back(base + 0);
                indices.push_back(base + 2);
                indices.push_back(base + 3);
            };

        for (int y = 0; y < texHeight; y++)
        {
            for (int x = 0; x < texWidth; x++)
            {
                BYTE* pixel = pData + y * mapped.RowPitch + x * 4;
                BYTE a = pixel[3];

                if (a <= 10) continue;

                float fx = x * pixelSize;
                float fy = 1.f - (y + 1) * pixelSize;  // Y 뒤집기
                float fz = 0.f;

                // UV (픽셀 중심)
                float u0 = (x) / (float)texWidth;
                float u1 = (x + 1) / (float)texWidth;
                float v0 = (y) / (float)texHeight;
                float v1 = (y + 1) / (float)texHeight;

                // 앞면 (Z+)
                AddQuad(
                    { fx,           fy,           depth },
                    { fx + pixelSize, fy,           depth },
                    { fx + pixelSize, fy + pixelSize, depth },
                    { fx,           fy + pixelSize, depth },
                    { u0, v1 }, { u1, v1 }, { u1, v0 }, { u0, v0 },
                    { 0, 0, 1 }
                );

                // 뒷면 (Z-)
                AddQuad(
                    { fx + pixelSize, fy,           0.f },
                    { fx,           fy,           0.f },
                    { fx,           fy + pixelSize, 0.f },
                    { fx + pixelSize, fy + pixelSize, 0.f },
                    { u1, v1 }, { u0, v1 }, { u0, v0 }, { u1, v0 },
                    { 0, 0, -1 }
                );

                if (1)
                {
                    // 옆면 4개는 인접 픽셀이 없을 때만 생성 (그리디 메싱 핵심)
                    auto IsOpaque = [&](int nx, int ny) -> bool
                        {
                            if (nx < 0 || nx >= texWidth || ny < 0 || ny >= texHeight) return false;
                            return pData[ny * mapped.RowPitch + nx * 4 + 3] > 10;
                        };

                    float umid = (u0 + u1) * 0.5f;
                    float vmid = (v0 + v1) * 0.5f;

                    // 오른쪽 면
                    if (!IsOpaque(x + 1, y))
                        AddQuad(
                            { fx + pixelSize, fy,             depth },
                            { fx + pixelSize, fy,             0.f },
                            { fx + pixelSize, fy + pixelSize, 0.f },
                            { fx + pixelSize, fy + pixelSize, depth },
                            { umid, vmid }, { umid, vmid }, { umid, vmid }, { umid, vmid },
                            { 1, 0, 0 }
                        );

                    // 왼쪽 면
                    if (!IsOpaque(x - 1, y))
                        AddQuad(
                            { fx, fy,             0.f },
                            { fx, fy,             depth },
                            { fx, fy + pixelSize, depth },
                            { fx, fy + pixelSize, 0.f },
                            { umid, vmid }, { umid, vmid }, { umid, vmid }, { umid, vmid },
                            { -1, 0, 0 }
                        );

                    // 위쪽 면
                    if (!IsOpaque(x, y - 1))
                        AddQuad(
                            { fx,            fy + pixelSize, 0.f },
                            { fx,            fy + pixelSize, depth },
                            { fx + pixelSize, fy + pixelSize, depth },
                            { fx + pixelSize, fy + pixelSize, 0.f },
                            { umid, vmid }, { umid, vmid }, { umid, vmid }, { umid, vmid },
                            { 0, 1, 0 }
                        );

                    // 아래쪽 면
                    if (!IsOpaque(x, y + 1))
                        AddQuad(
                            { fx + pixelSize, fy, 0.f },
                            { fx + pixelSize, fy, depth },
                            { fx,            fy, depth },
                            { fx,            fy, 0.f },
                            { umid, vmid }, { umid, vmid }, { umid, vmid }, { umid, vmid },
                            { 0, -1, 0 }
                        );
                }
            }
        }

        m_pContext->Unmap(pStaging.Get(), 0);



        for ( auto& v : vertices)
        {
            v.pos.x -= pixelSize * texWidth * 0.5f;
            v.pos.y -= pixelSize * texHeight * 0.5f;
        }

        constexpr float DROP_SCALE = 0.5f;

        for (auto& v : vertices)
        {
            v.pos.x *= DROP_SCALE;
            v.pos.y *= DROP_SCALE;
            v.pos.z *= DROP_SCALE;
        }

        
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

        //indices.resize(36 * iCntCube);
        //int tmp = 0;
        //for (uint32_t i = 0; i < 6 * iCntCube; i++) {
        //    indices[i * 6 + 0] = tmp + 0;
        //    indices[i * 6 + 1] = tmp + 1;
        //    indices[i * 6 + 2] = tmp + 2;

        //    indices[i * 6 + 3] = tmp + 0;
        //    indices[i * 6 + 4] = tmp + 2;
        //    indices[i * 6 + 5] = tmp + 3;

        //    tmp += 4;
        //}

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

HRESULT CResExtrudedItemVIBuffer::Unload(const std::any& arg)
{
    return S_OK;
}

SPtr<CResExtrudedItemVIBuffer> CResExtrudedItemVIBuffer::Create()
{
    return ToSPtr(new CResExtrudedItemVIBuffer{ "", CGameInstance::Get().GetGraphicDevice() , CGameInstance::Get().GetGraphicDeviceContext() });
}
