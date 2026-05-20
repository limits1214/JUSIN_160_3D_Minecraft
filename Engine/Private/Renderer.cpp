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
    RECT rect;
    GetClientRect(CGameInstance::Get().GetHwnd(), &rect);


    auto dsv = CGameInstance::Get().GetResourceFirst<CResDynamicTexture2D>(TAG_RES_GRP_PERMANENT_TEXTURE, "DYNTEX_DSV_BackBuffer");
    if (!dsv)
    {
        return E_FAIL;
    }
    m_pBackBufferDSV = dsv;

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


    ctx.pass = RENDERPASS::DEFAULT;
    auto pDepthStencilView = m_pBackBufferDSV->GetDSV().Get();
    ID3D11RenderTargetView* rt[1] = { CGameInstance::Get().GetBackBufferRTV().Get() };
    m_pContext->OMSetRenderTargets(1, rt, pDepthStencilView);
    _float4 clearColor = { 0.f, 0.f, 1.f, 1.f };
    m_pContext->ClearRenderTargetView(rt[0], reinterpret_cast<const float*>(&clearColor));
    m_pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


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
                cbPerFrame.vCamPos = pUICame->GetTransform().GetPosition();

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



    
	E::CGameInstance::Get().FontDraw("NeoDGM_20px", L"흙이 운다 흙흑", {});

    // TODO: 제거
    {
        m_pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

        // 2. 깊이/스텐실 스테이트 복구 (3D 렌더링을 위해 다시 켜기)
        m_pContext->OMSetDepthStencilState(nullptr, 0); // 엔진 내 기본 DepthStencilState가 있다면 nullptr 대신 그걸 대입

        // 3. 래스터라이저 스테이트 복구 (CullMode 등을 다시 원래대로)
        m_pContext->RSSetState(nullptr); // 엔진 내 기본 RasterizerState가 있다면 대입
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
