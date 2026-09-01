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
			case CBlock3::TYPE::AIR: return L"공기";
			case CBlock3::TYPE::STONE: return L"돌";
			case CBlock3::TYPE::BEDROCK: return L"기반암";
			case CBlock3::TYPE::GRASS: return L"잔디";
			case CBlock3::TYPE::STONE_REDSTONE_ORE: return L"레드스톤 광석";
			case CBlock3::TYPE::STONE_LAPIS_ORE: return L"청금석 광석";
			case CBlock3::TYPE::STONE_IRON_ORE: return L"철 광석";
			case CBlock3::TYPE::STONE_GOLD_ORE: return L"금 광석";
			case CBlock3::TYPE::STONE_EMERALD_ORE: return L"에메랄드 광석";
			case CBlock3::TYPE::STONE_DIAMOND_ORE: return L"다이아몬드 광석";
			case CBlock3::TYPE::STONE_COPPER_ORE: return L"구리 광석";
			case CBlock3::TYPE::STONE_COAL_ORE: return L"석탄 광석";
			case CBlock3::TYPE::DEEPSLATE: return L"심층암";
			case CBlock3::TYPE::DEEPSLATE_COAL_ORE: return L"심층암 석탄 광석";
			case CBlock3::TYPE::DEEPSLATE_COPPER_ORE: return L"심층암 구리 광석";
			case CBlock3::TYPE::DEEPSLATE_DIAMOND_ORE: return L"심층암 다이아몬드 광석";
			case CBlock3::TYPE::DEEPSLATE_EMERALD_ORE: return L"심층암 에메랄드 광석";
			case CBlock3::TYPE::DEEPSLATE_GOLD_ORE: return L"심층암 금 광석";
			case CBlock3::TYPE::DEEPSLATE_IRON_ORE: return L"심층암 철 광석";
			case CBlock3::TYPE::DEEPSLATE_LAPIS_ORE: return L"심층암 청금석 광석";
			case CBlock3::TYPE::DEEPSLATE_REDSTONE_ORE: return L"심층암 레드스톤 광석";
			case CBlock3::TYPE::DIRT: return L"흙";
			case CBlock3::TYPE::SAND: return L"모래";
			case CBlock3::TYPE::WATER_PLACE_HOLDER: return L"물(임시)";
			case CBlock3::TYPE::TORCH_ON: return L"횃불";
			case CBlock3::TYPE::REDSTONE_TORCH_OFF: return L"레드스톤 횃불(꺼짐)";
			case CBlock3::TYPE::REDSTONE_TORCH_ON: return L"레드스톤 횃불";
			case CBlock3::TYPE::LOG_CHERRY: return L"벚나무 원목";
			case CBlock3::TYPE::LOG_OAK: return L"참나무 원목";
			case CBlock3::TYPE::LOG_BIRCH: return L"자작나무 원목";
			case CBlock3::TYPE::LOG_ACACIA: return L"아카시아 원목";
			case CBlock3::TYPE::PLANK_CHERRY: return L"벚나무 판자";
			case CBlock3::TYPE::PLANK_OAK: return L"참나무 판자";
			case CBlock3::TYPE::PLANK_BIRCH: return L"자작나무 판자";
			case CBlock3::TYPE::PLANK_ACACIA: return L"아카시아 판자";
			case CBlock3::TYPE::LEAVES_CHERRY: return L"벚나무 잎";
			case CBlock3::TYPE::LEAVES_OAK: return L"참나무 잎";
			case CBlock3::TYPE::LEAVES_BIRCH: return L"자작나무 잎";
			case CBlock3::TYPE::LEAVES_ACACIA: return L"아카시아 잎";
			case CBlock3::TYPE::FLOWER_ALLIUM: return L"알리움";
			case CBlock3::TYPE::FLOWER_BLUE_ORCHID: return L"파란 난초";
			case CBlock3::TYPE::FLOWER_CORNFLOWER: return L"수레국화";
			case CBlock3::TYPE::FLOWER_DANDELION: return L"민들레";
			case CBlock3::TYPE::FLOWER_HOUSTONIA: return L"휴스토니아";
			case CBlock3::TYPE::FLOWER_LILY_OF_THE_VALLEY: return L"은방울꽃";
			case CBlock3::TYPE::FLOWER_OXEYE_DAISY: return L"옥스아이 데이지";
			case CBlock3::TYPE::FLOWER_PAEONIA: return L"작약";
			case CBlock3::TYPE::FLOWER_ROSE: return L"장미";
			case CBlock3::TYPE::FLOWER_ROSE_BLUE: return L"파란 장미";
			case CBlock3::TYPE::FLOWER_TULIP_ORANGE: return L"주황 튤립";
			case CBlock3::TYPE::FLOWER_TULIP_PINK: return L"분홍 튤립";
			case CBlock3::TYPE::FLOWER_TULIP_RED: return L"빨간 튤립";
			case CBlock3::TYPE::FLOWER_WITHER_ROSE: return L"위더 장미";
			case CBlock3::TYPE::FIJI_SHORT_DRY_GRASS: return L"마른 풀";
			case CBlock3::TYPE::FIJI_SHORT_GRASS: return L"짧은 풀";
			case CBlock3::TYPE::FIJI_TALL_DRY_GRASS: return L"키 큰 마른 풀";
			case CBlock3::TYPE::FIJI_TALL_GRASS_BOTTOM: return L"키 큰 풀(하)";
			case CBlock3::TYPE::FIJI_TALL_GRASS_TOP: return L"키 큰 풀(상)";
			case CBlock3::TYPE::SLAP_PLANK_CHERRY: return L"벚나무 반 블록";
			case CBlock3::TYPE::SLAP_PLANK_OAK: return L"참나무 반 블록";
			case CBlock3::TYPE::SLAP_PLANK_BIRCH: return L"자작나무 반 블록";
			case CBlock3::TYPE::SLAP_PLANK_ACACIA: return L"아카시아 반 블록";
			case CBlock3::TYPE::STAIR_PLANK_CHERRY: return L"벚나무 계단";
			case CBlock3::TYPE::STAIR_PLANK_OAK: return L"참나무 계단";
			case CBlock3::TYPE::STAIR_PLANK_BIRCH: return L"자작나무 계단";
			case CBlock3::TYPE::STAIR_PLANK_ACACIA: return L"아카시아 계단";
			case CBlock3::TYPE::OBSIDIAN: return L"흑요석";
			case CBlock3::TYPE::CHEST: return L"상자";
			case CBlock3::TYPE::CRAFTING_TABLE: return L"제작대";
			case CBlock3::TYPE::FURNACE: return L"화로";
			case CBlock3::TYPE::TNT: return L"TNT";
			case CBlock3::TYPE::ENCHANTING_TABLE: return L"마법 부여대";
			case CBlock3::TYPE::BOOKSHELF: return L"책장";
			case CBlock3::TYPE::LADDER: return L"사다리";
			case CBlock3::TYPE::COBBLESTONE: return L"조약돌";
			case CBlock3::TYPE::COBBLED_DEEPSLATE: return L"심층암 조약돌";
			case CBlock3::TYPE::WATER_STILL: return L"물";
			case CBlock3::TYPE::WATER_FLOW: return L"흐르는 물";
			case CBlock3::TYPE::LAVA_PLACE_HOLDER: return L"용암(임시)";
			case CBlock3::TYPE::LAVA_STILL: return L"용암";
			case CBlock3::TYPE::LAVA_FLOW: return L"흐르는 용암";
			}
		}
		else
		{
			switch (info.eItemType)
			{
			case ITEM_TYPE::ITEM_WoodPickaxe: return L"나무 곡괭이";
			case ITEM_TYPE::ITEM_StonePickaxe: return L"돌 곡괭이";
			case ITEM_TYPE::ITEM_CopperPickaxe: return L"구리 곡괭이";
			case ITEM_TYPE::ITEM_IronPickaxe: return L"철 곡괭이";
			case ITEM_TYPE::ITEM_GoldPickaxe: return L"금 곡괭이";
			case ITEM_TYPE::ITEM_DiamondPickaxe: return L"다이아몬드 곡괭이";
			case ITEM_TYPE::ITEM_NetheritePickaxe: return L"네더라이트 곡괭이";
			case ITEM_TYPE::ITEM_WoodAxe: return L"나무 도끼";
			case ITEM_TYPE::ITEM_StoneAxe: return L"돌 도끼";
			case ITEM_TYPE::ITEM_CopperAxe: return L"구리 도끼";
			case ITEM_TYPE::ITEM_IronAxe: return L"철 도끼";
			case ITEM_TYPE::ITEM_GoldAxe: return L"금 도끼";
			case ITEM_TYPE::ITEM_DiamondAxe: return L"다이아몬드 도끼";
			case ITEM_TYPE::ITEM_NetheriteAxe: return L"네더라이트 도끼";
			case ITEM_TYPE::ITEM_WoodShovel: return L"나무 삽";
			case ITEM_TYPE::ITEM_StoneShovel: return L"돌 삽";
			case ITEM_TYPE::ITEM_CopperShovel: return L"구리 삽";
			case ITEM_TYPE::ITEM_IronShovel: return L"철 삽";
			case ITEM_TYPE::ITEM_GoldShovel: return L"금 삽";
			case ITEM_TYPE::ITEM_DiamondShovel: return L"다이아몬드 삽";
			case ITEM_TYPE::ITEM_NetheriteShovel: return L"네더라이트 삽";
			case ITEM_TYPE::ITEM_WoodHoe: return L"나무 괭이";
			case ITEM_TYPE::ITEM_StoneHoe: return L"돌 괭이";
			case ITEM_TYPE::ITEM_CopperHoe: return L"구리 괭이";
			case ITEM_TYPE::ITEM_IronHoe: return L"철 괭이";
			case ITEM_TYPE::ITEM_GoldHoe: return L"금 괭이";
			case ITEM_TYPE::ITEM_DiamondHoe: return L"다이아몬드 괭이";
			case ITEM_TYPE::ITEM_NetheriteHoe: return L"네더라이트 괭이";
			case ITEM_TYPE::ITEM_WoodSword: return L"나무 검";
			case ITEM_TYPE::ITEM_StoneSword: return L"돌 검";
			case ITEM_TYPE::ITEM_CopperSword: return L"구리 검";
			case ITEM_TYPE::ITEM_IronSword: return L"철 검";
			case ITEM_TYPE::ITEM_GoldSword: return L"금 검";
			case ITEM_TYPE::ITEM_DiamondSword: return L"다이아몬드 검";
			case ITEM_TYPE::ITEM_NetheriteSword: return L"네더라이트 검";
			case ITEM_TYPE::ITEM_CopperHelmet: return L"구리 헬멧";
			case ITEM_TYPE::ITEM_IronHelmet: return L"철 헬멧";
			case ITEM_TYPE::ITEM_GoldHelmet: return L"금 헬멧";
			case ITEM_TYPE::ITEM_DiamondHelmet: return L"다이아몬드 헬멧";
			case ITEM_TYPE::ITEM_NetheriteHelmet: return L"네더라이트 헬멧";
			case ITEM_TYPE::ITEM_CopperChestplate: return L"구리 흉갑";
			case ITEM_TYPE::ITEM_IronChestplate: return L"철 흉갑";
			case ITEM_TYPE::ITEM_GoldChestplate: return L"금 흉갑";
			case ITEM_TYPE::ITEM_DiamondChestplate: return L"다이아몬드 흉갑";
			case ITEM_TYPE::ITEM_NetheriteChestplate: return L"네더라이트 흉갑";
			case ITEM_TYPE::ITEM_CopperLeggings: return L"구리 각반";
			case ITEM_TYPE::ITEM_IronLeggings: return L"철 각반";
			case ITEM_TYPE::ITEM_GoldLeggings: return L"금 각반";
			case ITEM_TYPE::ITEM_DiamondLeggings: return L"다이아몬드 각반";
			case ITEM_TYPE::ITEM_NetheriteLeggings: return L"네더라이트 각반";
			case ITEM_TYPE::ITEM_CopperBoots: return L"구리 부츠";
			case ITEM_TYPE::ITEM_IronBoots: return L"철 부츠";
			case ITEM_TYPE::ITEM_GoldBoots: return L"금 부츠";
			case ITEM_TYPE::ITEM_DiamondBoots: return L"다이아몬드 부츠";
			case ITEM_TYPE::ITEM_NetheriteBoots: return L"네더라이트 부츠";
			case ITEM_TYPE::ITEM_Bow_Standby: return L"활";
			case ITEM_TYPE::ITEM_Bow_Pulling_0: return L"활";
			case ITEM_TYPE::ITEM_Bow_Pulling_1: return L"활";
			case ITEM_TYPE::ITEM_Bow_Pulling_2: return L"활";
			case ITEM_TYPE::ITEM_Arrow: return L"화살";
			case ITEM_TYPE::ITEM_Raw_Iron: return L"철 원석";
			case ITEM_TYPE::ITEM_Raw_Copper: return L"구리 원석";
			case ITEM_TYPE::ITEM_Raw_Gold: return L"금 원석";
			case ITEM_TYPE::ITEM_Iron_Ingot: return L"철 주괴";
			case ITEM_TYPE::ITEM_Copper_Ingot: return L"구리 주괴";
			case ITEM_TYPE::ITEM_Gold_Ingot: return L"금 주괴";
			case ITEM_TYPE::ITEM_Netherite_Ingot: return L"네더라이트 주괴";
			case ITEM_TYPE::ITEM_Diamond: return L"다이아몬드";
			case ITEM_TYPE::ITEM_NetheriteScrap: return L"네더라이트 파편";
			case ITEM_TYPE::ITEM_Charcoal: return L"숯";
			case ITEM_TYPE::ITEM_Coal: return L"석탄";
			case ITEM_TYPE::ITEM_Stick: return L"막대기";
			case ITEM_TYPE::ITEM_Torch: return L"횃불";
			case ITEM_TYPE::ITEM_String: return L"실";
			case ITEM_TYPE::ITEM_Feather: return L"깃털";
			case ITEM_TYPE::ITEM_Flint: return L"부싯돌";
			case ITEM_TYPE::ITEM_Bucket_Empty: return L"양동이";
			case ITEM_TYPE::ITEM_Bucket_Water: return L"물 양동이";
			case ITEM_TYPE::ITEM_Bucket_Lava: return L"용암 양동이";
			case ITEM_TYPE::ITEM_FlintAndSteel: return L"라이터";
			case ITEM_TYPE::ITEM_Gunpowder: return L"화약";
			case ITEM_TYPE::ITEM_Raw_Beef: return L"날고기";
			case ITEM_TYPE::ITEM_Raw_Chicken: return L"생닭";
			case ITEM_TYPE::ITEM_Raw_Porkchop: return L"날돼지고기";
			case ITEM_TYPE::ITEM_Raw_Mutton: return L"날양고기";
			case ITEM_TYPE::ITEM_Beef_Cooked: return L"스테이크";
			case ITEM_TYPE::ITEM_Chicken_Cooked: return L"구운 닭";
			case ITEM_TYPE::ITEM_Porkchop_Cooked: return L"구운 돼지고기";
			case ITEM_TYPE::ITEM_Mutton_Cooked: return L"구운 양고기";
			case ITEM_TYPE::ITEM_Apple: return L"사과";
			case ITEM_TYPE::ITEM_Wheat: return L"밀";
			case ITEM_TYPE::ITEM_Bread: return L"빵";
			case ITEM_TYPE::ITEM_Flower_Allium: return L"알리움";
			case ITEM_TYPE::ITEM_Flower_Blue_Orchid: return L"파란 난초";
			case ITEM_TYPE::ITEM_Flower_CornFlower: return L"수레국화";
			case ITEM_TYPE::ITEM_Flower_Dandelion: return L"민들레";
			case ITEM_TYPE::ITEM_Flower_Houstonia: return L"휴스토니아";
			case ITEM_TYPE::ITEM_Flower_Lily_Of_The_Valley: return L"은방울꽃";
			case ITEM_TYPE::ITEM_Flower_Oxeye_Daisy: return L"옥스아이 데이지";
			case ITEM_TYPE::ITEM_Flower_Paeonia: return L"작약";
			case ITEM_TYPE::ITEM_Flower_Rose: return L"장미";
			case ITEM_TYPE::ITEM_Flower_Rose_Blue: return L"파란 장미";
			case ITEM_TYPE::ITEM_Flower_Tulip_Orange: return L"주황 튤립";
			case ITEM_TYPE::ITEM_Flower_Tulip_Pink: return L"분홍 튤립";
			case ITEM_TYPE::ITEM_Flower_Tulip_Red: return L"빨간 튤립";
			case ITEM_TYPE::ITEM_Flower_WIther_Rose: return L"위더 장미";
			case ITEM_TYPE::ITEM_Seeds_Wheat: return L"밀 씨앗";
			case ITEM_TYPE::ITEM_Bless_Lv_0: return L"토템 LV 0";
			case ITEM_TYPE::ITEM_Bless_Lv_2: return L"토템 LV 2\n\n무한의화살: 화살 소비 없음";
			case ITEM_TYPE::ITEM_Bless_Lv_4: return L"토템 LV 4\n\n무한의화살: 화살 소비 없음\n폭풍의 시: 공격 속도 증가";
			case ITEM_TYPE::ITEM_Bless_Lv_6: return L"토템 LV 6\n\n무한의화살: 화살 소비 없음\n에로우 밤: 화살 폭발";
			case ITEM_TYPE::ITEM_Bless_Lv_8: return L"토템 LV 8\n\n무한의화살: 화살 소비 없음\n폭풍의 시: 공격 속도 증가\n에로우 밤: 화살 폭발";
			case ITEM_TYPE::ITEM_Bless_Lv_10: return L"토템 LV 10\n\n무한의화살: 화살 소비 없음\n폭풍의 시: 공격 속도 증가\n에로우 밤: 화살 폭발\nTHE GOD: 비행 가능";
			}
		}
		return L"알 수 없음";
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