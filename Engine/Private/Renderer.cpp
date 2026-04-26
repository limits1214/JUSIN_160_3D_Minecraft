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


    auto dsv = CGameInstance::Get().GetResourceFirst<CResDynamicTexture2D>("PERMANENT_DEVICE", "DSV");
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
    auto pDepthStencilView = m_pBackBufferDSV->GetDSV().Get();
    ID3D11RenderTargetView* rt[1] = { CGameInstance::Get().GetBackBufferRTV().Get() };
    m_pContext->OMSetRenderTargets(1, rt, pDepthStencilView);
    _float4 clearColor = { 0.f, 0.f, 1.f, 1.f };
    m_pContext->ClearRenderTargetView(rt[0], reinterpret_cast<const float*>(&clearColor));
    m_pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    RENDER_CTX ctx{};

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

    if (FAILED(RenderUI(ctx)))
    {
        return E_FAIL;
    }

    return S_OK;
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
