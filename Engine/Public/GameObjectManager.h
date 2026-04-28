#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"
#include "Handle.h"
#include "Slot.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObjectManager final : public CEngineBase
{
public:
	CGameObjectManager(const CGameObjectManager&) = delete;
	CGameObjectManager& operator=(const CGameObjectManager& rhs) = delete;

private:
	CGameObjectManager();
	~CGameObjectManager() override;

private:
	HRESULT Initialize();

public:
	void UpdateGUI();
	void UpdateGUIDrawTreeNode( CGameObject* handle);

	void FrameStart();
	void FrameEnd();

public:
	CGameObject* GetGameObjectByHandle(const CHandle& handle) { return const_cast<CGameObject*>(_GetGameObjectByHandle(handle)); }
	const CGameObject* GetGameObjectByHandle(const CHandle& handle) const { return _GetGameObjectByHandle(handle); }
	template<typename T> T* GetGameObjectByHandleT(const CHandle& handle);
	template<typename T> const T* GetGameObjectByHandleT(const CHandle& handle) const;
private:
	const CGameObject* _GetGameObjectByHandle(const CHandle& handle ) const;

public:
	std::optional<CHandle> AddGameObjectToLayer(const StringID& siProtoGroupTag, const StringID& siPrototypeTag, uint32_t iLayerIdx, void* pArg);
	const std::vector<CHandle>* GetLayer(uint32_t iLayerIdx) const;
	void DelLayer(uint32_t iLayerIdx);
	void LayerInitialize(uint32_t iNumLayers, std::function<std::string(uint32_t)> funcToString);

public:
	void AllReset();

public:
	void PriorityUpdate(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void LateUpdate(_float fTimeDelta);

private:
	std::vector<CSlot<CGameObject>> m_Objects{};
	std::vector<size_t> m_FreeSlots{};

	std::vector<std::vector<CHandle>> m_Layers{};
	std::function<std::string(uint32_t)> m_LayerToStringFunc{};

	std::vector<CGameObject*> m_TreePreparation{};
	std::vector<CGameObject*> m_Tree{};
	std::vector<CGameObject*> m_DFSReserved{};
	_bool m_bTreeReBuild{ true };

public:
	static UPtr< CGameObjectManager> Create();

public:
	void Free() override;
};

NS_END

inline const Engine::CGameObject* Engine::CGameObjectManager::_GetGameObjectByHandle(const CHandle& handle) const
{
	size_t idx = handle.GetIndex();
	if (idx >= m_Objects.size())
	{
		return nullptr;
	}

	const auto& slot = m_Objects[idx];
	if (!slot.IsOccupied())
	{
		return nullptr;
	}
	if (slot.GetGeneration() != handle.GetGeneration())
	{
		return nullptr;
	}

	return slot.Get();
}

template<typename T>
inline T* Engine::CGameObjectManager::GetGameObjectByHandleT(const CHandle& handle)
{
	const CGameObject* obj = _GetGameObjectByHandle(handle);

	if (!obj)
	{
		return nullptr;
	}

	if (obj->GetType() != T::StaticType)
	{
		return nullptr;
	}
	
	return const_cast<T*>(static_cast<const T*>(obj));
}


template<typename T>
inline const T* Engine::CGameObjectManager::GetGameObjectByHandleT(const CHandle& handle) const
{
	const CGameObject* obj = _GetGameObjectByHandle(handle);

	if (!obj)
	{
		return nullptr;
	}

	if (obj->GetType() != T::StaticType)
	{
		return nullptr;
	}

	return static_cast<const T*>(obj);
}
