#pragma once
#include "Engine_Defines.h"
#include "ResourceManager.h"
#include "WorkerManager.h"
#include "GameObjectManager.h"
#include "VoxelManager3.h"
struct FMOD_SOUND;

NS_BEGIN(Engine)
class CTimeProvider;
class CSoundManager;
class CDInputManager;
class CGraphicDevice;
class CLevelManager;
class CLevel;
class CImguiManager;
class CPrototypeManager;
class CPrototype;
class CCameraManager;
class CColliderManager;
class CCollider;
class CRenderer;
class CLightManager;
class CVoxelManager;
class CVoxelManager2;
class CFontManager;
//class CVoxelManager3;
class CChunk3;
class CCameraObject;

class CParticleManager;
class ENGINE_DLL CGameInstance final : public Singleton<CGameInstance>
{
	friend Singleton<CGameInstance>;
private:
	CGameInstance();
	~CGameInstance();

public:
	HRESULT InitializeEngine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& ppDevice, ComPtr<ID3D11DeviceContext>& ppContext);
	void UpdateEngine(_float fTimeDelta);
	HRESULT Draw();
	void UpdateGUI();
	void ClearResource(uint32_t iClearLevelIndex);
	void FrameStart(_float fTimeDelta);
	void FrameEnd(_float fTimeDelta);

public:
	void Release_Engine();

public:
	_float2 GetClientScreenSize() const { return m_vClientScreenSize; }

private:
	HRESULT InitializeResources();
	HRESULT InitializeMCResource();
	HRESULT InitializePrototype();

public:
	HWND GetHwnd() const { return m_hWnd; }
private:
	HWND m_hWnd;


#pragma region RESOURCE_MANAGER
public:
	SPtr<CResource> AddResource(const StringID& sGroupTag, const StringID& sResTag, _string_id eAssetType, const _string& sPath, void* pArg = nullptr);
	SPtr<CResource> AddResource(const StringID& sGroupTag, const StringID& sResTag, SPtr<CResource> pAsset);
	template<typename T>
	SPtr<T> AddResourceT(const StringID& sGroupTag, const StringID& sResTag, const _string& sPath, void* pArg = nullptr)
	{
		return m_pResourceManager->AddResourceT<T>(sGroupTag, sResTag, sPath, pArg);
	}
	template<typename T>
	SPtr<T> AddResourceT(const StringID& sGroupTag, const StringID& sResTag, SPtr<T> pAsset)
	{
		return m_pResourceManager->AddResourceT<T>(sGroupTag, sResTag, pAsset);
	}
	template<typename T>
	SPtr<T> GetResourceFirst(const StringID& sGroupTag, const StringID& sResTag) const
	{
		return m_pResourceManager->GetResourceFirst<T>(sGroupTag, sResTag);
	}
	const std::vector<SPtr<CResource>>* GetResource(const StringID& sGroupTag, const StringID& sResTag) const;
	const std::unordered_map<StringID, std::vector<SPtr<CResource>>>* GetResource(const StringID& sGroupTag) const;
	HRESULT LoadResource(const StringID& sGroupTag);
	HRESULT LoadResource(const StringID& sGroupTag, const StringID& sResTag);
	HRESULT UnLoadResource(const StringID& sGroupTag);
	HRESULT UnLoadResource(const StringID& sGroupTag, const StringID& sResTag);
	void DelResource(const StringID& sGroupTag);
	void DelResource(const StringID& sGroupTag, const StringID& sResTag);
#pragma endregion

#pragma region SOUND_MANAGER
public:
	HRESULT CreateSound(const _string& sPath, FMOD_SOUND** ppSound);
#pragma endregion

#pragma region DINPUT_MANAGER
public:
	_bool KeyPressing(_ubyte byKeyID) const;
	_bool KeyUp(_ubyte byKeyID) const;
	_bool KeyDown(_ubyte byKeyID) const;
	int32_t	MouseMove(MOUSEMOVESTATE eMouseState) const;
	_bool MousePressing(MOUSEKEYSTATE eMouseState) const;
	_bool MouseUp(MOUSEKEYSTATE eMouseState) const;
	_bool MouseDown(MOUSEKEYSTATE eMouseState) const;
#pragma endregion

#pragma region GRAPHIC_DEVICE
public:
	ComPtr<ID3D11Device> GetGraphicDevice() const;
	ComPtr<ID3D11DeviceContext> GetGraphicDeviceContext()const;
	ComPtr<ID3D11RenderTargetView> GetBackBufferRTV() const;
	HRESULT ClearBackBufferView(const _float4* pClearColor);
	HRESULT ClearDepthStencilView();
	HRESULT Present();
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT ChangeLevel(UPtr<CLevel> pNewLevel);
	void RegisterLevelChangeFunc(const _string& ID, _Func func);
#pragma endregion


#pragma region IMGUI_MANAGER
public:
	void ImguiNewFrame();
	void ImguiEndFrameAndRender();
	_bool ImguiWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	_bool ImguiGetActive() const;
	void ImguiSetActive(_bool bActive);
#pragma endregion

#pragma region WORKER_MANAGER
public:
	void WorkerEnqueue(_string_view svTaskName, _Func func);
	template<typename Func, typename... Args>
	auto WorkerEnqueueWithFuture(_string_view svTaskName, Func&& f, Args&&... args)
		-> std::future<std::invoke_result_t<Func, Args...>>
	{
		return m_pChunkLoadWorkerManager->WorkerEnqueueWithFuture(
			svTaskName,
			std::forward<Func>(f),
			std::forward<Args>(args)...
		);
	}

	void ChunkLoadWorkerEnqueue(_string_view svTaskName, _Func func);
	template<typename Func, typename... Args>
	auto ChunkLoadWorkerEnqueueWithFuture(_string_view svTaskName, Func&& f, Args&&... args)
		-> std::future<std::invoke_result_t<Func, Args...>>
	{
		return m_pChunkLoadWorkerManager->WorkerEnqueueWithFuture(
			svTaskName,
			std::forward<Func>(f),
			std::forward<Args>(args)...
		);
	}
#pragma endregion

#pragma region TIME_PROVIDER
public:
	inline _float UpdateTimeProvider();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT AddPrototype(const StringID& svGroupTag, const StringID& svPrototypetag, UPtr<CPrototype> pPrototype);
	UPtr<CPrototype> ClonePrototype(const StringID& svGroupTag, const StringID& svPrototypetag, void* pArg = nullptr);
	void DelPrototype(const StringID& sGroupTag);
#pragma endregion


#pragma region GAMEOBJECT_MANAGER
public:
	void GameObjectAllReset();
	std::optional<CHandle> AddGameObjectToLayer(const StringID& iPrototypeLevelIndex, const StringID& svPrototypeTag, std::string_view sLayerName, void* pArg = nullptr);
	const std::vector<CHandle>* GetGameObjectLayer(std::string_view sLayerName) const;
	const std::vector<CHandle>* GetGameObjectLayer(std::string_view sLayerName, const StringID& iPrototypeLevelIndex, const StringID& svPrototypeTag, void* pArg);
	void DelGameObjectLayer(std::string_view sLayerName);

	//std::optional<CHandle> GetFreeHandle() const;

	inline CGameObject* GetGameObjectByHandle(const CHandle& handle);
	template<typename T>
	T* GetGameObjectByHandleT(const CHandle& handle)
	{
		return m_pGameObjectManager->GetGameObjectByHandleT<T>(handle);
	}
	template<typename T>
	const T* GetGameObjectByHandleT(const CHandle& handle) const
	{
		return static_cast<const CGameObjectManager*>(m_pGameObjectManager.get())->GetGameObjectByHandleT<T>(handle);
	}
#pragma endregion


#pragma region CAMERA_MANAGER
public:
	//const CCameraObject* GetCameraObject(const StringID& GroupID) const;
	//HRESULT SetCameraObject(const StringID& GroupID, const CHandle& handle);

	CCameraObject* GetActiveGameCamera() const;
	HRESULT SetActiveGameCamera(const StringID& CameraID);
	CCameraObject* GetActiveUICamera() const;
	HRESULT SetActiveUICamera(const StringID& CameraID);

	CCameraObject* GetActiveGameCamera(const StringID& CameraID) const;
	CCameraObject* GetActiveUICamera(const StringID& CameraID) const;

	CCameraObject* GetGameCamera(const StringID& CameraID) const;
	CCameraObject* GetUICamera(const StringID& CameraID) const;

	HRESULT RegistGameCamera(const StringID& CameraID, const CHandle& handle);
	HRESULT RegistUICamera(const StringID& CameraID, const CHandle& handle);
#pragma endregion


#pragma region COLLIDER_MANAGER
public:
	void AddColliderGroup(const StringID& groupTag, const CCollider*);
	const std::vector<const CCollider*>* GetColliderGroup(const StringID& groupTag) const;
	_bool IntersectColl(const CCollider* pColl1, const CCollider* pColl2);
	const std::unordered_map<StringID, std::vector<const CCollider*>>* GetColliders() const;
	//const CCollider* GetColliderGroupFirst(const StringID& groupTag) const;
#pragma endregion

#pragma region RENDERER
public:
	HRESULT AddRenderObject(RENDERGROUP eRenderGroup, IRenderable* pRenderObject);
#pragma endregion

#pragma region LIGHT_MANAGER
public:
	std::optional< DIRECTIONAL_LIGHT> GetDirectionalLight(const StringID& iStr);
	HRESULT SetDirectionalLight(const StringID& iStr, const std::optional< DIRECTIONAL_LIGHT>& light);
#pragma endregion

#pragma region VOXEL_MANAGER
public:
	void VoxelManagerStateUpdate(const VOXEL_MANAGER_STATE_UPDATE_DESC& desc);
	//_float GetVoxelHeightNoise(_float x, _float z) const;
	FastNoiseLite& GetVoxelNoiseByType(NOISE_TYPE eNoiseType);

	const std::unordered_map<uint64_t, std::unordered_map<uint32_t, CBlock3>>& GetVoxelEditShadow() const;
	CChunk3* GetVoxelChunk(int32_t x, int32_t y, int32_t z) const;
	CChunk3* GetVoxelChunkByWorldBlockCoord(int32_t x, int32_t y, int32_t z) const;

	void VoxelProcessPlayerBlockSet(int32_t wbx, int32_t wby, int32_t wbz, CBlock3 block);
	std::optional<CBlock3> GetVoxelBlock(int32_t wbx, int32_t wby, int32_t wbz) const;
	void SetVoxelBlock(int32_t wbx, int32_t wby, int32_t wbz, CBlock3 block);
	_bool VoxelBlockRaycast(const _float3& rayOrigin,
		const _float3& rayDir,
		float fMaxDist,
		CVoxelManager3::BLOCK_RAY_RESULT& outResult) const;

	bool VoxelAABBOverlap(const _float3& pos, const _float3& halfExtents) const;
#pragma endregion
	
#pragma region FONT_MANAGER
	void FontDraw(const StringID& fontName, const _tchar* pText, const _float2& vPosition, float fScale = 1.f, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = { 0.f, 0.f });
	void FontAddLateDraw(RENDERGROUP eRenderGroup, const StringID& fontName, const _wstring& pText, const _float2& vPosition, float fScale = 1.f, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = { 0.f, 0.f });
	_float2 FontMeasureString(const StringID& fontName, const wchar_t* txt, float scale = 1.f) const;
	void FontLateDraw(RENDERGROUP eRenderGroup);
#pragma


#pragma region PARTICLE_MANAGER
	void AddParticleRenderDestruct(CBlock3 block, _float3 pos);
#pragma
private:
	UPtr<CGraphicDevice> m_pGraphicDevice{};
	UPtr<CImguiManager> m_pImguiManager{};
	UPtr<CResourceManager> m_pResourceManager{};
	UPtr<CSoundManager> m_pSoundManager{};
	UPtr<CDInputManager> m_pDInputManager{};
	UPtr<CLevelManager> m_pLevelManager{};
	UPtr<CWorkerManager> m_pWorkerManager{};
	UPtr<CWorkerManager> m_pChunkLoadWorkerManager{};
	UPtr<CTimeProvider> m_pTimeProvider{};
	UPtr<CPrototypeManager> m_pPrototypeManager{};
	UPtr<CGameObjectManager> m_pGameObjectManager{};
	UPtr<CCameraManager> m_pCameraManager{};
	UPtr<CColliderManager> m_pColliderManager{};
	UPtr<CRenderer> m_pRenderer{};
	UPtr<CLightManager> m_pLightManager{};
	UPtr<CVoxelManager> m_pVoxelManager{};
	UPtr<CVoxelManager2> m_pVoxelManager2{};
	UPtr<CVoxelManager3> m_pVoxelManager3{};
	UPtr<CParticleManager> m_pParticleManager{};
	UPtr<CFontManager> m_pFontManager{};


public:
	_bool GetMouseFix() const { return m_bMouseFix; }
private:
	_bool m_bMouseFix{};


	_float2 m_vClientScreenSize{1280.f, 720.f};

private:
	void MouseFix() const;
};

NS_END
