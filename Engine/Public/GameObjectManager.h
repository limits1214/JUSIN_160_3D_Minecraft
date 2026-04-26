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

public:
	void UpdateGUI();
	void UpdateGUIDrawTreeNode(const CHandle& handle);

	void FrameStart();
	void FrameEnd();

public:
	std::optional<CHandle> AddGameObjectToLayer(const StringID& siProtoGroupTag, const StringID& siPrototypeTag, const _string& siLayerTag, void* pArg);
	CGameObject* GetGameObjectByHandle(const CHandle& handle) { return const_cast<CGameObject*>(_GetGameObjectByHandle(handle)); }
	const CGameObject* GetGameObjectByHandle(const CHandle& handle) const { return _GetGameObjectByHandle(handle); }
	template<typename T>
	T* GetGameObjectByHandleT(const CHandle& handle);
	template<typename T>
	const T* GetGameObjectByHandleT(const CHandle& handle) const;
private:
	const CGameObject* _GetGameObjectByHandle(const CHandle& handle) const;

public:
	const std::vector<CHandle>* GetLayer(const _string& siLayerTag) const;

public:
	void AllReset();
	void DelLayer(const _string& siLayerTag);

public:
	void PriorityUpdate(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void LateUpdate(_float fTimeDelta);

private:
	const std::vector<CHandle>* FindLayer(const _string& siLayerTag) const;

private:
	std::vector<CSlot<CGameObject>> m_Objects{};
	std::vector<size_t> m_FreeSlots{};

	std::map<_string, std::vector<CHandle>> m_Layers{};
	std::vector<CHandle> m_TreePreparation{};
	std::vector<CGameObject*> m_Tree{};

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
	auto obj = const_cast<Engine::CGameObject*>(_GetGameObjectByHandle(handle));

	if (!obj)
	{
		return nullptr;
	}

	if (obj->GetType() != T::StaticType)
	{
		return nullptr;
	}

	return static_cast<T*>(obj);
}


template<typename T>
inline const T* Engine::CGameObjectManager::GetGameObjectByHandleT(const CHandle& handle) const
{
	auto obj = _GetGameObjectByHandle(handle);

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
