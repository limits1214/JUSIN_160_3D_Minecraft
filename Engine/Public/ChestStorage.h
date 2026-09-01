#pragma once
#include "Engine_Defines.h"

#include "ItemObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CChestStorage final: public CEngineBase
{
public:
	DECLARE_DERIVED_TYPE(CChestStorage, CEngineBase)

private:
	explicit CChestStorage();
	~CChestStorage() override;

public:
	struct SChestStorage
	{
		std::array<std::optional<CItemObject::ItemInfo>, 9 * 3> items{};
	};
public:
	SChestStorage* GetStorage(const XMINT3& location)
	{
		auto iter = m_mapLocation.find(location);
		if (iter == m_mapLocation.end())
		{
			auto pair = m_mapLocation.emplace(location, SChestStorage{});
			return &(*pair.first).second;
		}

		return &(*iter).second;
	}
	void DelStorage(const XMINT3& location)
	{
		auto iter = m_mapLocation.find(location);
		if (iter != m_mapLocation.end())
		{
			m_mapLocation.erase(iter);
		}
	}
private:
	std::unordered_map<XMINT3, SChestStorage> m_mapLocation{};

public:
	void Update(E::_float fTimeDelta) ;

public:
	static UPtr<CChestStorage> Create();
};

NS_END