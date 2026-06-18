#include "pch.h"
#include "GameInstance.h"
#include "SoundManager.h"
#include "DInputManager.h"
#include "GraphicDevice.h"
#include "LevelManager.h"
#include "Level.h"
#include "ImguiManager.h"
#include "TimeProvider.h"
#include "PrototypeManager.h"
#include "LightManager.h"
//#include "VoxelManager.h"
//#include "VoxelManager2.h"
#include "VoxelManager3.h"
#include "ParticleManager.h"
#include "FontManager.h"
#include "WorldManager.h"

#include "GameObject.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Collider.h"

#include "Renderer.h"

#include "FlyCamera.h"
#include "UICamera.h"
#include "PlayerCamera.h"
#include "ShadowCamera.h"

#include "Resources.h"

#include "ComEntityModel.h"
#include "ComConstantBuffer.h"
#include "ComAnimator.h"


NS_USING(Engine)

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
}

HRESULT CGameInstance::InitializeEngine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& ppDevice, ComPtr<ID3D11DeviceContext>& ppContext)
{
	m_hWnd = EngineDesc.hWnd;
	m_vClientScreenSize.x = (float)EngineDesc.iWinSizeX;
	m_vClientScreenSize.y = (float)EngineDesc.iWinSizeY;
	

	m_pGraphicDevice = CGraphicDevice::Create(ppDevice, ppContext);
	if (m_pGraphicDevice == nullptr)
	{
		return E_FAIL;
	}

	m_pResourceManager = CResourceManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pResourceManager == nullptr)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGraphicDevice->ReadyDevice(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY)))
	{
		return E_FAIL;
	}


	if (FAILED(InitializeResources()))
	{
		return E_FAIL;
	}
	if (FAILED(InitializeMCResource()))
	{
		return E_FAIL;
	}

	m_pPrototypeManager = CPrototypeManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pPrototypeManager == nullptr)
	{
		return E_FAIL;
	}

	if (FAILED(InitializePrototype()))
	{
		return E_FAIL;
	}

	m_pImguiManager = CImguiManager::Create(EngineDesc.hWnd, ppDevice.Get(), ppContext.Get());
	if (m_pImguiManager == nullptr)
	{
		return E_FAIL;
	}

	m_pLevelManager = CLevelManager::Create();
	if (m_pLevelManager == nullptr)
	{
		return E_FAIL;
	}

	m_pDInputManager = CDInputManager::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (m_pDInputManager == nullptr)
	{
		return E_FAIL;
	}

	m_pTimeProvider = CTimeProvider::Create();
	if (m_pTimeProvider == nullptr)
	{
		return E_FAIL;
	}

	m_pWorkerManager = CWorkerManager::Create("Normal",3);
	if (m_pWorkerManager == nullptr)
	{
		return E_FAIL;
	}

	m_pChunkLoadWorkerManager = CWorkerManager::Create("ChunkLoader", std::thread::hardware_concurrency() - 3);
	if (m_pChunkLoadWorkerManager == nullptr)
	{
		return E_FAIL;
	}

	m_pGameObjectManager = CGameObjectManager::Create();
	if (m_pGameObjectManager == nullptr)
	{
		return E_FAIL;
	}

	m_pRenderer = CRenderer::Create(ppDevice.Get(), ppContext.Get());
	if (m_pRenderer == nullptr)
	{
		return E_FAIL;
	}

	m_pCameraManager = CCameraManager::Create();
	if (m_pCameraManager == nullptr)
	{
		return E_FAIL;
	}

	m_pColliderManager = CColliderManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pColliderManager == nullptr)
	{
		return E_FAIL;
	}

	m_pLightManager = CLightManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pLightManager == nullptr)
	{
		return E_FAIL;
	}

	//m_pVoxelManager = CVoxelManager::Create(ppDevice.Get(), ppContext.Get());
	//if (m_pVoxelManager == nullptr)
	//{
	//	return E_FAIL;
	//}

	//m_pVoxelManager2 = CVoxelManager2::Create(ppDevice.Get(), ppContext.Get());
	//if (m_pVoxelManager2 == nullptr)
	//{
	//	return E_FAIL;
	//}
	m_pVoxelManager3 = CVoxelManager3::Create(ppDevice.Get(), ppContext.Get());
	if (m_pVoxelManager3 == nullptr)
	{
		return E_FAIL;
	}

	m_pParticleManager = CParticleManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pParticleManager == nullptr)
	{
		return E_FAIL;
	}

	m_pFontManager = CFontManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pFontManager == nullptr)
	{
		return E_FAIL;
	}

	m_pWorldManager = CWorldManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pWorldManager == nullptr)
	{
		return E_FAIL;
	}



	return S_OK;
}

void CGameInstance::UpdateEngine(_float fTimeDelta)
{
	m_pDInputManager->Update_InputDev();

	if (CGameInstance::Get().KeyDown(DIK_TAB))
	{
		
		m_bMouseFix = !m_bMouseFix;
		if (!m_bMouseFix)
		{
			ShowCursor(TRUE);
		}
		else
		{
			ShowCursor(FALSE);
		}
	}
	if (m_bMouseFix)
	{
		MouseFix();
	}

	m_pVoxelManager3->Update(fTimeDelta);

	m_pParticleManager->Update(fTimeDelta);

	m_pWorldManager->Update(fTimeDelta);

	m_pGameObjectManager->PriorityUpdate(fTimeDelta);
	m_pGameObjectManager->Update(fTimeDelta);
	m_pGameObjectManager->LateUpdate(fTimeDelta);

	m_pLevelManager->Update(fTimeDelta);

	AddRenderObject(RENDERGROUP::NONBLEND, m_pVoxelManager3.get());
	AddRenderObject(RENDERGROUP::COLLIDER, m_pColliderManager.get());
	AddRenderObject(RENDERGROUP::NONBLEND, m_pParticleManager.get());
}

HRESULT CGameInstance::Draw()
{
	if (FAILED(m_pRenderer->Draw()))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pLevelManager->Render()))
	//{
	//	return E_FAIL;
	//}
	return S_OK;
}

void CGameInstance::UpdateGUI()
{
	m_pPrototypeManager->UpdateGUI();

	m_pGameObjectManager->UpdateGUI();

	m_pWorkerManager->UpdateGUI();
	m_pChunkLoadWorkerManager->UpdateGUI();

	m_pResourceManager->UpdateGUI();

	m_pWorldManager->UpdateGUI();

	m_pCameraManager->UpdateGUI();

	m_pLevelManager->UpdateGUI();

	m_pColliderManager->UpdateGUI();

	m_pParticleManager->UpdateGUI();

	m_pLightManager->UpdateGUI();

	m_pVoxelManager3->UpdateGUI();

	if (ImGui::Button("ShaderRebuild"))
	{
		//TAG_RES_GRP_PERMANENT_SHADER
		if (auto resources = GetResource(TAG_RES_GRP_PERMANENT_SHADER))
		{
			for (auto& [_, res] : *resources)
			{
				if (!res.empty())
				{
					res.front()->Unload();
					res.front()->Load();
				}
			}
		}
	}
}

void CGameInstance::ClearResource(uint32_t iClearLevelIndex)
{
}

void CGameInstance::FrameStart(_float fTimeDelta)
{
	m_pLevelManager->FrameStart(fTimeDelta);
	m_pGameObjectManager->FrameStart();
	m_pColliderManager->FrameStart();
}

void CGameInstance::FrameEnd(_float fTimeDelta)
{
	m_pGameObjectManager->FrameEnd();
	m_pLevelManager->FrameEnd(fTimeDelta);

	m_pRenderer->FrameEnd();
	m_pColliderManager->FrameEnd();
}

void CGameInstance::Release_Engine()
{
	m_pSoundManager.reset();
	m_pImguiManager.reset();
	m_pDInputManager.reset();
	m_pWorldManager.reset();
	m_pGameObjectManager->AllReset();
	m_pGameObjectManager.reset();
	m_pLevelManager.reset();
	m_pColliderManager.reset();
	m_pParticleManager.reset();
	m_pWorkerManager.reset();
	m_pChunkLoadWorkerManager.reset();
	m_pPrototypeManager.reset();
	m_pLightManager.reset();
	//m_pVoxelManager.reset();
	m_pVoxelManager3.reset();
	m_pRenderer.reset();
	m_pFontManager.reset();
	m_pResourceManager.reset();

	m_pGraphicDevice.reset();
}

HRESULT CGameInstance::InitializeResources()
{
	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_FRAME, E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_FRAME) })))
		{
			return E_FAIL;
		}
	}
	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT, E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_OBJECT) })))
		{
			return E_FAIL;
		}
	}
	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_MATERIAL, E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_MATERIAL) })))
		{
			return E_FAIL;
		}
	}
	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBone", E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_BONE) })))
		{
			return E_FAIL;
		}
	}
	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerQuadItemAnim", E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_QUADITEM_ANIM) })))
		{
			return E_FAIL;
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(GetGraphicDeviceContext()->Map(res->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
			E::CB_PER_QUADITEM_ANIM cbPerQuadItemAnim{};
			memcpy(mappedSubResource.pData, &cbPerQuadItemAnim, sizeof(cbPerQuadItemAnim));
			GetGraphicDeviceContext()->Unmap(res->GetCBuffer().Get(), 0);
		}
		GetGraphicDeviceContext()->VSSetConstantBuffers(5, 1, res->GetCBuffer().GetAddressOf());
	}

	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerBlockOutline", E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_BLOCKOUTLINE) })))
		{
			return E_FAIL;
		}
	}

	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerDestroyStage", E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_DESTROYSTAGE) })))
		{
			return E_FAIL;
		}
	}

	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerUI", E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_UI) })))
		{
			return E_FAIL;
		}
	}

	//
	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerVoxelWater", E::CResCBuffer::Create()))
	{
		if (FAILED(res->Load(E::CResCBuffer::CBUFFER_DESC{ .byteWidth = sizeof(CB_PER_VOXEL_WATER) })))
		{
			return E_FAIL;
		}
	}
	
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_LINEAR_WRAP, CResSamplerState::Create()))
	{
		res->Load(D3D11_SAMPLER_DESC{
			.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
			.ComparisonFunc = D3D11_COMPARISON_NEVER,
			.MinLOD = 0,
			.MaxLOD = D3D11_FLOAT32_MAX,
			});
	}
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP, CResSamplerState::Create()))
	{
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		samplerDesc.MinLOD = 0.f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		res->Load(samplerDesc);
	}
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP_NOMIP, CResSamplerState::Create()))
	{
		res->Load(D3D11_SAMPLER_DESC{
			.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
			.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
			.ComparisonFunc = D3D11_COMPARISON_NEVER,
			.MinLOD = 0,
			.MaxLOD = 0.0f,
			});
	}
	if (auto res = AddResourceT(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_SAHDOW, CResSamplerState::Create()))
	{
		D3D11_SAMPLER_DESC sampDesc{};
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.BorderColor[0] = 1.f;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;

		//sampDesc.MinLOD = -D3D11_FLOAT32_MAX;
		//sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		//sampDesc.MipLODBias = 0.f;
		//sampDesc.MaxAnisotropy = 1;
		if (FAILED(res->Load(sampDesc)))
		{
			return E_FAIL;
		}

		GetGraphicDeviceContext()->PSSetSamplers(4, 1, res->GetSamplerState().GetAddressOf());
	}
		
	if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_QuadTex", "./Resources/Shader/QuadTex/QuadTex.hlsl"))
	{
		res->Load();
	}
	if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_QuadTex", "./Resources/Shader/QuadTex/QuadTex.hlsl"))
	{
		res->Load();
	}
	if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_QuadCol", "./Resources/Shader/QuadCol/QuadCol.hlsl"))
	{
		res->Load();
	}
	if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_QuadCol", "./Resources/Shader/QuadCol/QuadCol.hlsl"))
	{
		res->Load();
	}
	


	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_BUFFER, "VIBuffer_QuadTex", E::CResQuadTexBuffer::Create()))
	{
		res->Load();
	}

	
	

	//
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_BACKCULL, E::CResRasterizerState::Create()))
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthClipEnable = TRUE;
		res->Load(desc);
	}
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_FRONTCULL, E::CResRasterizerState::Create()))
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.DepthClipEnable = TRUE;
		res->Load(desc);
	}
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL, E::CResRasterizerState::Create()))
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthClipEnable = TRUE;
		desc.DepthBias = 10;
		desc.SlopeScaledDepthBias = 0.5f;
		desc.DepthBiasClamp = 0.0f;
		res->Load(desc);
	}
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_WIREFRAME_NOCULL, E::CResRasterizerState::Create()))
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthClipEnable = TRUE;
		
		res->Load(desc);
	}
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, "RS_SOLID_BACKCULL_DEPTHBIAS", E::CResRasterizerState::Create()))
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthClipEnable = TRUE;
		desc.DepthBias = -100;
		desc.SlopeScaledDepthBias = -1.0f;
		desc.DepthBiasClamp = 0.0f;
		res->Load(desc);
	}

	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, "RS_SOLID_BLOCK_VOXEL_SHADOW", E::CResRasterizerState::Create()))
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.DepthClipEnable = TRUE;
		res->Load(desc);
	}
	
	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, "RS_ALPHATEST_BLOCK_VOXEL_SHADOW", E::CResRasterizerState::Create()))
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthClipEnable = TRUE;
		desc.DepthBias = 10;
		desc.SlopeScaledDepthBias = 0.5f;
		desc.DepthBiasClamp = 0.0f;
		res->Load(desc);
	}


	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, "BS_ALPHA_BLEND", E::CResBlendState::Create()))
	{
		D3D11_BLEND_DESC blendDesc{};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		res->Load(blendDesc);
	}

	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, "DS_NO_DEPTHWRITE", E::CResDepthStencilState::Create()))
	{
		D3D11_DEPTH_STENCIL_DESC depthDesc{};
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 무력화 (기록 안함)
		depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		res->Load(depthDesc);
	}

	if (auto res = AddResource(TAG_RES_GRP_PERMANENT_STATE, "DS_Skybox", E::CResDepthStencilState::Create()))
	{
		D3D11_DEPTH_STENCIL_DESC depthDesc{};
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 무력화 (기록 안함)
		depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		res->Load(depthDesc);
	}




	// offscreenTextureVIBuffer

	return S_OK;
}

HRESULT CGameInstance::InitializeMCResource()
{
	// initialize block, watershader(voxel)
	{
		if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Block", "./Resources/Shader/Block/Block.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Block", "./Resources/Shader/Block/Block.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_BlockAlphaTest", "./Resources/Shader/Block/BlockAlphaTest.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_BlockAlphaTest", "./Resources/Shader/Block/BlockAlphaTest.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Water", "./Resources/Shader/Block/Water.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Water", "./Resources/Shader/Block/Water.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}




		if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Shadow_Block", "./Resources/Shader/Block/Shadow_Block.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Shadow_Block", "./Resources/Shader/Block/Shadow_Block.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Shadow_BlockAlphaTest", "./Resources/Shader/Block/Shadow_BlockAlphaTest.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Shadow_BlockAlphaTest", "./Resources/Shader/Block/Shadow_BlockAlphaTest.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
	}
		// initialize entity shaders
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Entity", "./Resources/Shader/Entity/Entity.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Entity", "./Resources/Shader/Entity/Entity.hlsl"))
		{
			res->Load();
		}
	}

	// initialize item shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DropItem", "./Resources/Shader/Item/DropItem.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DropItem", "./Resources/Shader/Item/DropItem.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DropBlock", "./Resources/Shader/Item/DropBlock.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DropBlock", "./Resources/Shader/Item/DropBlock.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Shadow_DropItem", "./Resources/Shader/Item/Shadow_DropItem.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Shadow_DropItem", "./Resources/Shader/Item/Shadow_DropItem.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Shadow_DropBlock", "./Resources/Shader/Item/Shadow_DropBlock.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Shadow_DropBlock", "./Resources/Shader/Item/Shadow_DropBlock.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_HandHeldItem", "./Resources/Shader/Item/HandHeldItem.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_HandHeldItem", "./Resources/Shader/Item/HandHeldItem.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}

		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_HandHeldBlock", "./Resources/Shader/Item/HandHeldBlock.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_HandHeldBlock", "./Resources/Shader/Item/HandHeldBlock.hlsl"))
		{
			if (FAILED(res->Load()))
			{
				return E_FAIL;
			}
		}
	}

	// cross hair shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Crosshair", "./Resources/Shader/Crosshair/CrossHair.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Crosshair", "./Resources/Shader/Crosshair/CrossHair.hlsl"))
		{
			res->Load();
		}
	}

	// blockoutline shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_BlockOutline", "./Resources/Shader/BlockOutline/BlockOutline.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_BlockOutline", "./Resources/Shader/BlockOutline/BlockOutline.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResGeometryShader>(TAG_RES_GRP_PERMANENT_SHADER, "GS_BlockOutline", "./Resources/Shader/BlockOutline/BlockOutline.hlsl"))
		{
			res->Load();
		}
	}

	// destroystage shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DestroyStage", "./Resources/Shader/DestroyStage/DestroyStage.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DestroyStage", "./Resources/Shader/DestroyStage/DestroyStage.hlsl"))
		{
			res->Load();
		}
	}

	// ui shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_UI", "./Resources/Shader/UI/UI.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_UI", "./Resources/Shader/UI/UI.hlsl"))
		{
			res->Load();
		}
	}

	// falling voxel Shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_FallingVoxel", "./Resources/Shader/FallingVoxel/FallingVoxel.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_FallingVoxel", "./Resources/Shader/FallingVoxel/FallingVoxel.hlsl"))
		{
			res->Load();
		}
	}

	// initialize skybox Shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Skybox", "./Resources/Shader/Skybox/Skybox.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Skybox", "./Resources/Shader/Skybox/Skybox.hlsl"))
		{
			res->Load();
		}
	}

	// initialize cloud shader
	{
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Cloud", "./Resources/Shader/Cloud/Cloud.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Cloud", "./Resources/Shader/Cloud/Cloud.hlsl"))
		{
			res->Load();
		}
	}


	// initialize destroy stage texture
	{
		{
			for (uint32_t i = 0; i < 10; ++i)
			{
				auto pTexture = CResTexture2D::Create("./Resources/Texture/DestroyStage/destroy_stage_" + std::to_string(i) + ".png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("DESTROY_STAGE", "TEXTURES", pTexture);
			}
		}

		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "DESTROY_STAGE", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("DESTROY_STAGE", "TEXTURE_ARRAY", pTextureArray);


			GetGraphicDeviceContext()->PSSetShaderResources(11, 1, pTextureArray->GetSRV().GetAddressOf());
		}
	}

	// initialie 256 256 tex
	{
		{
			{
				//0
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/icons.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
			{
				//1
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/gui2.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
			{
				//2
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/inventory.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
			{
				//3
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/crafting_table.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
			{
				//4
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/blast_furnace.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
			{
				//5
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/enchanting_table.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
			{
				//6
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/chest.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
			{
				//7
				auto pTexture = CResTexture2D::Create("./Resources/Texture/UI/chest2.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURES", pTexture);
			}
		}

		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "MC_TEX_256_256", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("MC_TEX_256_256", "TEXTURE_ARRAY", pTextureArray);
			GetGraphicDeviceContext()->PSSetShaderResources(12, 1, pTextureArray->GetSRV().GetAddressOf());
		}
	}

	// initialize voxel texture
	{
		{
			auto VoxelManagerTexAdd = [&](const _string& path)
				{
					auto pTexture = CResTexture2D::Create(path);
					if (FAILED(pTexture->Load()))
					{
						return E_FAIL;
					}
					CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
					return S_OK;
				};

			{
				//0: dirt
				VoxelManagerTexAdd("./Resources/Texture/Blocks/dirt.png");
			}

			{
				//1: stone
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/stone.png");
			}

			{
				//2: sand
				VoxelManagerTexAdd("./Resources/Texture/Blocks/sand.png");
			}

			{
				//3: bedrock
				VoxelManagerTexAdd("./Resources/Texture/Blocks/bedrock.png");
			}

			{
				//4: redstone_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/redstone_ore.png");
			}

			{
				//5: lapis_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/lapis_ore.png");
			}

			{
				//6: iron_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/iron_ore.png");
			}

			{
				//7: gold_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/gold_ore.png");
			}

			{
				//8: emerald_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/emerald_ore.png");
			}

			{
				//9: diamond_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/diamond_ore.png");
			}

			{
				//10: copper_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/copper_ore.png");
			}

			{
				//11: coal_ore
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/coal_ore.png");
			}

			{
				//12: deepslate.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate.png");
			}

			{
				// 13: deepslate_coal_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_coal_ore.png");
			}

			{
				// 14: deepslate_copper_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_copper_ore.png");
			}

			{
				// 15: deepslate_diamond_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_diamond_ore.png");
			}

			{
				// 16: deepslate_emerald_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_emerald_ore.png");
			}

			{
				// 17: deepslate_gold_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_gold_ore.png");
			}

			{
				// 18: deepslate_iron_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_iron_ore.png");
			}

			{
				// 19: deepslate_lapis_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_lapis_ore.png");
			}

			{
				// 20: deepslate_redstone_ore.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/deepslate_redstone_ore.png");
			}

			{
				// 21 : grass_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/grass_carried.png");
			}

			{
				// 22 : grass_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/grass_side_carried.png");
			}

			{
				// 23: grass_block_snow.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/grass_block_snow.png");
			}

			{
				// 24: water_placeholder.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/water/water_placeholder.png");
			}

			{
				// 25:torch_on.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/torch_on.png");
			}

			{
				// 26: redstone_torch_off.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/redstone_torch_off.png");
			}

			{
				// 27: redstone_torch_on.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/redstone_torch_on.png");
			}

			{
				// 28: cherry_log_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/cherry_log_top.png");
			}

			{
				// 29: cherry_log_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/cherry_log_side.png");
			}

			{
				// 30: log_oak_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/log_oak_top.png");
			}

			{
				// 31: log_oak.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/log_oak.png");
			}

			{
				// 32: log_birch_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/log_birch_top.png");
			}

			{
				// 33: log_birch.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/log_birch.png");
			}

			{
				// 34: log_acacia_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/log_acacia_top.png");
			}

			{
				// 35: log_acacia.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/log/log_acacia.png");
			}

			{
				// 36: cherry_planks.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/plank/cherry_planks.png");
			}

			{
				// 37: planks_oak.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/plank/planks_oak.png");
			}

			{
				// 38: planks_birch.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/plank/planks_birch.png");
			}

			{
				// 39: planks_acacia.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/plank/planks_acacia.png");
			}

			{
				// 40: cherry_leaves.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/leaves/cherry_leaves.png");
			}

			{
				// 41: leaves_oak.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/leaves/leaves_oak.png");
			}

			{
				// 42: leaves_birch.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/leaves/leaves_birch.png");
			}

			{
				// 43: leaves_acacia.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/leaves/leaves_acacia.png");
			}

			{
				// 44: flower_allium.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_allium.png");
			}

			{
				// 45: flower_blue_orchid.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_blue_orchid.png");
			}

			{
				// 46: flower_cornflower.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_cornflower.png");
			}

			{
				// 47: flower_dandelion.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_dandelion.png");
			}

			{
				// 48: flower_houstonia.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_houstonia.png");
			}

			{
				// 49: flower_lily_of_the_valley.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_lily_of_the_valley.png");
			}

			{
				// 50: flower_oxeye_daisy.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_oxeye_daisy.png");
			}

			{
				// 51: flower_paeonia.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_paeonia.png");
			}

			{
				// 52: flower_rose.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_rose.png");
			}
			
			{
				// 53: flower_rose_blue.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_rose_blue.png");
			}

			{
				// 54: flower_tulip_orange.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_tulip_orange.png");
			}

			{
				// 55: flower_tulip_pink.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_tulip_pink.png");
			}

			{
				// 56: flower_tulip_red.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_tulip_red.png");
			}

			{
				// 57:flower_wither_rose.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/flower/flower_wither_rose.png");
			}

			{
				// 58:short_dry_grass.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/fiji/short_dry_grass.png");
			}

			{
				// 59:short_grass.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/fiji/short_grass.png");
			}

			{
				// 60:tall_dry_grass.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/fiji/tall_dry_grass.png");
			}
			{
				// 61:tall_grass_bottom.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/fiji/tall_grass_bottom.png");
			}
			{
				// 62:tall_grass_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/fiji/tall_grass_top.png");
			}
			
			{
				// 63: stripped_cherry_log_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_cherry_log_top.png");
			}
			{
				// 64: stripped_cherry_log_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_cherry_log_side.png");
			}
			{
				// 65: stripped_oak_log_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_oak_log_top.png");
			}
			{
				// 66: stripped_oak_log.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_oak_log.png");
			}
			{
				// 67: stripped_birch_log_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_birch_log_top.png");
			}
			{
				// 68: stripped_birch_log.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_birch_log.png");
			}
			{
				// 69: stripped_acacia_log_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_acacia_log_top.png");
			}
			{
				// 70: stripped_acacia_log.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stripped_log/stripped_acacia_log.png");
			}

			{
				// 71: obsidian.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/obsidian.png");
			}

			{
				// 72: chest_front.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/chest/chest_front.png");
			}

			{
				// 73: chest_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/chest/chest_side.png");
			}

			{
				// 74: chest_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/chest/chest_top.png");
			}

			{
				// 75: crafting_table_front.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/crafting_table/crafting_table_front.png");
			}
			{
				// 76: crafting_table_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/crafting_table/crafting_table_side.png");
			}
			{
				// 77: crafting_table_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/crafting_table/crafting_table_top.png");
			}

			{
				// 78: furnace_front_off.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/furnace/furnace_front_off.png");
			}

			{
				// 79: furnace_front_on.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/furnace/furnace_front_on.png");
			}

			{
				// 80: furnace_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/furnace/furnace_side.png");
			}

			{
				// 81: furnace_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/furnace/furnace_top.png");
			}

			{
				// 82: tnt_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/tnt/tnt_top.png");
			}
			{
				// 83: tnt_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/tnt/tnt_side.png");
			}
			{
				// 84: tnt_bottom.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/tnt/tnt_bottom.png");
			}

			{
				// 85: enchanting_table_top.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/enchant_table/enchanting_table_top.png");
			}
			{
				// 86: enchanting_table_side.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/enchant_table/enchanting_table_side.png");
			}
			{
				// 87: enchanting_table_bottom.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/enchant_table/enchanting_table_bottom.png");
			}
			{
				// 88: bookshelf.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/bookshelf.png");
			}
			{
				// 89: ladder.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/ladder.png");
			}
			{
				// 90: cobblestone.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/stone/cobblestone.png");
			}
			{
				// 91: cobbled_deepslate.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/deepslate/cobbled_deepslate.png");
			}
			{
				// 92: lava_placeholder.png
				VoxelManagerTexAdd("./Resources/Texture/Blocks/lava/lava_placeholder.png");
			}

		}

		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "VOXEL_MANAGER_TEX", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURE_ARRAY", pTextureArray);
			GetGraphicDeviceContext()->PSSetShaderResources(9, 1, pTextureArray->GetSRV().GetAddressOf());
		}


		{
			//"VOXEL_MANAGER_TEX", "TEXTURES"
			CGameInstance::Get().DelResource("VOXEL_MANAGER_TEX", "TEXTURES");
		}

		// water_still
		{
			auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/water/water_still.png");
			if (FAILED(pTexture->Load()))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "WATER_STILL", pTexture);
			GetGraphicDeviceContext()->PSSetShaderResources(13, 1, pTexture->GetSRV().GetAddressOf());
		}

		// water_flow
		{
			auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/water/water_flow.png");
			if (FAILED(pTexture->Load()))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "WATER_FLOW", pTexture);
			GetGraphicDeviceContext()->PSSetShaderResources(14, 1, pTexture->GetSRV().GetAddressOf());
		}

		// lava_still
		{
			auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/lava/lava_still.png");
			if (FAILED(pTexture->Load()))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "LAVA_STILL", pTexture);
			GetGraphicDeviceContext()->PSSetShaderResources(15, 1, pTexture->GetSRV().GetAddressOf());
		}

		// lava_flow
		{
			auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/lava/lava_flow.png");
			if (FAILED(pTexture->Load()))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "LAVA_FLOW", pTexture);
			GetGraphicDeviceContext()->PSSetShaderResources(16, 1, pTexture->GetSRV().GetAddressOf());
		}

	}

	{
		auto CubeItem300_300TexAdd = [&](const _string& path)
			{
				auto pTexture = CResTexture2D::Create(path);
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("MC_TEX_300_300", "TEXTURES", pTexture);
				return S_OK;
			};

		{
			{
				// 0
				CubeItem300_300TexAdd("./Resources/Texture/Item/Cube/Dirt.png");
			}

			{
				// 1
				CubeItem300_300TexAdd("./Resources/Texture/Item/Cube/Cobblestone.png");
			}

			{
				// 2
				CubeItem300_300TexAdd("./Resources/Texture/Item/Cube/Sand.png");
			}

			{
				// 3
				CubeItem300_300TexAdd("./Resources/Texture/Item/Cube/TNT.png");
			}
		}



		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "MC_TEX_300_300", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("MC_TEX_300_300", "TEXTURE_ARRAY", pTextureArray);
			GetGraphicDeviceContext()->PSSetShaderResources(17, 1, pTextureArray->GetSRV().GetAddressOf());
		}

		{
			CGameInstance::Get().DelResource("MC_TEX_300_300", "TEXTURES");
		}
	}

	// initialize cube item
	{
		if (auto res = AddResource("MC_ITEM_VIBuffer", "CubeItemDirt", CResCubeItemVIBuffer::Create()))
		{
			//uint32_t tmp[ETOUI(FACE_DIR::END)]{PackTexId(9, 0),PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) };

			CResCubeItemVIBuffer::DESC desc{};
			//desc.textureId = { "VOXEL_MANAGER_TEX", "TEXTURES" };
			//desc.resourceIdx = 0;
			//memcpy(desc.texIndices, tmp, sizeof(tmp));

			res->Load(desc);
		}
	}

	{
		if (auto res = AddResource("MC_VIBuffer", "Cloud", CResCloudVIBuffer::Create()))
		{
			//uint32_t tmp[ETOUI(FACE_DIR::END)]{PackTexId(9, 0),PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) };

			CResCloudVIBuffer::DESC desc{};
			//desc.textureId = { "VOXEL_MANAGER_TEX", "TEXTURES" };
			//desc.resourceIdx = 0;
			//memcpy(desc.texIndices, tmp, sizeof(tmp));

			if (FAILED(res->Load(desc)))
			{
				return E_FAIL;
			}
			;
		}
	}

	// initialize entity textures
	{
		// 0: Pig
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_64", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Pig/pig_v3.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 1: Saddle
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_64", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Pig/saddle_v2.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 2: Cow
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_64", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Cow/cow_v2.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 3: Steve
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_64", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Steve/steve.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}


		// 4: experience_orb.png
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_64", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/experience_orb.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "ExperienceOrb", CResQuadItemVIBuffer::Create()))
				{
					res->Load(CResQuadItemVIBuffer::DESC{ .textureId = {"MC_TEX_64_64", "TEXTURES"}, .resourceIdx = 4, .texIndex = PackTexId(8, 4) });
				}
			}
		}

		// Entity_64_64_Ted2d_Array
		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "MC_TEX_64_64", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("MC_TEX_64_64", "TEXTURE_ARRAY", pTextureArray);
			GetGraphicDeviceContext()->PSSetShaderResources(8, 1, pTextureArray->GetSRV().GetAddressOf());
		}
	}

	{
		// 0: Chicken
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Chicken/chicken.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 1: Skeleton
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Skeleton/skeleton.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 2: Item Cloth 1
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/Armor/cloth_1.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}
		// 3: Item Cloth 2
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/Armor/cloth_2.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 4: Zombie
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Zombie/zombie.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 5: Creeper
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_64_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Entity/Creeper/creeper.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}



		// Entity_64_32_Ted2d_Array
		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "MC_TEX_64_32", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("MC_TEX_64_32", "TEXTURE_ARRAY", pTextureArray);
			GetGraphicDeviceContext()->PSSetShaderResources(7, 1, pTextureArray->GetSRV().GetAddressOf());
		}
	}

	{
		// 0: Sun
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_32_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Env/sun.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 1: Moon
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_32_32", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Env/Moon/full_moon.png")))
		{
			if (FAILED(pRes->Load()))
			{
				int x = 0;
			}
		}

		// 32_32_texArray
		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "MC_TEX_32_32", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("MC_TEX_32_32", "TEXTURE_ARRAY", pTextureArray);
			GetGraphicDeviceContext()->PSSetShaderResources(5, 1, pTextureArray->GetSRV().GetAddressOf());
		}
	}




	// initialize entity geometry
	{
		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "Cow", CResEnttGeoCow::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "Cow",CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "Cow"}});
				}
			}
		}

		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "Pig", CResEnttGeoPig::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "Pig", CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "Pig"} });
				}
			}
		}

		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "Chicken", CResEnttGeoChicken::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "Chicken", CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "Chicken"} });
				}
			}
		}

		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "Steve", CResEnttGeoSteve::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "Steve", CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "Steve"} , .baseTexId = PackTexId(8, 3)});
				}
			}
		}

		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "SteveArm", CResEnttGeoSteveArm::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "SteveArm", CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "SteveArm"} , .baseTexId = PackTexId(8, 3) });
				}
			}
		}


		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "Skeleton", CResEnttGeoSkeleton::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "Skeleton", CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "Skeleton"} });
				}
			}
		}

		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "PlayerArmor", CResEnttGeoPlayerArmor::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "PlayerArmor", CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "PlayerArmor"}, .baseTexId = PackTexId(7, 2), .specificCubeTexIds = {{"armor2", PackTexId(7, 3)}}});
				}
			}
		}

		if (auto pRes = CGameInstance::Get().AddResource("MC_ENTITY_GEOMETRY", "Zombie", CResEnttGeoZombie::Create()))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ENTITY_VIBuffer", "Zombie", CResEnttVIBuffer::Create()))
				{
					res->Load(CResEnttVIBuffer::DESC{ .geometryId = {"MC_ENTITY_GEOMETRY", "Zombie"}} );
				}
			}
		}
	}

	// initialize item texture
	{
		
		// 0: woodPickaxe
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/Pickaxe/wood_pickaxe.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "WoodPickaxe", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 0 });
				}
			}
		}

		// 1: string
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/string.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "String", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 1 });
				}
			}
		}

		// 2: porkchop_raw
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/porkchop_raw.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "PorkchopRaw", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 2 });
				}
			}
		}

		// 3: chicken_raw
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/chicken_raw.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "ChickenRaw", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 3 });
				}
			}
		}

		// 4: beef_raw.png
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/beef_raw.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "BeefRaw", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 4 });
				}
			}
		}

		// 5: mutton_raw.png
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/mutton_raw.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "MuttonRaw", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 5 });
				}
			}
		}

		// 6: copperHelmet
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/Helmet/copper_helmet.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "CopperHelmet", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 6  });
				}
			}
		}

		// 7: copper_pickaxe
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/Pickaxe/copper_pickaxe.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				//if (auto res = AddResource("MC_ITEM_VIBuffer", "CopperHelmet", CResExtrudedItemVIBuffer::Create()))
				//{
				//	res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 6 , .texIndex = PackTexId(6, 6) });
				//}
			}
		}

		// 8: coal
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/coal.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "Coal", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 8 });
				}
			}
		}

		// 9: stick.png
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/stick.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "Stick", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 9 });
				}
			}
		}

		// 10: torch_on.png
		if (auto pRes = CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURES", CResTexture2D::Create("./Resources/Texture/Item/torch_on.png")))
		{
			if (SUCCEEDED(pRes->Load()))
			{
				if (auto res = AddResource("MC_ITEM_VIBuffer", "Torch", CResExtrudedItemVIBuffer::Create()))
				{
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 10 });
				}
			}
		}



		// MC_TEX_ITEM_16_16  TEXTURE_ARRAY
		{
			CResTexture2DArray::DESC desc{};
			desc.textureId = { "MC_TEX_ITEM_16_16", "TEXTURES" };
			auto pTextureArray = CResTexture2DArray::Create();
			if (FAILED(pTextureArray->Load(desc)))
			{
				return E_FAIL;
			}
			CGameInstance::Get().AddResource("MC_TEX_ITEM_16_16", "TEXTURE_ARRAY", pTextureArray);
			GetGraphicDeviceContext()->PSSetShaderResources(6, 1, pTextureArray->GetSRV().GetAddressOf());
		}
	}


	return S_OK;
}


HRESULT CGameInstance::InitializePrototype()
{
	if (AddPrototype("PERMANENT", "Prototype_Component_Transform", CComTransform::Create()))
	{
		return E_FAIL;
	}

	if (AddPrototype("PERMANENT", "Prototype_Component_EntityModel", CComEntityModel::Create()))
	{
		return E_FAIL;
	}
	if (AddPrototype("PERMANENT", "Prototype_Component_Animator", CComAnimator::Create()))
	{
		return E_FAIL;
	}
	if (AddPrototype("PERMANENT", "Prototype_Component_ConstantBuffer", CComConstantBuffer::Create()))
	{
		return E_FAIL;
	}

	if (AddPrototype("CAMERAS", "Prototype_GameObject_FlyCamera", CFlyCamera::Create()))
	{
		return E_FAIL;
	}

	if (AddPrototype("CAMERAS", "Prototype_GameObject_UICamera", CUICamera::Create()))
	{
		return E_FAIL;
	}

	if (AddPrototype("CAMERAS", "Prototype_GameObject_PlayerCamera", CPlayerCamera::Create()))
	{
		return E_FAIL;
	}
	if (AddPrototype("CAMERAS", "Prototype_GameObject_ShadowCamera", CShadowCamera::Create()))
	{
		return E_FAIL;
	}
	return S_OK;
}




#pragma region RESOURCE_MANAGER
SPtr<CResource> CGameInstance::AddResource(const StringID& sGroupTag, const StringID& sResTag, _string_id eAssetType, const _string& sPath, void* pArg)
{
	return m_pResourceManager->AddResource(sGroupTag, sResTag, eAssetType, sPath, pArg);
}
SPtr<CResource> CGameInstance::AddResource(const StringID& sGroupTag, const StringID& sResTag, SPtr<CResource> pAsset)
{
	return m_pResourceManager->AddResource(sGroupTag, sResTag, pAsset);
}
const std::vector<SPtr<CResource>>* CGameInstance::GetResource(const StringID& sGroupTag, const StringID& sResTag) const
{
	return m_pResourceManager->GetResource(sGroupTag, sResTag);
}
const std::unordered_map<StringID, std::vector<SPtr<CResource>>>* CGameInstance::GetResource(const StringID& sGroupTag) const
{
	return m_pResourceManager->GetResource(sGroupTag);
}
HRESULT CGameInstance::LoadResource(const StringID& sGroupTag)
{
	return m_pResourceManager->LoadResource(sGroupTag);
}
HRESULT CGameInstance::LoadResource(const StringID& sGroupTag, const StringID& sResTag)
{
	return m_pResourceManager->LoadResource(sGroupTag, sResTag);
}
HRESULT CGameInstance::UnLoadResource(const StringID& sGroupTag)
{
	return m_pResourceManager->UnLoadResource(sGroupTag);
}
HRESULT CGameInstance::UnLoadResource(const StringID& sGroupTag, const StringID& sResTag)
{
	return m_pResourceManager->UnLoadResource(sGroupTag, sResTag);
}
void CGameInstance::DelResource(const StringID& sGroupTag)
{
	m_pResourceManager->DelResource(sGroupTag);
}
void CGameInstance::DelResource(const StringID& sGroupTag, const StringID& sResTag)
{
	m_pResourceManager->DelResource(sGroupTag, sResTag);
}
#pragma endregion

#pragma region SOUND_MANAGER
HRESULT CGameInstance::CreateSound(const _string& sPath, FMOD_SOUND** ppSound)
{
	return m_pSoundManager->CreateSound(sPath, ppSound);
}


#pragma region DINPUT_MANAGER
_bool CGameInstance::KeyPressing(_ubyte byKeyID) const
{
	return m_pDInputManager->KeyPressing(byKeyID);
}
_bool CGameInstance::KeyUp(_ubyte byKeyID) const
{
	return m_pDInputManager->KeyUp(byKeyID);
}
_bool CGameInstance::KeyDown(_ubyte byKeyID) const
{
	return m_pDInputManager->KeyDown(byKeyID);
}
int32_t CGameInstance::MouseMove(MOUSEMOVESTATE eMouseState) const
{
	return m_pDInputManager->MouseMove(eMouseState);
}
_bool CGameInstance::MousePressing(MOUSEKEYSTATE eMouseState) const
{
	return  m_pDInputManager->MousePressing(eMouseState);
}
_bool CGameInstance::MouseUp(MOUSEKEYSTATE eMouseState) const
{
	return  m_pDInputManager->MouseUp(eMouseState);
}
_bool CGameInstance::MouseDown(MOUSEKEYSTATE eMouseState) const
{
	return  m_pDInputManager->MouseDown(eMouseState);
}

#pragma endregion




#pragma region GRAPHIC_DEVICE
ComPtr<ID3D11Device> CGameInstance::GetGraphicDevice() const
{
	return m_pGraphicDevice->GetDevice();
}
ComPtr<ID3D11DeviceContext> CGameInstance::GetGraphicDeviceContext() const
{
	return  m_pGraphicDevice->GetContext();
}
ComPtr<ID3D11RenderTargetView> CGameInstance::GetBackBufferRTV() const
{
	return m_pGraphicDevice->GetBackBufferRTV();
}
ComPtr<ID3D11DepthStencilView> CGameInstance::GetBackBufferDSV() const
{
	return m_pGraphicDevice->GetBackBufferDSV();
}

HRESULT CGameInstance::ClearBackBufferView(const _float4* pClearColor)
{
	return m_pGraphicDevice->ClearBackBufferView(pClearColor);
}

HRESULT CGameInstance::ClearDepthStencilView()
{
	return m_pGraphicDevice->ClearDepthStencilView();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphicDevice->Present();
}
#pragma endregion


#pragma region LEVEL_MANAGER
HRESULT CGameInstance::ChangeLevel(UPtr<CLevel> pNewLevel)
{
	return m_pLevelManager->ChangeLevel(std::move(pNewLevel));
}
void CGameInstance::RegisterLevelChangeFunc(const _string& ID, _Func func)
{
	m_pLevelManager->RegisterLevelChangeFunc(ID, func);
}
#pragma endregion


#pragma region IMGUI_MANAGER
void CGameInstance::ImguiNewFrame()
{
	m_pImguiManager->Update_Imgui();
}

void CGameInstance::ImguiEndFrameAndRender()
{
	m_pImguiManager->Render_Imgui();
}

_bool CGameInstance::ImguiWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pImguiManager->WinProc(hWnd, message, wParam, lParam);
}

_bool CGameInstance::ImguiGetActive() const
{
	return m_pImguiManager->Get_Active();
}

void CGameInstance::ImguiSetActive(_bool bActive)
{
	m_pImguiManager->Set_Active(bActive);
}
#pragma endregion


#pragma region WORKER_MANAGER
void CGameInstance::WorkerEnqueue(_string_view svTaskName, _Func func)
{
	m_pWorkerManager->Enqueue(svTaskName, func);
}

void CGameInstance::ChunkLoadWorkerEnqueue(_string_view svTaskName, _Func func)
{
	m_pChunkLoadWorkerManager->Enqueue(svTaskName, func);
}

#pragma endregion


#pragma region TIME_PROVIDER
inline _float CGameInstance::UpdateTimeProvider()
{
	return m_pTimeProvider->UpdateTimeProvider();
}
#pragma endregion


#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::AddPrototype(const StringID& svGroupTag, const StringID& svPrototypetag, UPtr<CPrototype> pPrototype)
{
	return m_pPrototypeManager->AddPrototype(svGroupTag, svPrototypetag, std::move(pPrototype));
}
UPtr<CPrototype> CGameInstance::ClonePrototype(const StringID& svGroupTag, const StringID& svPrototypetag, void* pArg)
{
	return m_pPrototypeManager->ClonePrototype(svGroupTag, svPrototypetag, pArg);
}
void CGameInstance::DelPrototype(const StringID& sGroupTag)
{
	m_pPrototypeManager->DelPrototype(sGroupTag);
}
#pragma endregion


#pragma region OBJECT_MANAGER
void CGameInstance::GameObjectAllReset()
{
	m_pGameObjectManager->AllReset();
}
std::optional<CHandle> CGameInstance::AddGameObjectToLayer(const StringID& iPrototypeLevelIndex, const StringID& svPrototypeTag, std::string_view sLayerName, void* pArg)
{
	return m_pGameObjectManager->AddGameObjectToLayer(iPrototypeLevelIndex, svPrototypeTag, sLayerName, pArg);
}
inline CGameObject* CGameInstance::GetGameObjectByHandle(const CHandle& handle)
{
	return m_pGameObjectManager->GetGameObjectByHandle(handle);
}
const std::vector<CHandle>* CGameInstance::GetGameObjectLayer(std::string_view sLayerName) const
{
	return m_pGameObjectManager->GetLayer(sLayerName);
}
const std::vector<CHandle>* CGameInstance::GetGameObjectLayer(std::string_view sLayerName, const StringID& iPrototypeLevelIndex, const StringID& svPrototypeTag, void* pArg)
{
	return m_pGameObjectManager->GetLayer(sLayerName, iPrototypeLevelIndex, svPrototypeTag, pArg);
}
void CGameInstance::DelGameObjectLayer(std::string_view sLayerName)
{
	return m_pGameObjectManager->DelLayer(sLayerName);
}
//std::optional<CHandle> CGameInstance::GetFreeHandle() const
//{
//	return m_pGameObjectManager->GetFreeHandle();
//}
#pragma endregion


#pragma region CAMERA_MANAGER
//const CCameraObject* CGameInstance::GetCameraObject(const StringID& GroupID) const
//{
//	return m_pCameraManager->GetCameraObject(GroupID);
//}
//HRESULT CGameInstance::SetCameraObject(const StringID& GroupID, const CHandle& handle)
//{
//	return m_pCameraManager->SetCameraObject(GroupID, handle);
//}

CCameraObject* CGameInstance::GetActiveGameCamera() const
{
	return m_pCameraManager->GetActiveGameCamera();
}

HRESULT CGameInstance::SetActiveGameCamera(const StringID& CameraID)
{
	return m_pCameraManager->SetActiveGameCamera(CameraID);
}

CCameraObject* CGameInstance::GetActiveUICamera() const
{
	return m_pCameraManager->GetActiveUICamera();
}

HRESULT CGameInstance::SetActiveUICamera(const StringID& CameraID)
{
	return m_pCameraManager->SetActiveUICamera(CameraID);
}

CCameraObject* CGameInstance::GetActiveGameCamera(const StringID& CameraID) const
{
	return m_pCameraManager->GetActiveGameCamera(CameraID);
}

CCameraObject* CGameInstance::GetActiveUICamera(const StringID& CameraID) const
{
	return m_pCameraManager->GetActiveUICamera(CameraID);
}

CCameraObject* CGameInstance::GetGameCamera(const StringID& CameraID) const
{
	return m_pCameraManager->GetGameCamera(CameraID);
}

CCameraObject* CGameInstance::GetUICamera(const StringID& CameraID) const
{
	return m_pCameraManager->GetUICamera(CameraID);
}

HRESULT CGameInstance::RegistGameCamera(const StringID& CameraID, const CHandle& handle)
{
	return m_pCameraManager->RegistGameCamera(CameraID, handle);
}

HRESULT CGameInstance::RegistUICamera(const StringID& CameraID, const CHandle& handle)
{
	return m_pCameraManager->RegistUICamera(CameraID, handle);
}

#pragma endregion



#pragma region COLLIDER_MANAGER
void CGameInstance::AddColliderGroup(const StringID& groupTag, const CCollider* pCollider)
{
	m_pColliderManager->AddColliderGroup(groupTag, pCollider);
}
const std::vector<const CCollider*>* CGameInstance::GetColliderGroup(const StringID& groupTag) const
{
	return m_pColliderManager->GetColliderGroup(groupTag);
}
_bool CGameInstance::IntersectColl(const CCollider* pColl1, const CCollider* pColl2)
{
	return m_pColliderManager->IntersectColl(pColl1, pColl2);
}
const std::unordered_map<StringID, std::vector<const CCollider*>>* CGameInstance::GetColliders() const
{
	return m_pColliderManager->GetColliders();
}
#pragma endregion

#pragma region RENDERER
HRESULT CGameInstance::AddRenderObject(RENDERGROUP eRenderGroup, IRenderable* pRenderObject)
{
	return m_pRenderer->AddRenderObject(eRenderGroup, pRenderObject);
}
#pragma endregion


#pragma region LIGHT_MANAGER

std::optional<DIRECTIONAL_LIGHT> CGameInstance::GetDirectionalLight(const StringID& iStr)
{
	return m_pLightManager->GetDirectionalLight(iStr);
}
HRESULT CGameInstance::SetDirectionalLight(const StringID& iStr, const std::optional<DIRECTIONAL_LIGHT>& light)
{
	return m_pLightManager->SetDirectionalLight(iStr, light);
}
#pragma endregion


#pragma region VOXEL_MANAGER
void CGameInstance::VoxelManagerStateUpdate(const VOXEL_MANAGER_STATE_UPDATE_DESC& desc)
{
	//m_pVoxelManager->StateUpdate(desc);
}
//_float CGameInstance::GetVoxelHeightNoise(_float x, _float z) const
//{
//	return m_pVoxelManager3->GetHeightNoise(x,z);
//}
FastNoiseLite& CGameInstance::GetVoxelNoiseByType(NOISE_TYPE eNoiseType)
{
	return m_pVoxelManager3->GetNoiseByType(eNoiseType);
}
const std::unordered_map<uint64_t, std::unordered_map<uint32_t, CBlock3>>& CGameInstance::GetVoxelEditShadow() const
{
	return m_pVoxelManager3->GetEditShadow();
}
CChunk3* CGameInstance::GetVoxelChunk(int32_t x, int32_t y, int32_t z) const
{
	return m_pVoxelManager3->GetChunkByChunkCoord(x, y, z);
	//return m_pVoxelManager->GetChunk(x, y, z);
	//return nullptr;
}

CChunk3* CGameInstance::GetVoxelChunkByWorldBlockCoord(int32_t x, int32_t y, int32_t z) const
{
	return m_pVoxelManager3->GetChunkByWorldBlockCoord(x, y, z);
}

void CGameInstance::VoxelProcessPlayerBlockSet(int32_t wbx, int32_t wby, int32_t wbz, CBlock3 block)
{
	return m_pVoxelManager3->ProcessPlayerBlockSet(wbx, wby, wbz, block);
}

std::optional<CBlock3> CGameInstance::GetVoxelBlock(int32_t wbx, int32_t wby, int32_t wbz) const
{
	return m_pVoxelManager3->GetBlock(wbx, wby, wbz);
}

void CGameInstance::SetVoxelBlock(int32_t wbx, int32_t wby, int32_t wbz, CBlock3 block)
{
	return m_pVoxelManager3->SetBlock(wbx, wby, wbz, block);
}

_bool CGameInstance::VoxelBlockRaycast(const _float3& rayOrigin, const _float3& rayDir, float fMaxDist, CVoxelManager3::BLOCK_RAY_RESULT& outResult) const
{
	return m_pVoxelManager3->BlockRaycast(rayOrigin, rayDir, fMaxDist , outResult);
}

bool CGameInstance::VoxelAABBOverlap(const _float3& pos, const _float3& halfExtents) const
{
	return m_pVoxelManager3->VoxelAABBOverlap(pos, halfExtents);
}
#pragma endregion

#pragma region FONT_MANAGER
void CGameInstance::FontDraw(const StringID& fontName, const _tchar* pText, const _float2& vPosition, float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
	m_pFontManager->Draw(fontName, pText, vPosition, fScale, vColor, fRotation, vOrigin);
}
void CGameInstance::FontAddLateDraw(RENDERGROUP eRenderGroup, const StringID& fontName, const _wstring& pText, const _float2& vPosition, float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
	m_pFontManager->AddLateDraw(eRenderGroup, fontName, pText, vPosition, fScale, vColor, fRotation, vOrigin);
}
_float2 CGameInstance::FontMeasureString(const StringID& fontName, const wchar_t* txt, float scale) const
{
	return m_pFontManager->MeasureString(fontName, txt, scale);
}
void CGameInstance::FontLateDraw(RENDERGROUP eRenderGroup)
{
	m_pFontManager->LateDraw(eRenderGroup);
}
#pragma endregion


#pragma region PARTICLE_MANAGER
void CGameInstance::AddParticleRenderDestruct(CBlock3 block, _float3 pos)
{
	m_pParticleManager->AddParticleRenderDestruct(block, pos);
}
#pragma endregion

#pragma region WORLD_MANAGER
CFurnaceStorage* CGameInstance::GetWorldFurnaceStorage()
{
	return m_pWorldManager->GetFurnaceStorage();
}
CChestStorage* CGameInstance::GetWorldChestStorage()
{
	return m_pWorldManager->GetChestStorage();
}
_float CGameInstance::GetWorldDayFactor() const
{
	return m_pWorldManager->GetDayFactor();
}
_float CGameInstance::GetWorldSkyRotation() const
{
	return m_pWorldManager->GetSkyRotation();
}
#pragma endregion

void CGameInstance::MouseFix() const
{
	RECT rect;
	GetClientRect(CGameInstance::Get().GetHwnd(), &rect);
	//POINT ul = { rect.left, rect.top };
	//POINT lr = { rect.right, rect.bottom };
	//ClientToScreen(CGameInstance::Get().GetHwnd(), &ul);
	//ClientToScreen(CGameInstance::Get().GetHwnd(), &lr);

	//RECT clipRect = { ul.x, ul.y, lr.x, lr.y };

	//ClipCursor(&clipRect); 

	POINT center;
	center.x = (rect.right - rect.left) / 2;
	center.y = (rect.bottom - rect.top) / 2;

	ClientToScreen(CGameInstance::Get().GetHwnd(), &center);
	SetCursorPos(center.x, center.y);
}