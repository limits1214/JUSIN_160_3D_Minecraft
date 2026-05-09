#include "PigEntity.h"

#include "Resources.h"
#include "GameInstance.h"

NS_USING(Engine)





CPigEntity::CPigEntity()
{
}

CPigEntity::~CPigEntity()
{
}

HRESULT CPigEntity::Initialize(void* pArg)
{
    m_Bones = {
        EntityBone{
            .name = "root",
            .pivot = {0,0,0}
        },
        EntityBone{
            .name = "head",
            .parent = "root",
            .pivot = {0, 12, -7},
            .cubes = {
                EntityCube{.origin = {-4, 8, -15}, .size = {8, 8, 8}, .uv {0,0} },
                EntityCube{.origin = {-2, 9, -16}, .size = {4, 3, 1}, .uv {16, 16} }
            }
        },
        EntityBone{
            .name = "body",
            .parent = "root",
            .pivot = {0,0,0},
            .cubes = {
                 //EntityCube{.origin = {-5, 2, -5}, .size = {10, 16, 8}, .rotation = {90, 0, 0},.uv {28, 32}, .inflate = 0.5  },
                 EntityCube{.origin = {-5, 2, -5}, .size = {10, 16, 8}, .rotation = {90, 0, 0},.uv {28, 8},  },
            }
        },
        EntityBone{
            .name = "leg0",
            .parent = "root",
            .pivot = {-3, 6, 6},
            .cubes = {
                 EntityCube{.origin = {-5, 0, 4}, .size = {4, 6, 4}, .uv {0, 16},  },
            }
        },

        EntityBone{
            .name = "leg1",
            .parent = "root",
            .pivot = {3, 6, 6},
            .cubes = {
                 EntityCube{.origin = {1, 0, 4}, .size = {4, 6, 4}, .uv {0, 16},  },
            }
        },

        EntityBone{
            .name = "leg3",
            .parent = "root",
            .pivot = {3, 6, -6},
            .cubes = {
                 EntityCube{.origin = { 1, 0, -8}, .size = {4, 6, 4}, .uv {0, 16},  },
            }
        },

        EntityBone{
           .name = "leg2",
           .parent = "root",
           .pivot = {-3, 6, -6},
           .cubes = {
                EntityCube{.origin = { -5, 0, -8}, .size = {4, 6, 4}, .uv {0, 16},  },
           }
        } 
    };


    uint32_t iCntCube{};
    for (auto& bone : m_Bones)
    {
        for (auto& cube : bone.cubes)
        {
            ++iCntCube;

            // inflate 먼저
            cube.origin.x -= cube.inflate;
            cube.origin.y -= cube.inflate;
            cube.origin.z -= cube.inflate;
            cube.size.x += cube.inflate * 2.f;
            cube.size.y += cube.inflate * 2.f;
            cube.size.z += cube.inflate * 2.f;

            cube.origin.z = -cube.origin.z - cube.size.z;
        }
        bone.pivot.z = -bone.pivot.z;
    }
   
    auto viBuffer = CResDynamicVIBuffer::Create();
    CResDynamicVIBuffer::DESC desc{};

    std::vector<VTX_ENTITY> vertices{};
    vertices.resize(24 * iCntCube);

    uint32_t boneIdx = 0;
    uint32_t tmpI{};
    for (auto& bone : m_Bones)
    {
        for (auto& cube : bone.cubes)
        {
            float x0 = cube.origin.x, x1 = cube.origin.x + cube.size.x;
            float y0 = cube.origin.y, y1 = cube.origin.y + cube.size.y;
            float z0 = cube.origin.z, z1 = cube.origin.z + cube.size.z;

            float tw = 64.f, th = 64.f; // texture_width, texture_height
            float U = cube.uv.x, V = cube.uv.y;     // uv 시작점 (json의 "uv": [U, V])
            float W = cube.size.x, H = cube.size.y, D = cube.size.z; // 8, 8, 8
            auto uv = [&](float px, float py) -> E::_float2 {
                return { px / tw, py / th };
                };
            /*

텍스처 좌표 = 픽셀 / texture_size 로 정규화

size = [W, H, D] 큐브일 때:

       D    W    D    W
    ┌────┬────┬────┬────┐
  D │ top│    │bot │    │
    ├────┼────┼────┼────┤
  H │right│frnt│left│back│
    └────┴────┴────┴────┘

시작점 (U, V) 기준:
  top   : (U+D,     V    ) ~ (U+D+W, V+D  )
  bottom: (U+D+W,   V    ) ~ (U+D+W+W, V+D)
  right : (U,       V+D  ) ~ (U+D,   V+D+H)
  front : (U+D,     V+D  ) ~ (U+D+W, V+D+H)
  left  : (U+D+W,   V+D  ) ~ (U+D+W+D, V+D+H)
  back  : (U+D+W+D, V+D  ) ~ (U+D+W+D+W, V+D+H)
*/
            // POS_X
            vertices[24 * tmpI + 0] = { .pos = {x1, y1, z0}, .normal = {1, 0, 0}, .texCoord = {uv(U + D + W, V + D) } };
            vertices[24 * tmpI + 1] = { .pos = {x1, y1, z1}, .normal = {1, 0, 0}, .texCoord = {uv(U + D + W + D, V + D) } };
            vertices[24 * tmpI + 2] = { .pos = {x1, y0, z1}, .normal = {1, 0, 0}, .texCoord = {uv(U + D + W + D, V + D + H) } };
            vertices[24 * tmpI + 3] = { .pos = {x1, y0, z0}, .normal = {1, 0, 0}, .texCoord = {uv(U + D + W, V + D + H)} };


            // NEG_X
            vertices[24 * tmpI + 4] = { .pos = {x0, y1, z1}, .normal = {-1, 0, 0}, .texCoord = {uv(U, V + D)} };
            vertices[24 * tmpI + 5] = { .pos = {x0, y1, z0}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D, V + D)} };
            vertices[24 * tmpI + 6] = { .pos = {x0, y0, z0}, .normal = {-1, 0, 0}, .texCoord = {uv(U + D, V + D + H) } };
            vertices[24 * tmpI + 7] = { .pos = {x0, y0, z1}, .normal = {-1, 0, 0}, .texCoord = { uv(U, V + D + H)} };


            // POS_Y
            vertices[24 * tmpI + 8] = { .pos = {x0, y1, z1} , .normal = { 0, 1, 0}, .texCoord = {uv(U + D, V)} };
            vertices[24 * tmpI + 9] = { .pos = {x1, y1, z1 }, .normal =  {0, 1, 0}, .texCoord = {uv(U + D + W, V)} };
            vertices[24 * tmpI + 10] = { .pos = { x1, y1, z0}, .normal = {0, 1, 0}, .texCoord = {uv(U + D + W, V + D) } };
            vertices[24 * tmpI + 11] = { .pos =  {x0, y1, z0}, .normal = {0, 1, 0}, .texCoord = { uv(U + D, V + D)} };

            // NEG_Y
            vertices[24 * tmpI + 12] = { .pos = { x0, y0, z0 }, .normal = { 0, -1, 0 }, .texCoord = { uv(U + D + W, V) } };
            vertices[24 * tmpI + 13] = { .pos = { x1, y0, z0 }, .normal = { 0, -1, 0 }, .texCoord = { uv(U + D + W + W, V) } };
            vertices[24 * tmpI + 14] = { .pos = { x1, y0, z1 }, .normal = { 0, -1, 0 }, .texCoord = { uv(U + D + W + W, V + D) } };
            vertices[24 * tmpI + 15] = { .pos = { x0, y0, z1 }, .normal = { 0, -1, 0 }, .texCoord = { uv(U + D + W, V + D) } };

            // POS_Z
            vertices[24 * tmpI + 16] = { .pos = { x0, y1, z1 }, .normal = { 0, 0, 1 }, .texCoord = { uv(U + D, V + D) } };
            vertices[24 * tmpI + 17] = { .pos = { x0, y0, z1 }, .normal = { 0, 0, 1 }, .texCoord = { uv(U + D, V + D + H) } };
            vertices[24 * tmpI + 18] = { .pos = { x1, y0, z1 }, .normal = { 0, 0, 1 }, .texCoord = { uv(U + D + W, V + D + H) } };
            vertices[24 * tmpI + 19] = { .pos = { x1, y1, z1 }, .normal = { 0, 0, 1 }, .texCoord = { uv(U + D + W, V + D) } };

            // NEG_Z
            vertices[24 * tmpI + 20] = { .pos = { x0, y1, z0 }, .normal = {0, 0, -1 }, .texCoord = { uv(U + D + W + D, V + D) } };
            vertices[24 * tmpI + 21] = { .pos = { x1, y1, z0 }, .normal = {0, 0, -1 }, .texCoord = { uv(U + D + W + D + W, V + D) } };
            vertices[24 * tmpI + 22] = { .pos = { x1, y0, z0 }, .normal = {0, 0, -1 }, .texCoord = { uv(U + D + W + D + W, V + D + H) } };
            vertices[24 * tmpI + 23] = { .pos = { x0, y0, z0 }, .normal = {0, 0, -1 }, .texCoord = { uv(U + D + W + D, V + D + H) } };


            if (cube.rotation.x != 0.f || cube.rotation.y != 0.f || cube.rotation.z != 0.f)
            {
                // 큐브 중심점
                XMFLOAT3 center = {
                    (x0 + x1) * 0.5f,
                    (y0 + y1) * 0.5f,
                    (z0 + z1) * 0.5f
                };
                XMVECTOR vCenter = XMLoadFloat3(&center);

                XMMATRIX rotMat =
                    XMMatrixRotationX(XMConvertToRadians(cube.rotation.x)) *
                    XMMatrixRotationY(XMConvertToRadians(cube.rotation.y)) *
                    XMMatrixRotationZ(XMConvertToRadians(cube.rotation.z));

                for (int i = 0; i < 24; i++) {
                    auto& v = vertices[24 * tmpI + i];

                    XMVECTOR pos = XMLoadFloat3(&v.pos);
                    pos = pos - vCenter;
                    pos = XMVector3TransformCoord(pos, rotMat);
                    pos = pos + vCenter;
                    XMStoreFloat3(&v.pos, pos);

                    XMVECTOR nor = XMLoadFloat3(&v.normal);
                    nor = XMVector3TransformNormal(nor, rotMat);
                    XMStoreFloat3(&v.normal, nor);
                }
            }




            // pivot 로컬화 + boneIndex 태깅
            for (int i = 0; i < 24; i++) {
                //vertices[24 * tmpI + i].pos.x -= bone.pivot.x;
                //vertices[24 * tmpI + i].pos.y -= bone.pivot.y;
                //vertices[24 * tmpI + i].pos.z -= bone.pivot.z;
                vertices[24 * tmpI + i].boneIndex = boneIdx;
            }

            ++tmpI;
        }
        ++boneIdx;
    }

    desc.iNumVertices = (uint32_t)vertices.size();
    desc.iVertexStride = sizeof(VTX_ENTITY);
    desc.vertexDesc = {
        .ByteWidth = desc.iVertexStride * desc.iNumVertices,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
    };
    desc.vertexSubResource = {
        .pSysMem = vertices.data()
    };

    std::vector<uint16_t> indices{};
    indices.resize(36 * iCntCube);
    int tmp = 0;
    for (int i = 0; i < 6 * iCntCube; i++) {
        indices[i * 6 + 0] = tmp + 0;
        indices[i * 6 + 1] = tmp + 1;
        indices[i * 6 + 2] = tmp + 2;

        indices[i * 6 + 3] = tmp + 0;
        indices[i * 6 + 4] = tmp + 2;
        indices[i * 6 + 5] = tmp + 3;

        tmp += 4;
    }

    desc.iNumIndices = (uint32_t)indices.size();
    desc.iIndexStride = sizeof(uint16_t);
    desc.IndexDesc = {
        .ByteWidth = desc.iIndexStride * desc.iNumIndices,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
    };
    desc.indexSubResource = {
        .pSysMem = indices.data()
    };
    desc.eIndexFormat = DXGI_FORMAT_R16_UINT;

    if (FAILED(viBuffer->Load(desc)))
    {
        int x = 0;
    }

    CGameInstance::Get().AddResource("ENTITY_PIG", "VIBUFFER", viBuffer);



    {
        //_float4x4 boneMatrices[64];

        int boneIdx = 0;
        for (auto& bone : m_Bones) {
            XMStoreFloat4x4(&m_boneMatrices[boneIdx], XMMatrixTranslation(bone.pivot.x, bone.pivot.y, bone.pivot.z));
            ++boneIdx;
        }

        {
            auto pContext = CGameInstance::Get().GetGraphicDeviceContext();
            auto pCbPerBone = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBone");
            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
            if (SUCCEEDED(pContext->Map(pCbPerBone->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
            {

                E::CB_PER_BONE cbPerBone{};
                memcpy(cbPerBone.matBone, m_boneMatrices, sizeof(_float4x4) * 64);

                memcpy(mappedSubResource.pData, &cbPerBone, sizeof(cbPerBone));
                pContext->Unmap(pCbPerBone->GetCBuffer().Get(), 0);
            }
            pContext->VSSetConstantBuffers(4, 1, pCbPerBone->GetCBuffer().GetAddressOf());
            pContext->PSSetConstantBuffers(4, 1, pCbPerBone->GetCBuffer().GetAddressOf());
        }
    }
    
    

    if (FAILED(CAnimalEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }
    // texture
    {
        auto pRes = CResTexture2D::Create("./Resources/Texture/Entity/Pig/pig_v3.png");
        if (FAILED(pRes->Load()))
        {
            int x = 0;
        }
        CGameInstance::Get().AddResource("ENTITY_PIG", "TEX", pRes);
    }

    

	return S_OK;
}

void CPigEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CPigEntity::Update(E::_float fTimeDelta)
{
    static float fTmpElapsed = 0.f;
    fTmpElapsed += fTimeDelta * 0.1f;

    int boneIdx = 0;
    for (auto& bone : m_Bones)
    {
        XMMATRIX local = XMMatrixIdentity();

        if (bone.name == "leg0" || bone.name == "leg3") {
            float rotX = XMConvertToRadians(cosf(fTmpElapsed * 38.17f) * 80.f);
            local = XMMatrixRotationX(rotX);
        }
        else if (bone.name == "leg1" || bone.name == "leg2") {
            float rotX = XMConvertToRadians(cosf(fTmpElapsed * 38.17f) * -80.f);
            local = XMMatrixRotationX(rotX);
        }

        //XMVECTOR pivot = XMLoadFloat3(&bone.pivot);
        //XMMATRIX boneMatrix =
        //    XMMatrixTranslationFromVector(pivot)
        //    * local
        //    * XMMatrixTranslationFromVector(-pivot);

        XMVECTOR pivot = XMLoadFloat3(&bone.pivot);

        XMMATRIX boneMatrix =
            XMMatrixTranslationFromVector(-pivot)  // 1. pivot을 원점으로
            * local                                  // 2. 회전
            * XMMatrixTranslationFromVector(pivot);  // 3. pivot 위치로 복원

        XMStoreFloat4x4(&m_boneMatrices[boneIdx], boneMatrix);
        ++boneIdx;
    }
 
    //for (int i = 0; i < 64; i++)
    //    XMStoreFloat4x4(&m_boneMatrices[i], XMMatrixIdentity());


    {
        auto pContext = CGameInstance::Get().GetGraphicDeviceContext();
        auto pCbPerBone = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBone");
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        if (SUCCEEDED(pContext->Map(pCbPerBone->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        {
            E::CB_PER_BONE cbPerBone{};
            memcpy(cbPerBone.matBone, m_boneMatrices, sizeof(_float4x4) * 64);
            memcpy(mappedSubResource.pData, &cbPerBone, sizeof(cbPerBone));
            pContext->Unmap(pCbPerBone->GetCBuffer().Get(), 0);
        }
        pContext->VSSetConstantBuffers(4, 1, pCbPerBone->GetCBuffer().GetAddressOf());
    }
}

void CPigEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CPigEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Entity");
    const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Entity");

    {
        const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResDynamicVIBuffer>("ENTITY_PIG", "VIBUFFER");
        pContext->IASetInputLayout(vs->GetInputLayout().Get());
        pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
        pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

        ID3D11Buffer* vertexBuffers[] = {
                viBuffer->GetVertexBuffer().Get()
        };
        uint32_t strides[] = {
            viBuffer->GetVertexStride()
        };
        uint32_t offsets[] = {
            0
        };
        pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
        pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
        pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());



        {
            auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
            if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
            {

                E::CB_PER_OBJECT cbPerObject{};
                cbPerObject.matWorld = *GetTransform().GetWorldMatrix();
                XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedWorldMatrix() * ctx.matViewProj);

                memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
                pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
            }
            pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
            pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
        }
        {
            const auto& srv = E::CGameInstance::GetConst().GetResourceFirst<E::CResTexture2D>("ENTITY_PIG", "TEX");
            pContext->PSSetShaderResources(0, 1, srv->GetSRV().GetAddressOf());

            const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
            pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
        }

        pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
    }

    //{
    //    const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResDynamicVIBuffer>("ENTITY_PIG", "VIBUFFER_HEAD_1");
    //    pContext->IASetInputLayout(vs->GetInputLayout().Get());
    //    pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    //    pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

    //    ID3D11Buffer* vertexBuffers[] = {
    //            viBuffer->GetVertexBuffer().Get()
    //    };
    //    uint32_t strides[] = {
    //        viBuffer->GetVertexStride()
    //    };
    //    uint32_t offsets[] = {
    //        0
    //    };
    //    pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    //    pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
    //    pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());



    //    {
    //        auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
    //        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    //        if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
    //        {

    //            E::CB_PER_OBJECT cbPerObject{};
    //            cbPerObject.matWorld = *GetTransform().GetWorldMatrix();
    //            XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedWorldMatrix() * ctx.matViewProj);

    //            memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
    //            pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
    //        }
    //        pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
    //        pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
    //    }
    //    {
    //        const auto& srv = E::CGameInstance::GetConst().GetResourceFirst<E::CResTexture2D>("ENTITY_PIG", "TEX");
    //        pContext->PSSetShaderResources(0, 1, srv->GetSRV().GetAddressOf());

    //        const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
    //        pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
    //    }

    //    pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
    //}
    return S_OK;
}

UPtr<CPigEntity> CPigEntity::Create()
{
    auto pInstance = ToUPtr(new CPigEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CPigEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CPigEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CPigEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPigEntity");
        return nullptr;
    }

    return pInstance;
}
