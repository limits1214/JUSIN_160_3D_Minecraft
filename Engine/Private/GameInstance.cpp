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
#include "VoxelManager.h"
#include "VoxelManager2.h"
#include "VoxelManager3.h"
#include "ParticleManager.h"
#include "FontManager.h"

#include "GameObject.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Collider.h"

#include "Renderer.h"

#include "FlyCamera.h"
#include "UICamera.h"
#include "PlayerCamera.h"

#include "Resources.h"

#include "ComEntityModel.h"
#include "ComConstantBuffer.h"



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
	m_vClientScreenSize.x = EngineDesc.iWinSizeX;
	m_vClientScreenSize.y = EngineDesc.iWinSizeY;
	

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

	m_pCameraManager->UpdateGUI();

	m_pLevelManager->UpdateGUI();

	m_pColliderManager->UpdateGUI();

	m_pParticleManager->UpdateGUI();

	m_pLightManager->UpdateGUI();

	m_pVoxelManager3->UpdateGUI();
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
	m_pGameObjectManager->AllReset();
	m_pGameObjectManager.reset();
	m_pLevelManager.reset();
	m_pColliderManager.reset();
	m_pParticleManager.reset();
	m_pWorkerManager.reset();
	m_pChunkLoadWorkerManager.reset();
	m_pPrototypeManager.reset();
	m_pFontManager.reset();
	m_pLightManager.reset();
	m_pVoxelManager.reset();
	m_pVoxelManager3.reset();
	m_pResourceManager.reset();
	m_pRenderer.reset();

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
		res->Load(D3D11_SAMPLER_DESC{
			.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT, // 핵심
			.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
			.ComparisonFunc = D3D11_COMPARISON_NEVER,
			.MinLOD = 0,
			.MaxLOD = D3D11_FLOAT32_MAX,
			});
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
	return S_OK;
}

HRESULT CGameInstance::InitializeMCResource()
{
	// initialize block shader(voxel)
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
		if (auto res = AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Item", "./Resources/Shader/Item/Item.hlsl"))
		{
			res->Load();
		}
		if (auto res = AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Item", "./Resources/Shader/Item/Item.hlsl"))
		{
			res->Load();
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
			{
				//0
				auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/dirt.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
			}

			{
				//1
				auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/stone.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
			}

			{
				//2
				auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/sand.png");
				if (FAILED(pTexture->Load()))
				{
					return E_FAIL;
				}
				CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
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
	}

	// initialize cube item
	{
		if (auto res = AddResource("MC_ITEM_VIBuffer", "CubeItemDirt", CResCubeItemVIBuffer::Create()))
		{
			uint32_t tmp[ETOUI(FACE_DIR::END)]{PackTexId(9, 0),PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) };

			CResCubeItemVIBuffer::DESC desc{};
			desc.textureId = { "VOXEL_MANAGER_TEX", "TEXTURES" };
			desc.resourceIdx = 0;
			memcpy(desc.texIndices, tmp, sizeof(tmp));

			res->Load(desc);
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
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 0, .texIndex = PackTexId(6, 0)});
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
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 1, .texIndex = PackTexId(6, 1) });
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
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 2, .texIndex = PackTexId(6, 2) });
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
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 3, .texIndex = PackTexId(6, 3) });
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
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 4, .texIndex = PackTexId(6, 4) });
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
					res->Load(CResExtrudedItemVIBuffer::DESC{ .textureId = {"MC_TEX_ITEM_16_16", "TEXTURES"}, .resourceIdx = 5 , .texIndex = PackTexId(6, 5) });
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
_float CGameInstance::GetVoxelHeightNoise(_float x, _float z) const
{
	return m_pVoxelManager3->GetHeightNoise(x,z);
}
CChunk3* CGameInstance::GetVoxelChunk(int32_t x, int32_t y, int32_t z) const
{
	return m_pVoxelManager3->GetChunkByChunkCoord(x, y, z);
	//return m_pVoxelManager->GetChunk(x, y, z);
	//return nullptr;
}

std::optional<CBlock3> CGameInstance::GetVoxelBlock(int32_t wbx, int32_t wby, int32_t wbz) const
{
	return m_pVoxelManager3->GetBlock(wbx, wby, wbz);
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