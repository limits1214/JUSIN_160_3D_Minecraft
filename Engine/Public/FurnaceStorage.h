
#pragma once
#include "Engine_Defines.h"

#include "ItemObject.h"
#include "Block3.h"

NS_BEGIN(Engine)
class ENGINE_DLL CFurnaceStorage final: public CEngineBase
{
public:
	DECLARE_DERIVED_TYPE(CFurnaceStorage, CEngineBase)

private:
	explicit CFurnaceStorage();
	~CFurnaceStorage() override;

public:
	void Update(_float fTimeDelta);


public:
	struct SFurnaceStorage
	{
		std::optional<CItemObject::ItemInfo> ingredient{};
		std::optional<CItemObject::ItemInfo> fuel{};
		_float progress{};
		_float fuelProgress{};
		_float fuelDuration{};
		std::optional < CItemObject::ItemInfo> result{};
	};

public:
	SFurnaceStorage* GetStorage(const XMINT3& location)
	{
		auto iter = m_mapLocation.find(location);
		if (iter == m_mapLocation.end())
		{
			auto pair = m_mapLocation.emplace(location, SFurnaceStorage{});
			return &(*pair.first).second;
		}

		return &(*iter).second;
	}
private:
	std::unordered_map<XMINT3, SFurnaceStorage> m_mapLocation{};



	static std::optional<CItemObject::ItemInfo> GetSmeltResult(const CItemObject::ItemInfo& ingredient);
	// 연료 유효성 + 연소 시간(초) 반환
	static std::optional<float> GetFuelDuration(const CItemObject::ItemInfo& fuel);

	static constexpr float SMELT_TIME = 10.f;   // 제련 완료까지 걸리는 시간(초)
	static constexpr uint8_t MAX_STACK = 64;

public:
	static UPtr<CFurnaceStorage> Create();
};

NS_END