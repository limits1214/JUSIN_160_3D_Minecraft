#include "Renderer.h"

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
    return E_NOTIMPL;
}

HRESULT CRenderer::AddRenderObject(RENDERGROUP eRenderGroup, IRenderable* pRenderObject)
{
    return E_NOTIMPL;
}

HRESULT CRenderer::Draw()
{
    return E_NOTIMPL;
}

HRESULT CRenderer::DrawFullscreen()
{
    return E_NOTIMPL;
}

HRESULT CRenderer::RenderPriority(const RENDER_CTX& ctx)
{
    return E_NOTIMPL;
}

HRESULT CRenderer::RenderNonBlend(const RENDER_CTX& ctx)
{
    return E_NOTIMPL;
}

HRESULT CRenderer::RenderBlend(const RENDER_CTX& ctx)
{
    return E_NOTIMPL;
}

HRESULT CRenderer::RenderCollider(const RENDER_CTX& ctx)
{
    return E_NOTIMPL;
}

HRESULT CRenderer::RenderUI(const RENDER_CTX& ctx)
{
    return E_NOTIMPL;
}

UPtr<CRenderer> CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return UPtr<CRenderer>();
}
