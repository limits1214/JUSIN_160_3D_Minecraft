
#pragma once
#include "VoxelObject.h"

#include "ItemObject.h"
#include "Block3.h"

NS_BEGIN(Engine)
class ENGINE_DLL CFurnaceStorage : public CVoxelObject
{
public:
	typedef struct tagDesc : CVoxelObject::DESC
	{
		//std::pair<StringID, StringID> viBufferId{};
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CFurnaceStorage, CVoxelObject)

private:
	explicit CFurnaceStorage();
	~CFurnaceStorage() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;

	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	struct SFurnaceStorage
	{
		std::optional<CItemObject::ItemInfo> ingredient{};
		std::optional<CItemObject::ItemInfo> fuel{};
		_float progress{};
		std::optional < CItemObject::ItemInfo> result{};
	};

public:
	SFurnaceStorage* GetStorage(const XMINT3& location)
	{
		auto iter = m_mapLocation.find(location);
		if (iter == m_mapLocation.end())
		{
			return nullptr;
		}

		return &(*iter).second;
	}
private:
	std::unordered_map<XMINT3, SFurnaceStorage> m_mapLocation{};

public:
	static UPtr<CFurnaceStorage> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END