#pragma once
#include "Engine_Defines.h"
#include "IRenderable.h"
NS_BEGIN(Engine)
class CGameObject;
class CResOffscreenTexture;
class CResDynamicTexture2D;

class CRenderer final : public CEngineBase
{
	static const int CubeMapSize = 256;

private:
	CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CRenderer() override;

public:
	HRESULT Initialize();
	HRESULT InitializeOffscreen();
	HRESULT InitializeShadow();
	HRESULT InitializeFullscreen();
	HRESULT AddRenderObject(RENDERGROUP eRenderGroup, IRenderable* pRenderObject);
	HRESULT Draw();
	void FrameEnd();

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};
	std::array<std::vector<IRenderable*>, ETOUI(RENDERGROUP::END)> m_RenderObject{};

private:
	SPtr<CResDynamicTexture2D> m_pOffScreenTex2D{};

private:
	SPtr<CResDynamicTexture2D> m_pShadowTex2D{};
	SPtr<CResViewPort> m_pShadowVP{};

private:
	ComPtr<ID3D11RenderTargetView> m_pBackBufferRTV{};
	ComPtr<ID3D11DepthStencilView> m_pBackBufferDSV{};
	SPtr<CResViewPort> m_pBackBufferVP{};

private:
	SPtr<CResVertexShader> m_pFullscreenVS{};
	SPtr<CResPixelShader> m_pFullscreenPS{};
	SPtr<CResVIBuffer> m_pFullscreenVIBuffer{};

private:

private:
	//_float4 m_SSAOOffsets[14]{};

private:
	HRESULT DrawFullscreen();

private:
	HRESULT RenderPriority(const RENDER_CTX& ctx);
	HRESULT RenderNonBlend(const RENDER_CTX& ctx);
	HRESULT RenderBlend(const RENDER_CTX& ctx);
	HRESULT RenderSkybox(const RENDER_CTX& ctx);
	HRESULT RenderCollider(const RENDER_CTX& ctx);
	HRESULT RenderUI(const RENDER_CTX& ctx);
	HRESULT RenderUIToolTip(const RENDER_CTX& ctx);

public:
	static UPtr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END