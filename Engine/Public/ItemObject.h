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
		ITEM_StonePickaxe,
		ITEM_CopperPickaxe,
		ITEM_IronPickaxe,
		ITEM_GoldPickaxe,
		ITEM_DiamondPickaxe,
		ITEM_NetheritePickaxe,

		ITEM_WoodAxe,
		ITEM_StoneAxe,
		ITEM_CopperAxe,
		ITEM_IronAxe,
		ITEM_GoldAxe,
		ITEM_DiamondAxe,
		ITEM_NetheriteAxe,

		ITEM_WoodShovel,
		ITEM_StoneShovel,
		ITEM_CopperShovel,
		ITEM_IronShovel,
		ITEM_GoldShovel,
		ITEM_DiamondShovel,
		ITEM_NetheriteShovel,

		ITEM_WoodHoe,
		ITEM_StoneHoe,
		ITEM_CopperHoe,
		ITEM_IronHoe,
		ITEM_GoldHoe,
		ITEM_DiamondHoe,
		ITEM_NetheriteHoe,

		ITEM_WoodSword,
		ITEM_StoneSword,
		ITEM_CopperSword,
		ITEM_IronSword,
		ITEM_GoldSword,
		ITEM_DiamondSword,
		ITEM_NetheriteSword,

		ITEM_CopperHelmet,
		ITEM_IronHelmet,
		ITEM_GoldHelmet,
		ITEM_DiamondHelmet,
		ITEM_NetheriteHelmet,

		ITEM_CopperChestplate,
		ITEM_IronChestplate,
		ITEM_GoldChestplate,
		ITEM_DiamondChestplate,
		ITEM_NetheriteChestplate,

		ITEM_CopperLeggings,
		ITEM_IronLeggings,
		ITEM_GoldLeggings,
		ITEM_DiamondLeggings,
		ITEM_NetheriteLeggings,

		ITEM_CopperBoots,
		ITEM_IronBoots,
		ITEM_GoldBoots,
		ITEM_DiamondBoots,
		ITEM_NetheriteBoots,

		ITEM_Bow_Standby,
		ITEM_Bow_Pulling_0,
		ITEM_Bow_Pulling_1,
		ITEM_Bow_Pulling_2,

		ITEM_Arrow,

		ITEM_Raw_Iron,
		ITEM_Raw_Copper,
		ITEM_Raw_Gold,
		ITEM_Iron_Ingot,
		ITEM_Copper_Ingot,
		ITEM_Gold_Ingot,
		ITEM_Netherite_Ingot,
		ITEM_Diamond,
		ITEM_NetheriteScrap,

		ITEM_Charcoal,

		ITEM_Coal,
		ITEM_Stick,
		ITEM_Torch,
		ITEM_String,
		ITEM_Feather,
		ITEM_Flint,

		ITEM_Bucket_Empty,
		ITEM_Bucket_Water,
		ITEM_Bucket_Lava,

		ITEM_FlintAndSteel,
		ITEM_Gunpowder,

		ITEM_Raw_Beef,
		ITEM_Raw_Chicken,
		ITEM_Raw_Porkchop,
		ITEM_Raw_Mutton,

		ITEM_Beef_Cooked,
		ITEM_Chicken_Cooked,
		ITEM_Porkchop_Cooked,
		ITEM_Mutton_Cooked,
		ITEM_Apple,
		ITEM_Wheat,
		ITEM_Bread,

		ITEM_Flower_Allium,
		ITEM_Flower_Blue_Orchid,
		ITEM_Flower_CornFlower,
		ITEM_Flower_Dandelion,
		ITEM_Flower_Houstonia,
		ITEM_Flower_Lily_Of_The_Valley,
		ITEM_Flower_Oxeye_Daisy,
		ITEM_Flower_Paeonia,
		ITEM_Flower_Rose,
		ITEM_Flower_Rose_Blue,
		ITEM_Flower_Tulip_Orange,
		ITEM_Flower_Tulip_Pink,
		ITEM_Flower_Tulip_Red,
		ITEM_Flower_WIther_Rose,

		ITEM_Seeds_Wheat,

		ITEM_Bless_Lv_0,
		ITEM_Bless_Lv_2,
		ITEM_Bless_Lv_4,
		ITEM_Bless_Lv_6,
		ITEM_Bless_Lv_8,
		ITEM_Bless_Lv_10,
		END,
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
				return L"흙";
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
			case ITEM_TYPE::ITEM_CopperPickaxe:
				return L"구리곡괭이";
			case ITEM_TYPE::ITEM_CopperHelmet:
				return L"구리헬맷";
			case ITEM_TYPE::ITEM_Coal:
				return L"석탄";
			case ITEM_TYPE::ITEM_Stick:
				return L"막대";
			case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_0:
				return L"???의 가호 LV0 \n\n          : ???";
			case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_2:
				return L"160기의 가호 LV 2\n\n무한의화살: 화살을 소비 하지 않습니다.\n                 : ???";
			case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_4:
				return L"자습실의 가호 LV 4\n\n무한의화살: 화살을 소비 하지 않습니다.\n폭풍의 시  : 빠르게 화살을 쏩니다.\n                 : ???";
			case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_6:
				return L"아지트의 가호 LV 6\n\n무한의화살: 화살을 소비 하지 않습니다.\n에로우 밤  : 화살이 폭발합니다.\n                 : ???";
			case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_8:
				return L"포트폴리오실의 가호 LV 8\n\n무한의화살: 화살을 소비 하지 않습니다.\n폭풍의 시  : 빠르게 화살을 쏩니다.\n에로우 밤  : 화살이 폭발합니다.\n                 : ???";
			case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_10:
				return L"쥬신의 가호 LV 10\n\n무한의화살: 화살을 소비 하지 않습니다.\n폭풍의 시: 빠르게 화살을 쏩니다.\n에로우 밤: 화살이 폭발합니다.\nTHE GOD        : 신이 되어 날아다닙니다.";
			};
		}

		return L"NO_NAME";
	}

	static _bool IsCountableItem(CItemObject::ITEM_TYPE eType)
	{
		switch (eType)
		{
		case CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_StonePickaxe:
		case CItemObject::ITEM_TYPE::ITEM_CopperPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_IronPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_GoldPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_DiamondPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_NetheritePickaxe:

		case CItemObject::ITEM_TYPE::ITEM_WoodAxe:
		case CItemObject::ITEM_TYPE::ITEM_StoneAxe:
		case CItemObject::ITEM_TYPE::ITEM_CopperAxe:
		case CItemObject::ITEM_TYPE::ITEM_IronAxe:
		case CItemObject::ITEM_TYPE::ITEM_GoldAxe:
		case CItemObject::ITEM_TYPE::ITEM_DiamondAxe:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteAxe:

		case CItemObject::ITEM_TYPE::ITEM_WoodHoe:
		case CItemObject::ITEM_TYPE::ITEM_StoneHoe:
		case CItemObject::ITEM_TYPE::ITEM_CopperHoe:
		case CItemObject::ITEM_TYPE::ITEM_IronHoe:
		case CItemObject::ITEM_TYPE::ITEM_GoldHoe:
		case CItemObject::ITEM_TYPE::ITEM_DiamondHoe:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteHoe:

		case CItemObject::ITEM_TYPE::ITEM_WoodShovel:
		case CItemObject::ITEM_TYPE::ITEM_StoneShovel:
		case CItemObject::ITEM_TYPE::ITEM_CopperShovel:
		case CItemObject::ITEM_TYPE::ITEM_IronShovel:
		case CItemObject::ITEM_TYPE::ITEM_GoldShovel:
		case CItemObject::ITEM_TYPE::ITEM_DiamondShovel:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteShovel:

		case CItemObject::ITEM_TYPE::ITEM_WoodSword:
		case CItemObject::ITEM_TYPE::ITEM_StoneSword:
		case CItemObject::ITEM_TYPE::ITEM_CopperSword:
		case CItemObject::ITEM_TYPE::ITEM_IronSword:
		case CItemObject::ITEM_TYPE::ITEM_GoldSword:
		case CItemObject::ITEM_TYPE::ITEM_DiamondSword:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteSword:


		case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:
		case CItemObject::ITEM_TYPE::ITEM_IronHelmet:
		case CItemObject::ITEM_TYPE::ITEM_GoldHelmet:
		case CItemObject::ITEM_TYPE::ITEM_DiamondHelmet:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteHelmet:

		case CItemObject::ITEM_TYPE::ITEM_CopperChestplate:
		case CItemObject::ITEM_TYPE::ITEM_IronChestplate:
		case CItemObject::ITEM_TYPE::ITEM_GoldChestplate:
		case CItemObject::ITEM_TYPE::ITEM_DiamondChestplate:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteChestplate:

		case CItemObject::ITEM_TYPE::ITEM_CopperLeggings:
		case CItemObject::ITEM_TYPE::ITEM_IronLeggings:
		case CItemObject::ITEM_TYPE::ITEM_GoldLeggings:
		case CItemObject::ITEM_TYPE::ITEM_DiamondLeggings:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteLeggings:

		case CItemObject::ITEM_TYPE::ITEM_CopperBoots:
		case CItemObject::ITEM_TYPE::ITEM_IronBoots:
		case CItemObject::ITEM_TYPE::ITEM_GoldBoots:
		case CItemObject::ITEM_TYPE::ITEM_DiamondBoots:
		case CItemObject::ITEM_TYPE::ITEM_NetheriteBoots:

		case CItemObject::ITEM_TYPE::ITEM_Bow_Standby:
		case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0:
		case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1:
		case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2:

		case CItemObject::ITEM_TYPE::ITEM_Bucket_Water:
		case CItemObject::ITEM_TYPE::ITEM_Bucket_Lava:

		case CItemObject::ITEM_TYPE::ITEM_FlintAndSteel:

		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_0:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_2:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_4:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_6:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_8:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_10:
			return false;
		}
		return true;
	}


	static uint32_t IsEatableItem(CItemObject::ITEM_TYPE eType)
	{
		switch (eType)
		{
			case CItemObject::ITEM_TYPE::ITEM_Apple:
			case CItemObject::ITEM_TYPE::ITEM_Bread:
			case CItemObject::ITEM_TYPE::ITEM_Raw_Beef:
			case CItemObject::ITEM_TYPE::ITEM_Raw_Chicken:
			case CItemObject::ITEM_TYPE::ITEM_Raw_Mutton:
			case CItemObject::ITEM_TYPE::ITEM_Raw_Porkchop:
			case CItemObject::ITEM_TYPE::ITEM_Beef_Cooked:
			case CItemObject::ITEM_TYPE::ITEM_Chicken_Cooked:
			case CItemObject::ITEM_TYPE::ITEM_Mutton_Cooked:
			case CItemObject::ITEM_TYPE::ITEM_Porkchop_Cooked:
				return 5;
		}
		return 0;
	}

	static uint32_t GetPackedTexIdByType(ITEM_TYPE eType)
	{
		switch (eType)
		{
		case Engine::CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
			return  PackTexId(6, 0);
		case Engine::CItemObject::ITEM_TYPE::ITEM_StonePickaxe:
			return  PackTexId(6, 11);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CopperPickaxe:
			return  PackTexId(6, 7);
		case Engine::CItemObject::ITEM_TYPE::ITEM_IronPickaxe:
			return  PackTexId(6, 12);
		case Engine::CItemObject::ITEM_TYPE::ITEM_GoldPickaxe:
			return  PackTexId(6, 13);
		case Engine::CItemObject::ITEM_TYPE::ITEM_DiamondPickaxe:
			return  PackTexId(6, 14);
		case Engine::CItemObject::ITEM_TYPE::ITEM_NetheritePickaxe:
			return  PackTexId(6, 15);

		case Engine::CItemObject::ITEM_TYPE::ITEM_WoodAxe:
			return  PackTexId(6, 35);
		case Engine::CItemObject::ITEM_TYPE::ITEM_StoneAxe:
			return  PackTexId(6, 36);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CopperAxe:
			return  PackTexId(6, 37);
		case Engine::CItemObject::ITEM_TYPE::ITEM_IronAxe:
			return  PackTexId(6, 38);
		case Engine::CItemObject::ITEM_TYPE::ITEM_GoldAxe:
			return  PackTexId(6, 39);
		case Engine::CItemObject::ITEM_TYPE::ITEM_DiamondAxe:
			return  PackTexId(6, 40);
		case Engine::CItemObject::ITEM_TYPE::ITEM_NetheriteAxe:
			return  PackTexId(6, 41);

		case Engine::CItemObject::ITEM_TYPE::ITEM_WoodHoe:
			return  PackTexId(6, 42);
		case Engine::CItemObject::ITEM_TYPE::ITEM_StoneHoe:
			return  PackTexId(6, 43);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CopperHoe:
			return  PackTexId(6, 44);
		case Engine::CItemObject::ITEM_TYPE::ITEM_IronHoe:
			return  PackTexId(6, 45);
		case Engine::CItemObject::ITEM_TYPE::ITEM_GoldHoe:
			return  PackTexId(6, 46);
		case Engine::CItemObject::ITEM_TYPE::ITEM_DiamondHoe:
			return  PackTexId(6, 47);
		case Engine::CItemObject::ITEM_TYPE::ITEM_NetheriteHoe:
			return  PackTexId(6, 48);

		case Engine::CItemObject::ITEM_TYPE::ITEM_WoodShovel:
			return  PackTexId(6, 49);
		case Engine::CItemObject::ITEM_TYPE::ITEM_StoneShovel:
			return  PackTexId(6, 50);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CopperShovel:
			return  PackTexId(6, 51);
		case Engine::CItemObject::ITEM_TYPE::ITEM_IronShovel:
			return  PackTexId(6, 52);
		case Engine::CItemObject::ITEM_TYPE::ITEM_GoldShovel:
			return  PackTexId(6, 53);
		case Engine::CItemObject::ITEM_TYPE::ITEM_DiamondShovel:
			return  PackTexId(6, 54);
		case Engine::CItemObject::ITEM_TYPE::ITEM_NetheriteShovel:
			return  PackTexId(6, 55);

		case Engine::CItemObject::ITEM_TYPE::ITEM_WoodSword:
			return  PackTexId(6, 56);
		case Engine::CItemObject::ITEM_TYPE::ITEM_StoneSword:
			return  PackTexId(6, 57);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CopperSword:
			return  PackTexId(6, 58);
		case Engine::CItemObject::ITEM_TYPE::ITEM_IronSword:
			return  PackTexId(6, 59);
		case Engine::CItemObject::ITEM_TYPE::ITEM_GoldSword:
			return  PackTexId(6, 60);
		case Engine::CItemObject::ITEM_TYPE::ITEM_DiamondSword:
			return  PackTexId(6, 61);
		case Engine::CItemObject::ITEM_TYPE::ITEM_NetheriteSword:
			return  PackTexId(6, 62);

		case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:
			return  PackTexId(6, 6);
		case CItemObject::ITEM_TYPE::ITEM_IronHelmet:
			return  PackTexId(6, 16);
		case CItemObject::ITEM_TYPE::ITEM_GoldHelmet:
			return  PackTexId(6, 17);
		case CItemObject::ITEM_TYPE::ITEM_DiamondHelmet:
			return  PackTexId(6, 18);
		case CItemObject::ITEM_TYPE::ITEM_NetheriteHelmet:
			return  PackTexId(6, 19);

		case CItemObject::ITEM_TYPE::ITEM_CopperChestplate:
			return  PackTexId(6, 20);
		case CItemObject::ITEM_TYPE::ITEM_IronChestplate:
			return  PackTexId(6, 21);
		case CItemObject::ITEM_TYPE::ITEM_GoldChestplate:
			return  PackTexId(6, 22);
		case CItemObject::ITEM_TYPE::ITEM_DiamondChestplate:
			return  PackTexId(6, 23);
		case CItemObject::ITEM_TYPE::ITEM_NetheriteChestplate:
			return  PackTexId(6, 24);

		case CItemObject::ITEM_TYPE::ITEM_CopperLeggings:
			return  PackTexId(6, 25);
		case CItemObject::ITEM_TYPE::ITEM_IronLeggings:
			return  PackTexId(6, 26);
		case CItemObject::ITEM_TYPE::ITEM_GoldLeggings:
			return  PackTexId(6, 27);
		case CItemObject::ITEM_TYPE::ITEM_DiamondLeggings:
			return  PackTexId(6, 28);
		case CItemObject::ITEM_TYPE::ITEM_NetheriteLeggings:
			return  PackTexId(6, 29);

		case CItemObject::ITEM_TYPE::ITEM_CopperBoots:
			return  PackTexId(6, 30);
		case CItemObject::ITEM_TYPE::ITEM_IronBoots:
			return  PackTexId(6, 31);
		case CItemObject::ITEM_TYPE::ITEM_GoldBoots:
			return  PackTexId(6, 32);
		case CItemObject::ITEM_TYPE::ITEM_DiamondBoots:
			return  PackTexId(6, 33);
		case CItemObject::ITEM_TYPE::ITEM_NetheriteBoots:
			return  PackTexId(6, 34);


		case CItemObject::ITEM_TYPE::ITEM_Bow_Standby:
			return  PackTexId(6, 63);
		case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0:
			return  PackTexId(6, 64);
		case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1:
			return  PackTexId(6, 65);
		case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2:
			return  PackTexId(6, 66);

		case CItemObject::ITEM_TYPE::ITEM_Arrow:
			return  PackTexId(6, 67);

		case CItemObject::ITEM_TYPE::ITEM_Raw_Iron:
			return  PackTexId(6, 68);
		case CItemObject::ITEM_TYPE::ITEM_Raw_Copper:
			return  PackTexId(6, 69);
		case CItemObject::ITEM_TYPE::ITEM_Raw_Gold:
			return  PackTexId(6, 70);
		case CItemObject::ITEM_TYPE::ITEM_Iron_Ingot:
			return  PackTexId(6, 71);
		case CItemObject::ITEM_TYPE::ITEM_Copper_Ingot:
			return  PackTexId(6, 72);
		case CItemObject::ITEM_TYPE::ITEM_Gold_Ingot:
			return  PackTexId(6, 73);
		case CItemObject::ITEM_TYPE::ITEM_Netherite_Ingot:
			return  PackTexId(6, 77);
		case CItemObject::ITEM_TYPE::ITEM_Diamond:
			return  PackTexId(6, 74);
		case CItemObject::ITEM_TYPE::ITEM_NetheriteScrap:
			return  PackTexId(6, 75);

		case CItemObject::ITEM_TYPE::ITEM_Charcoal:
			return  PackTexId(6, 76);


		case Engine::CItemObject::ITEM_TYPE::ITEM_Coal:
			return  PackTexId(6, 8);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Stick:
			return  PackTexId(6, 9);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Torch:
			return  PackTexId(6, 10);

		case Engine::CItemObject::ITEM_TYPE::ITEM_String:
			return  PackTexId(6, 1);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flint:
			return  PackTexId(6, 78);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Feather:
			return  PackTexId(6, 79);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Empty:
			return  PackTexId(6, 80);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Water:
			return  PackTexId(6, 81);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Lava:
			return  PackTexId(6, 82);

		case Engine::CItemObject::ITEM_TYPE::ITEM_FlintAndSteel:
			return  PackTexId(6, 83);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Gunpowder:
			return  PackTexId(6, 84);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Beef:
			return  PackTexId(6, 4);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Chicken:
			return  PackTexId(6, 3);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Porkchop:
			return  PackTexId(6, 2);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Mutton:
			return  PackTexId(6, 5);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Beef_Cooked:
			return  PackTexId(6, 85);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Chicken_Cooked:
			return  PackTexId(6, 86);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Porkchop_Cooked:
			return  PackTexId(6, 87);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Mutton_Cooked:
			return  PackTexId(6, 88);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Apple:
			return  PackTexId(6, 89);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Wheat:
			return  PackTexId(6, 90);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Bread:
			return  PackTexId(6, 91);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Allium:
			return  PackTexId(6, 92);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Blue_Orchid:
			return  PackTexId(6, 93);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_CornFlower:
			return  PackTexId(6, 94);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Dandelion:
			return  PackTexId(6, 95);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Houstonia:
			return  PackTexId(6, 96);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Lily_Of_The_Valley:
			return  PackTexId(6, 97);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Oxeye_Daisy:
			return  PackTexId(6, 98);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Paeonia:
			return  PackTexId(6, 99);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Rose:
			return  PackTexId(6, 100);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Rose_Blue:
			return  PackTexId(6, 101);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Tulip_Orange:
			return  PackTexId(6, 102);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Tulip_Pink:
			return  PackTexId(6, 103);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_Tulip_Red:
			return  PackTexId(6, 104);
		case Engine::CItemObject::ITEM_TYPE::ITEM_Flower_WIther_Rose:
			return  PackTexId(6, 105);

		case Engine::CItemObject::ITEM_TYPE::ITEM_Seeds_Wheat:
			return PackTexId(6, 106);

		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_0:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_2:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_4:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_6:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_8:
		case CItemObject::ITEM_TYPE::ITEM_Bless_Lv_10:
			return PackTexId(6, 107);
		}
		return 0;
	}


	static void SpawnDropItemObject(const CItemObject::ItemInfo& info, _float3 pos, _float3 vel);

	static void DestoryBlockAfterProcess(const CItemObject::ItemInfo& info, const XMINT3& wbLocatoin);
	static _bool DestoryBlockItemConverter(CItemObject::ItemInfo& info);
	
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

protected:
	std::optional<ItemInfo> m_ItemInfo{};

protected:
	explicit CItemObject();
	~CItemObject() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END