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

    if (FAILED(InitializeOffscreen()))
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
    ID3D11RenderTargetView* pRTVs[1] = { m_pOffScreenTex2D->GetRTV().Get() };
    m_pContext->OMSetRenderTargets(1, pRTVs, m_pBackBufferDSV.Get());

    RENDER_CTX ctx{};


    
    //auto pDepthStencilView = m_pBackBufferDSV.Get();
    //ID3D11RenderTargetView* pRTVs[1] = { CGameInstance::Get().GetBackBufferRTV().Get() };
    //m_pContext->OMSetRenderTargets(1, pRTVs, pDepthStencilView);

    ctx.pass = RENDERPASS::DEFAULT;
    _float4 clearColor = { 0.f, 0.f, 1.f, 1.f };
    m_pContext->ClearRenderTargetView(pRTVs[0], reinterpret_cast<const float*>(&clearColor));
    m_pContext->ClearDepthStencilView(m_pBackBufferDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


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

    // Test
    {
        E::CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI, "NeoDGM_20px", L"흙이 운다 흙흑", {});
    }
    if (FAILED(RenderUI(ctx)))
    {
        return E_FAIL;
    }

    if (FAILED(RenderUIToolTip(ctx)))
    {
        return E_FAIL;
    }


    // draw fullscreen
    {
        ID3D11RenderTargetView* pBackBufferRTVs[1] = { m_pBackBufferRTV.Get() };
        m_pContext->OMSetRenderTargets(1, pBackBufferRTVs, nullptr);
        m_pContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), reinterpret_cast<float*>(&clearColor));




        const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_FullscreenQuad");
        const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_FullscreenQuad");
        const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResVIBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "VIBuffer_FullscreenTex");

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

        ID3D11ShaderResourceView* pSRVs[1] = { m_pOffScreenTex2D->GetSRV().Get() };
        m_pContext->PSSetShaderResources(0, 1, pSRVs);

        const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_LINEAR_WRAP);
        m_pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
        
        m_pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);

        ID3D11ShaderResourceView* pNullSRVs[1] = { nullptr };
        m_pContext->PSSetShaderResources(0, 1, pNullSRVs);
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
