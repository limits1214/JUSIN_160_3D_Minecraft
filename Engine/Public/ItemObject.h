#pragma once
#include "GameObject.h"
#include "Block3.h"
//#include "UIItem.h"

NS_BEGIN(Engine)
class ENGINE_DLL CItemObject : public CGameObject
{

public:
	enum class ITEM_TYPE
	{
		ITEM_WoodPickaxe,
		ITEM_CooperPickaxe,
		ITEM_CooperHelmet,
		ITEM_Coal,
		ITEM_Stick,
		ITEM_Torch,
		END
	};
	struct ItemInfo
	{
		ITEM_TYPE eItemType{ CItemObject::ITEM_TYPE::END };
		std::optional<CBlock3> block{};
		_float fDurability{ 1.f };
		uint8_t iCnt{};
		ItemInfo() = default;
		ItemInfo(ITEM_TYPE _itemType, uint8_t _iCnt = 0)
			: eItemType{ _itemType }
			, iCnt{ _iCnt }
		{
		}
		ItemInfo(CBlock3 _block, uint8_t _iCnt = 0)
			:block{_block}
			, iCnt{_iCnt}
		{
		}
	};

public:
	
	struct SRecipe
	{
		SRecipe() { pattern.resize(9); }
		std::vector<std::pair<std::optional<ItemInfo>, void*>> pattern{};
		ItemInfo result{};
		std::string name{};
	};
	inline static std::vector< SRecipe> s_vecRecipies{};
	inline static _bool s_bRecipiesInitialize{false};
	static void RecipeInitialize();


	struct STrimmedGrid
	{
		int iWidth{};
		int iHeight{};
		std::vector<std::pair<std::optional<ItemInfo>, void*>> grid{};
	};

	static STrimmedGrid GetTrimmedGrid(const std::vector<std::pair<std::optional<ItemInfo>, void*>>& grid, int iGridWidth, int iGridHeight)
	{
		int minX = iGridWidth, maxX = -1;
		int minY = iGridHeight, maxY = -1;

		for (int y = 0; y < iGridHeight; ++y)
		{
			for (int x = 0; x < iGridWidth; ++x)
			{
				int idx = y * iGridWidth + x;
				if (grid[idx].first)
				{
					if (x < minX) minX = x;
					if (x > maxX) maxX = x;
					if (y < minY) minY = y;
					if (y > maxY) maxY = y;
				}
			}
		}

		if (maxX == -1) return {};

		int trimmedW = maxX - minX + 1;
		int trimmedH = maxY - minY + 1;

		STrimmedGrid result{};
		result.iWidth = trimmedW;
		result.iHeight = trimmedH;
		result.grid.resize(trimmedW * trimmedH);
		for (int y = 0; y < trimmedH; ++y)
		{
			for (int x = 0; x < trimmedW; ++x)
			{
				int srcIdx = (minY + y) * iGridWidth + (minX + x);
				int destIdx = y * trimmedW + x;
				result.grid[destIdx] = grid[srcIdx];
			}
		}

		return result;
	}

	static _bool MatchTrimmedGrid(const STrimmedGrid& srcGrid, const STrimmedGrid& dstGrid)
	{
		if (srcGrid.iWidth != dstGrid.iWidth || srcGrid.iHeight != dstGrid.iHeight)
		{
			return false;
		}

		for (size_t i = 0; i < srcGrid.grid.size(); ++i)
		{
			if (srcGrid.grid[i].first.has_value() != dstGrid.grid[i].first.has_value())
			{
				return false;
			}

			if (srcGrid.grid[i].first.has_value())
			{
				if (srcGrid.grid[i].first->block.has_value() != dstGrid.grid[i].first->block.has_value())
				{
					return false;
				}
				if (srcGrid.grid[i].first->block.has_value())
				{
					if (srcGrid.grid[i].first->block->GetType() != dstGrid.grid[i].first->block->GetType())
					{
						return false;
					}
				}
				else
				{
					if (srcGrid.grid[i].first->eItemType != dstGrid.grid[i].first->eItemType)
					{
						return false;
					}
				}

				if (srcGrid.grid[i].first->iCnt < dstGrid.grid[i].first->iCnt)
				{
					return false;
				}
			}

			
		}

		return true;
	}

	static const _tchar* GetItemName(const ItemInfo& info)
	{
		if (info.block)
		{
			switch (info.block->GetType())
			{
			case CBlock3::TYPE::DIRT:
				return L"흙흙";
			case CBlock3::TYPE::LOG_ACACIA:
				return L"아카시아나무";
			case CBlock3::TYPE::LOG_BIRCH:
				return L"BIRCH나무";
			case CBlock3::TYPE::LOG_OAK:
				return L"참나무";
			case CBlock3::TYPE::LOG_CHERRY:
				return L"벗꼿나무";
			};
		}
		else
		{
			switch (info.eItemType)
			{
			case ITEM_TYPE::ITEM_WoodPickaxe:
				return L"나무곡괭이";
			case ITEM_TYPE::ITEM_CooperPickaxe:
				return L"구리곡괭이";
			case ITEM_TYPE::ITEM_CooperHelmet:
				return L"구리헬맷";
			case ITEM_TYPE::ITEM_Coal:
				return L"석탄";
			case ITEM_TYPE::ITEM_Stick:
				return L"막대";
			};
		}

		return L"NO_NAME";
	}

	static _bool IsCountableItem(CItemObject::ITEM_TYPE eType)
	{
		switch (eType)
		{
		case CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_CooperPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_CooperHelmet:
			return false;
		}
		return true;
	}

	static uint32_t GetPackedTexIdByType(ITEM_TYPE eType)
	{
		switch (eType)
		{
		case Engine::CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
			return  PackTexId(6, 0);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CooperPickaxe:
			return  PackTexId(6, 7);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CooperHelmet:
			return  PackTexId(6, 6);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Coal:
			return  PackTexId(6, 8);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Stick:
			return  PackTexId(6, 9);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Torch:
			return  PackTexId(6, 10);
		}
		return 0;
	}

	
public:
	typedef struct tagDesc : GAMEOBJECT_DESC
	{
		ItemInfo info{};
	}DESC;
public:
	DECLARE_DERIVED_TYPE(CItemObject, CGameObject)

public:
	ItemInfo* GetItemInfo()
	{
		if (!m_ItemInfo.has_value())
			return nullptr;

		return &m_ItemInfo.value();
	}
	void SetItemInfo(std::optional<ItemInfo> info) { m_ItemInfo = info; }

private:
	std::optional<ItemInfo> m_ItemInfo{};

protected:
	explicit CItemObject();
	~CItemObject() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END