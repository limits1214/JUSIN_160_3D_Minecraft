#include "pch.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"

NS_USING(Engine)
CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}
CRenderer::~CRenderer()
{
}

HRESULT CRenderer::Initialize()
{
    m_pBackBufferDSV = CGameInstance::Get().GetBackBufferDSV();
    m_pBackBufferRTV = CGameInstance::Get().GetBackBufferRTV();
    m_pBackBufferVP = CGameInstance::Get().GetResourceFirst<CResViewPort>(TAG_RES_GRP_PERMANENT_VP, "VP_BackBuffer");

    if (FAILED(InitializeOffscreen()))
    {
        return E_FAIL;
    }
    if (FAILED(InitializeShadow()))
    {
        return E_FAIL;
    }

    if (FAILED(InitializeFullscreen()))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenderer::InitializeOffscreen()
{// offscreenTexture
    {
        auto vClientScreenSize = CGameInstance::Get().GetClientScreenSize();
        if (auto res = CGameInstance::Get().AddResource(TAG_RES_GRP_PERMANENT_BUFFER, "VIBuffer_FullscreenTex", E::CResQuadFullscreenTexBuffer::Create()))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
        }
        if (auto res = CGameInstance::Get().AddResourceT(TAG_RES_GRP_PERMANENT_TEXTURE, "DynTex2D_Offscreen", E::CResDynamicTexture2D::Create()))
        {
            //typedef struct tagDesc {
            //	D3D11_TEXTURE2D_DESC texDesc{};
            //	D3D11_SUBRESOURCE_DATA texSubResource{};
            //}DESC;

            //typedef struct D3D11_TEXTURE2D_DESC
            //{
            //	UINT Width;
            //	UINT Height;
            //	UINT MipLevels;
            //	UINT ArraySize;
            //	DXGI_FORMAT Format;
            //	DXGI_SAMPLE_DESC SampleDesc;
            //	D3D11_USAGE Usage;
            //	UINT BindFlags;
            //	UINT CPUAccessFlags;
            //	UINT MiscFlags;
            //} 	D3D11_TEXTURE2D_DESC;
            CResDynamicTexture2D::DESC Desc{};
            Desc.texDesc = {
                .Width = (UINT)vClientScreenSize.x,
                .Height = (UINT)vClientScreenSize.y,
                .MipLevels = 1,
                .ArraySize = 1,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {.Count = 1, .Quality = 0 },
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                .CPUAccessFlags = 0,
                .MiscFlags = 0
            };
            if (FAILED(res->Load(Desc)))
            {
                return E_FAIL;
            }
            if (FAILED(res->CreateSRV()))
            {
                return E_FAIL;
            }
            if (FAILED(res->CreateRTV()))
            {
                return E_FAIL;
            }
            m_pOffScreenTex2D = res;
        }
    }

    if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_FullscreenQuad", "./Resources/Shader/FullscreenQuad/FullscreenQuad.hlsl"))
    {
        if (FAILED(res->Load()))
        {
            return E_FAIL;
        }
    }
    if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_FullscreenQuad", "./Resources/Shader/FullscreenQuad/FullscreenQuad.hlsl"))
    {
        if (FAILED(res->Load()))
        {
            return E_FAIL;
        }
    }
    return S_OK;
}

HRESULT CRenderer::InitializeShadow()
{
    UINT iShadowWidth = 2048 * 2;
    UINT iShadowHeight = 2048 * 2;

    if (auto res = CGameInstance::Get().AddResourceT(TAG_RES_GRP_PERMANENT_TEXTURE, "DynTex2D_Shadow", E::CResDynamicTexture2D::Create()))
    {
        D3D11_TEXTURE2D_DESC texDesc{};
        texDesc.Width = iShadowWidth;
        texDesc.Height = iShadowHeight;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = 0;

        CResDynamicTexture2D::DESC DynTex2DDesc{};
        DynTex2DDesc.texDesc = texDesc;
        if (FAILED(res->Load(DynTex2DDesc)))
        {
            return E_FAIL;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;
        if (FAILED(res->CreateDSV(dsvDesc)))
        {
            return E_FAIL;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        if (FAILED(res->CreateSRV(srvDesc)))
        {
            return E_FAIL;
        }

        m_pShadowTex2D = res;
    }
    if (auto res = CGameInstance::Get().AddResourceT(TAG_RES_GRP_PERMANENT_VP, "VP_Shadow", E::CResViewPort::Create()))
    {
        D3D11_VIEWPORT Desc{};
        Desc.TopLeftX = 0.f;
        Desc.TopLeftY = 0.f;
        Desc.Width = static_cast<float>(iShadowWidth);
        Desc.Height = static_cast<float>(iShadowHeight);
        Desc.MinDepth = 0.f;
        Desc.MaxDepth = 1.f;
        if (FAILED(res->Load(Desc)))
        {
            return E_FAIL;
        }
        m_pShadowVP = res;
    }
    
    return S_OK;
}

HRESULT CRenderer::InitializeFullscreen()
{
    m_pFullscreenVS = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_FullscreenQuad");
    m_pFullscreenPS= E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_FullscreenQuad");
    m_pFullscreenVIBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "VIBuffer_FullscreenTex");
    return S_OK;
}

HRESULT CRenderer::AddRenderObject(RENDERGROUP eRenderGroup, IRenderable* pRenderObject)
{
    if (eRenderGroup >= RENDERGROUP::END ||
        nullptr == pRenderObject)
        return E_FAIL;

    m_RenderObject[ETOUI(eRenderGroup)].push_back(pRenderObject);
    return S_OK;
}

HRESULT CRenderer::Draw()
{
    RENDER_CTX ctx{};
    CCameraObject* pShadowCamera{};


    _bool bApplyShadow = false;
    bApplyShadow = CGameInstance::Get().GetWorldDayFactor() > 0.64f ? true : false;
    if (bApplyShadow)
    {
        //draw shadow texture
        {
            // unbinding shadow map
            {
                ID3D11ShaderResourceView* pShadowSRVs[1] = { nullptr };
                m_pContext->PSSetShaderResources(4, 1, pShadowSRVs);
            }


            ID3D11RenderTargetView* pRTVs[1] = { nullptr };
            m_pContext->OMSetRenderTargets(1, pRTVs, m_pShadowTex2D->GetDSV().Get());
            m_pContext->ClearDepthStencilView(m_pShadowTex2D->GetDSV().Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
            m_pContext->RSSetViewports(1, &m_pShadowVP->GetViewPort());

            ctx.pass = RENDERPASS::SHADOW;

            {
                pShadowCamera = CGameInstance::Get().GetGameCamera("Shadow");
                if (pShadowCamera)
                {
                    ctx.matProj = pShadowCamera->GetProj();
                    ctx.matView = pShadowCamera->GetView();
                    ctx.matViewProj = ctx.matView * ctx.matProj;
                    ctx.eye = pShadowCamera->GetTransform().GetLoadedPostion();

                    auto pCbPerFrame = CGameInstance::Get().GetResourceFirst<CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_FRAME);
                    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                    if (SUCCEEDED(m_pContext->Map(pCbPerFrame->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
                    {
                        CB_PER_FRAME cbPerFrame{};
                        XMStoreFloat4x4(&cbPerFrame.matProj, pShadowCamera->GetProj());
                        XMStoreFloat4x4(&cbPerFrame.matView, pShadowCamera->GetView());
                        XMStoreFloat4x4(&cbPerFrame.matViewProj, pShadowCamera->GetView() * pShadowCamera->GetProj());
                        XMStoreFloat4x4(&cbPerFrame.matInvView, XMMatrixInverse(nullptr, pShadowCamera->GetView()));
                        XMStoreFloat4x4(&cbPerFrame.matInvViewProj, XMMatrixInverse(nullptr, XMLoadFloat4x4(&cbPerFrame.matViewProj)));
                        XMStoreFloat4x4(&cbPerFrame.matSkyRotation, XMMatrixRotationX(CGameInstance::Get().GetWorldSkyRotation()));
                        float starAngle = CGameInstance::Get().GetWorldSkyRotation() * -0.1f;
                        XMStoreFloat4x4(&cbPerFrame.matStarRotation, XMMatrixRotationX(starAngle));
                        cbPerFrame.fDayFactor = CGameInstance::Get().GetWorldDayFactor();
                        cbPerFrame.vCamPos = pShadowCamera->GetTransform().GetPosition();


                        memcpy(mappedSubResource.pData, &cbPerFrame, sizeof(cbPerFrame));
                        m_pContext->Unmap(pCbPerFrame->GetCBuffer().Get(), 0);
                    }
                    m_pContext->VSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
                    m_pContext->PSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
                    m_pContext->GSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());




                    if (FAILED(RenderNonBlend(ctx)))
                    {
                        return E_FAIL;
                    }


                }
            }
        }
    }
    
    
    // draw offscreen texture
    {
        ID3D11RenderTargetView* pRTVs[1] = { m_pOffScreenTex2D->GetRTV().Get() };
        m_pContext->OMSetRenderTargets(1, pRTVs, m_pBackBufferDSV.Get());
        m_pContext->RSSetViewports(1, &m_pBackBufferVP->GetViewPort());

        

        ctx.pass = RENDERPASS::DEFAULT;
        _float4 clearColor = { 0.f, 0.f, 1.f, 1.f };
        m_pContext->ClearRenderTargetView(pRTVs[0], reinterpret_cast<const float*>(&clearColor));
        m_pContext->ClearDepthStencilView(m_pBackBufferDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // binding shadow map
        {
            ID3D11ShaderResourceView* pShadowSRVs[1] = { m_pShadowTex2D->GetSRV().Get() };
            m_pContext->PSSetShaderResources(4, 1, pShadowSRVs);
        }

        
        {
            auto pGameCam = CGameInstance::Get().GetActiveGameCamera();
            if (!pGameCam)
            {
                return S_OK;
            }

            auto dirLight = CGameInstance::Get().GetDirectionalLight("0_Test");
            {
                ctx.matProj = pGameCam->GetProj();
                ctx.matView = pGameCam->GetView();
                ctx.matViewProj = ctx.matView * ctx.matProj;
                ctx.eye = pGameCam->GetTransform().GetLoadedPostion();

                auto pCbPerFrame = CGameInstance::Get().GetResourceFirst<CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_FRAME);
                D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                if (SUCCEEDED(m_pContext->Map(pCbPerFrame->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
                {
                    CB_PER_FRAME cbPerFrame{};
                    XMStoreFloat4x4(&cbPerFrame.matProj, pGameCam->GetProj());
                    XMStoreFloat4x4(&cbPerFrame.matView, pGameCam->GetView());
                    XMStoreFloat4x4(&cbPerFrame.matViewProj, pGameCam->GetView() * pGameCam->GetProj());
                    XMStoreFloat4x4(&cbPerFrame.matInvView, XMMatrixInverse(nullptr, pGameCam->GetView()));
                    XMStoreFloat4x4(&cbPerFrame.matInvViewProj, XMMatrixInverse(nullptr, XMLoadFloat4x4(&cbPerFrame.matViewProj)));
                    XMStoreFloat4x4(&cbPerFrame.matSkyRotation, XMMatrixRotationX(CGameInstance::Get().GetWorldSkyRotation()));
                    float starAngle = CGameInstance::Get().GetWorldSkyRotation() * -0.1f;
                    XMStoreFloat4x4(&cbPerFrame.matStarRotation, XMMatrixRotationX(starAngle));
                    cbPerFrame.fDayFactor = CGameInstance::Get().GetWorldDayFactor();
                    cbPerFrame.vCamPos = pGameCam->GetTransform().GetPosition();
                    if (dirLight.has_value())
                    {
                        cbPerFrame.dirLight = dirLight.value();
                    }

                    if (pShadowCamera)
                    {
                        XMStoreFloat4x4(&cbPerFrame.matShadowLightViewProj, pShadowCamera->GetView()* pShadowCamera->GetProj());
                    }

                    memcpy(mappedSubResource.pData, &cbPerFrame, sizeof(cbPerFrame));
                    m_pContext->Unmap(pCbPerFrame->GetCBuffer().Get(), 0);
                }
                m_pContext->VSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
                m_pContext->PSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
                m_pContext->GSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
            }
        }

        if (FAILED(RenderPriority(ctx)))
        {
            return E_FAIL;
        }

        if (FAILED(RenderNonBlend(ctx)))
        {
            return E_FAIL;
        }

        if (FAILED(RenderBlend(ctx)))
        {
            return E_FAIL;
        }

        if (FAILED(RenderSkybox(ctx)))
        {
            return E_FAIL;
        }

        if (FAILED(RenderCollider(ctx)))
        {
            return E_FAIL;
        }


        {
            auto pUICame = CGameInstance::Get().GetActiveUICamera();
            if (!pUICame)
            {
                return S_OK;
            }
            {
                ctx.matProj = pUICame->GetProj();
                ctx.matView = pUICame->GetView();
                ctx.matViewProj = ctx.matView * ctx.matProj;
                ctx.eye = pUICame->GetTransform().GetLoadedPostion();

                auto pCbPerFrame = CGameInstance::Get().GetResourceFirst<CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_FRAME);
                D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                if (SUCCEEDED(m_pContext->Map(pCbPerFrame->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
                {
                    CB_PER_FRAME cbPerFrame{};
                    XMStoreFloat4x4(&cbPerFrame.matProj, pUICame->GetProj());
                    XMStoreFloat4x4(&cbPerFrame.matView, pUICame->GetView());
                    XMStoreFloat4x4(&cbPerFrame.matViewProj, pUICame->GetView() * pUICame->GetProj());
                    XMStoreFloat4x4(&cbPerFrame.matInvView, XMMatrixInverse(nullptr, pUICame->GetView()));
                    //XMStoreFloat4x4(&cbPerFrame.matInvViewProj, XMMatrixInverse(nullptr, XMLoadFloat4x4(&cbPerFrame.matViewProj)));
                    cbPerFrame.vCamPos = pUICame->GetTransform().GetPosition();
                    cbPerFrame.fDayFactor = CGameInstance::Get().GetWorldDayFactor();
                    memcpy(mappedSubResource.pData, &cbPerFrame, sizeof(cbPerFrame));
                    m_pContext->Unmap(pCbPerFrame->GetCBuffer().Get(), 0);
                }
                m_pContext->VSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
                m_pContext->PSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
                m_pContext->GSSetConstantBuffers(1, 1, pCbPerFrame->GetCBuffer().GetAddressOf());
            }
        }

        if (FAILED(RenderUI(ctx)))
        {
            return E_FAIL;
        }

        if (FAILED(RenderUIToolTip(ctx)))
        {
            return E_FAIL;
        }

        // unbinding shadow map
        {
            ID3D11ShaderResourceView* pShadowSRVs[1] = { nullptr };
            m_pContext->PSSetShaderResources(4, 1, pShadowSRVs);
        }
    }


    // draw fullscreen
    {
        if (FAILED(DrawFullscreen()))
        {
            return E_FAIL;
        }
    }


    return S_OK;
}

void CRenderer::FrameEnd()
{
    for (auto& vecRenderables : m_RenderObject)
    {
        vecRenderables.clear();
    }
}

HRESULT CRenderer::DrawFullscreen()
{
    ID3D11RenderTargetView* pBackBufferRTVs[1] = { m_pBackBufferRTV.Get() };
    m_pContext->OMSetRenderTargets(1, pBackBufferRTVs, nullptr);

    _float4 clearColor = { 0.f, 0.f, 1.f, 1.f };
    m_pContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), reinterpret_cast<float*>(&clearColor));


    const auto& vs = m_pFullscreenVS;
    const auto& ps = m_pFullscreenPS;
    const auto& viBuffer = m_pFullscreenVIBuffer;

    m_pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
    m_pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

    m_pContext->IASetInputLayout(vs->GetInputLayout().Get());

    ID3D11Buffer* vertexBuffers[] = {
            viBuffer->GetVertexBuffer().Get()
    };
    uint32_t strides[] = {
        viBuffer->GetVertexStride()
    };
    uint32_t offsets[] = {
        0
    };
    m_pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    m_pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
    m_pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

    if (CGameInstance::Get().KeyPressing(DIK_N))
    {
        ID3D11ShaderResourceView* pSRVs[1] = { m_pShadowTex2D->GetSRV().Get() };
        m_pContext->PSSetShaderResources(0, 1, pSRVs);
    }
    else
    {
        ID3D11ShaderResourceView* pSRVs[1] = { m_pOffScreenTex2D->GetSRV().Get() };
        m_pContext->PSSetShaderResources(0, 1, pSRVs);
    }


    const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_LINEAR_WRAP);
    m_pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());

    m_pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);

    ID3D11ShaderResourceView* pNullSRVs[1] = { nullptr };
    m_pContext->PSSetShaderResources(0, 1, pNullSRVs);
    return S_OK;
}

HRESULT CRenderer::RenderPriority(const RENDER_CTX& ctx)
{
    for (auto& pRenderObject : m_RenderObject[ETOUI(RENDERGROUP::PRIORITY)])
    {
        if (pRenderObject->HasRenderPass(ctx.pass))
        {
            pRenderObject->Render(m_pContext.Get(), ctx);
        }
    }

    return S_OK;
}

HRESULT CRenderer::RenderNonBlend(const RENDER_CTX& ctx)
{
    for (auto& pRenderObject : m_RenderObject[ETOUI(RENDERGROUP::NONBLEND)])
    {
        if (pRenderObject->HasRenderPass(ctx.pass))
        {
            pRenderObject->Render(m_pContext.Get(), ctx);
        }
    }

    return S_OK;
}

HRESULT CRenderer::RenderBlend(const RENDER_CTX& ctx)
{
    for (auto& pRenderObject : m_RenderObject[ETOUI(RENDERGROUP::BLEND)])
    {
        if (pRenderObject->HasRenderPass(ctx.pass))
        {
            pRenderObject->Render(m_pContext.Get(), ctx);
        }
    }

    return S_OK;
}

HRESULT CRenderer::RenderSkybox(const RENDER_CTX& ctx)
{
    for (auto& pRenderObject : m_RenderObject[ETOUI(RENDERGROUP::SKYBOX)])
    {
        if (pRenderObject->HasRenderPass(ctx.pass))
        {
            pRenderObject->Render(m_pContext.Get(), ctx);
        }
    }

    return S_OK;
}

HRESULT CRenderer::RenderCollider(const RENDER_CTX& ctx)
{
    for (auto& pRenderObject : m_RenderObject[ETOUI(RENDERGROUP::COLLIDER)])
    {
        if (pRenderObject->HasRenderPass(ctx.pass))
        {
            pRenderObject->Render(m_pContext.Get(), ctx);
        }
    }

    return S_OK;
}

HRESULT CRenderer::RenderUI(const RENDER_CTX& ctx)
{
    for (auto& pRenderObject : m_RenderObject[ETOUI(RENDERGROUP::UI)])
    {
        if (pRenderObject->HasRenderPass(ctx.pass))
        {
            pRenderObject->Render(m_pContext.Get(), ctx);
        }
    }

    {
        E::CGameInstance::Get().FontLateDraw(RENDERGROUP::UI);
    }

    return S_OK;
}
HRESULT CRenderer::RenderUIToolTip(const RENDER_CTX& ctx)
{
    for (auto& pRenderObject : m_RenderObject[ETOUI(RENDERGROUP::UI_TOOLTIP)])
    {
        if (pRenderObject->HasRenderPass(ctx.pass))
        {
            pRenderObject->Render(m_pContext.Get(), ctx);
        }
    }

    {
        E::CGameInstance::Get().FontLateDraw(RENDERGROUP::UI_TOOLTIP);
    }

    return S_OK;
}

UPtr<CRenderer> CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = ToUPtr(new CRenderer{ pDevice, pContext });
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        return nullptr;
    }
    return pInstance;
}
