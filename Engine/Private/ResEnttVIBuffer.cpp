#include "ResEnttVIBuffer.h"

#include "GameInstance.h"
#include "ResEnttGeo.h"

NS_USING(Engine)

CResEnttVIBuffer::CResEnttVIBuffer(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CResVIBuffer{ sPath, pDevice, pContext } 
{
}

CResEnttVIBuffer::~CResEnttVIBuffer()
{
}

HRESULT CResEnttVIBuffer::Load(const std::any& arg)
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
		auto resources = CGameInstance::Get().GetResourceFirst<CResEnttGeo>(argDesc->geometryId.first, argDesc->geometryId.second);
		
        
        const auto& geometry = resources->GetGeometry();


        uint32_t iCntCube{};
        for (const auto& bone : geometry.bones)
        {
            for (const auto& cube : bone.cubes)
            {
                ++iCntCube;
            }
        }


        std::vector<VTX_ENTITY> vertices{};
        vertices.resize(24 * iCntCube);

        uint32_t boneIdx = 0;
        uint32_t tmpI{};
        for (const auto& bone : geometry.bones)
        {
            for (const auto& cube : bone.cubes)
            {
                float x0 = cube.origin.x, x1 = cube.origin.x + cube.size.x;
                float y0 = cube.origin.y, y1 = cube.origin.y + cube.size.y;
                float z0 = cube.origin.z, z1 = cube.origin.z + cube.size.z;

                float tw = (float)geometry.texWidth, th = (float)geometry.texHeight; // texture_width, texture_height
                float U = cube.uv.x, V = cube.uv.y;     // uv 시작점 (json의 "uv": [U, V])
                float W = cube.uvSize.x, H = cube.uvSize.y, D = cube.uvSize.z; // 8, 8, 8

                auto uv = [&](float px, float py) -> E::_float2 {
                    return { px / tw, py / th };
                    };
                /*
                     텍스처 좌표 = 픽셀 / texture_size 로 정규화

                     size = [W, H, D] 큐브일 때:

                            D    W
                         ┌────┬────┬────┬────┐
                       D │         |  top   │bot     │        │
                         ├────┼────┼────┼────┤
                       H │rgt      │frnt   │lft     │back    │
                         └────┴────┴────┴────┘

                     시작점 (U, V) 기준:
                       top   : (U+D,       V    ) ~ (U+D+W,     V+D  )
                       bottom: (U+D+W,     V    ) ~ (U+D+W+W,   V+D  )
                       right : (U,         V+D  ) ~ (U+D,       V+D+H)
                       front : (U+D,       V+D  ) ~ (U+D+W,     V+D+H)
                       left  : (U+D+W,     V+D  ) ~ (U+D+W+D,   V+D+H)
                       back  : (U+D+W+D,   V+D  ) ~ (U+D+W+D+W, V+D+H)

                     좌표계 변환 (베드락 오른손 → DX 왼손):
                       X반전 + Z반전 적용 (= Y축 180도와 동일, winding 유지)
                       → POS_X(x1면) = 베드락 left face  (U~U+D)
                       → NEG_X(x0면) = 베드락 right face (U+D+W~U+D+W+D)
                       → POS_Z(z1면) = 베드락 front face (U+D~U+D+W)
                       → NEG_Z(z0면) = 베드락 back face  (U+D+W+D~U+D+W+D+W)
                       → POS_Y/NEG_Y UV는 X반전으로 좌우 반전

                     mirror:
                       베드락 mirror는 left/right face를 스왑하고 각 face 내부 U를 반전
                       → POS_X = right face (U+D+W~U+D+W+D), U 반전
                       → NEG_X = left face  (U~U+D),          U 반전
                       → 나머지 face도 내부 U 반전
                */


                if (bone.mirror)
                {
                    // POS_X
                    vertices[24 * tmpI + 0] =  { .pos = {x1, y1, z0}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D + W + D,  V + D)} };
                    vertices[24 * tmpI + 1] =  { .pos = {x1, y1, z1}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D + W,      V + D)} };
                    vertices[24 * tmpI + 2] =  { .pos = {x1, y0, z1}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D + W,      V + D + H)} };
                    vertices[24 * tmpI + 3] =  { .pos = {x1, y0, z0}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D + W + D,  V + D + H)} };

                    // NEG_X
                    vertices[24 * tmpI + 4] =  { .pos = {x0, y1, z1}, .normal = {1, 0, 0},  .texCoord = {uv(U + D,  V + D)} };
                    vertices[24 * tmpI + 5] =  { .pos = {x0, y1, z0}, .normal = {1, 0, 0},  .texCoord = {uv(U,      V + D)} };
                    vertices[24 * tmpI + 6] =  { .pos = {x0, y0, z0}, .normal = {1, 0, 0},  .texCoord = {uv(U,      V + D + H)} };
                    vertices[24 * tmpI + 7] =  { .pos = {x0, y0, z1}, .normal = {1, 0, 0},  .texCoord = {uv(U + D,  V + D + H)} };

                    // POS_Y
                    vertices[24 * tmpI + 8] =  { .pos = {x0, y1, z1}, .normal = {0, 1, 0},  .texCoord = {uv(U + D,      V + D)} };
                    vertices[24 * tmpI + 9] =  { .pos = {x1, y1, z1}, .normal = {0, 1, 0},  .texCoord = {uv(U + D + W,  V + D)} };
                    vertices[24 * tmpI + 10] = { .pos = {x1, y1, z0}, .normal = {0, 1, 0},  .texCoord = {uv(U + D + W,  V)} };
                    vertices[24 * tmpI + 11] = { .pos = {x0, y1, z0}, .normal = {0, 1, 0},  .texCoord = {uv(U + D,      V)} };

                    // NEG_Y
                    vertices[24 * tmpI + 12] = { .pos = {x1, y0, z1}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W + W,  V + D)} };
                    vertices[24 * tmpI + 13] = { .pos = {x0, y0, z1}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W,      V + D)} };
                    vertices[24 * tmpI + 14] = { .pos = {x0, y0, z0}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W,      V)} };
                    vertices[24 * tmpI + 15] = { .pos = {x1, y0, z0}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W + W,  V)} };

                    // POS_Z
                    vertices[24 * tmpI + 16] = { .pos = {x1, y1, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D + W,  V + D)} };
                    vertices[24 * tmpI + 17] = { .pos = {x0, y1, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D,      V + D)} };
                    vertices[24 * tmpI + 18] = { .pos = {x0, y0, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D,      V + D + H)} };
                    vertices[24 * tmpI + 19] = { .pos = {x1, y0, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D + W,  V + D + H)} };

                    // NEG_Z
                    vertices[24 * tmpI + 20] = { .pos = {x0, y1, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D + W,  V + D)} };
                    vertices[24 * tmpI + 21] = { .pos = {x1, y1, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D,      V + D)} };
                    vertices[24 * tmpI + 22] = { .pos = {x1, y0, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D,      V + D + H)} };
                    vertices[24 * tmpI + 23] = { .pos = {x0, y0, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D + W,  V + D + H)} };
                }
                else
                {

                    // POS_X
                    vertices[24 * tmpI + 0] =  { .pos = {x1, y1, z0}, .normal = {-1, 0, 0}, .texCoord = {uv(U,      V + D)} };
                    vertices[24 * tmpI + 1] =  { .pos = {x1, y1, z1}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D,  V + D)} };
                    vertices[24 * tmpI + 2] =  { .pos = {x1, y0, z1}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D,  V + D + H)} };
                    vertices[24 * tmpI + 3] =  { .pos = {x1, y0, z0}, .normal = {-1, 0, 0}, .texCoord = {uv(U,      V + D + H)} };

                    // NEG_X
                    vertices[24 * tmpI + 4] =  { .pos = {x0, y1, z1}, .normal = {1, 0, 0},  .texCoord = {uv(U + D + W,      V + D)} };
                    vertices[24 * tmpI + 5] =  { .pos = {x0, y1, z0}, .normal = {1, 0, 0},  .texCoord = {uv(U + D + W + D,  V + D)} };
                    vertices[24 * tmpI + 6] =  { .pos = {x0, y0, z0}, .normal = {1, 0, 0},  .texCoord = {uv(U + D + W + D,  V + D + H)} };
                    vertices[24 * tmpI + 7] =  { .pos = {x0, y0, z1}, .normal = {1, 0, 0},  .texCoord = {uv(U + D + W,      V + D + H)} };

                    // POS_Y
                    vertices[24 * tmpI + 8] =  { .pos = {x0, y1, z1}, .normal = {0, 1, 0},  .texCoord = {uv(U + D + W,  V + D)} };
                    vertices[24 * tmpI + 9] =  { .pos = {x1, y1, z1}, .normal = {0, 1, 0},  .texCoord = {uv(U + D,      V + D)} };
                    vertices[24 * tmpI + 10] = { .pos = {x1, y1, z0}, .normal = {0, 1, 0},  .texCoord = {uv(U + D,      V)} };
                    vertices[24 * tmpI + 11] = { .pos = {x0, y1, z0}, .normal = {0, 1, 0},  .texCoord = {uv(U + D + W,  V)} };

                    // NEG_Y
                    vertices[24 * tmpI + 12] = { .pos = {x1, y0, z1}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W,      V + D)} };
                    vertices[24 * tmpI + 13] = { .pos = {x0, y0, z1}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W + W,  V + D)} };
                    vertices[24 * tmpI + 14] = { .pos = {x0, y0, z0}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W + W,  V)} };
                    vertices[24 * tmpI + 15] = { .pos = {x1, y0, z0}, .normal = {0, -1, 0}, .texCoord = {uv(U + D + W,      V)} };

                    // POS_Z
                    vertices[24 * tmpI + 16] = { .pos = {x1, y1, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D,      V + D)} };
                    vertices[24 * tmpI + 17] = { .pos = {x0, y1, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D + W,  V + D)} };
                    vertices[24 * tmpI + 18] = { .pos = {x0, y0, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D + W,  V + D + H)} };
                    vertices[24 * tmpI + 19] = { .pos = {x1, y0, z1}, .normal = {0, 0, 1},  .texCoord = {uv(U + D,      V + D + H)} };

                    // NEG_Z
                    vertices[24 * tmpI + 20] = { .pos = {x0, y1, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D,      V + D)} };
                    vertices[24 * tmpI + 21] = { .pos = {x1, y1, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D + W,  V + D)} };
                    vertices[24 * tmpI + 22] = { .pos = {x1, y0, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D + W,  V + D + H)} };
                    vertices[24 * tmpI + 23] = { .pos = {x0, y0, z0}, .normal = {0, 0, -1}, .texCoord = {uv(U + D + W + D,      V + D + H)} };
                }

                if (cube.rotation.x != 0.f || cube.rotation.y != 0.f || cube.rotation.z != 0.f)
                {
                    XMFLOAT3 pivot = cube.pivot;
                    XMVECTOR vPivot = XMLoadFloat3(&pivot);

                    if (pivot.x == 0.f && pivot.y == 0.f && pivot.z == 0.f)
                    {
                        // 큐브 중심점
                        XMFLOAT3 center = {
                            (x0 + x1) * 0.5f,
                            (y0 + y1) * 0.5f,
                            (z0 + z1) * 0.5f
                        };
                        vPivot = XMLoadFloat3(&center);
                    }

                    XMMATRIX rotMat =
                        XMMatrixRotationX(XMConvertToRadians(cube.rotation.x)) *
                        XMMatrixRotationY(XMConvertToRadians(cube.rotation.y)) *
                        XMMatrixRotationZ(XMConvertToRadians(cube.rotation.z));

                    for (int i = 0; i < 24; i++)
                    {
                        auto& v = vertices[24 * tmpI + i];

                        XMVECTOR pos = XMLoadFloat3(&v.pos);

                        pos -= vPivot;
                        pos = XMVector3TransformCoord(pos, rotMat);
                        pos += vPivot;

                        XMStoreFloat3(&v.pos, pos);

                        XMVECTOR nor = XMLoadFloat3(&v.normal);
                        nor = XMVector3TransformNormal(nor, rotMat);

                        XMStoreFloat3(&v.normal, nor);
                    }
                }

                for (int i = 0; i < 24; i++) {
                    vertices[24 * tmpI + i].boneIndex = boneIdx;
                    vertices[24 * tmpI + i].texIndex = cube.texIndex;
                }

                ++tmpI;
            }
            ++boneIdx;
        }

        m_iNumVertices = (uint32_t)vertices.size();
        m_iVertexStride = sizeof(VTX_ENTITY);
        
        D3D11_BUFFER_DESC vertexDesc{
            .ByteWidth = m_iVertexStride * m_iNumVertices,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };

        D3D11_SUBRESOURCE_DATA vertexSubResource{
            .pSysMem = vertices.data()
        };

        std::vector<uint16_t> indices{};
        indices.resize(36 * iCntCube);
        int tmp = 0;
        for (uint32_t i = 0; i < 6 * iCntCube; i++) {
            indices[i * 6 + 0] = tmp + 0;
            indices[i * 6 + 1] = tmp + 1;
            indices[i * 6 + 2] = tmp + 2;

            indices[i * 6 + 3] = tmp + 0;
            indices[i * 6 + 4] = tmp + 2;
            indices[i * 6 + 5] = tmp + 3;

            tmp += 4;
        }

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

HRESULT CResEnttVIBuffer::Unload(const std::any& arg)
{
    return S_OK;
}

SPtr<CResEnttVIBuffer> CResEnttVIBuffer::Create()
{
    return ToSPtr(new CResEnttVIBuffer{ "", CGameInstance::Get().GetGraphicDevice() , CGameInstance::Get().GetGraphicDeviceContext() });
}
