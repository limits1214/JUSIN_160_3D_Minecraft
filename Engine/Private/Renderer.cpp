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
