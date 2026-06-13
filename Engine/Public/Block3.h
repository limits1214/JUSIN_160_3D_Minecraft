#pragma once
#include "Engine_Defines.h"
NS_BEGIN(Engine)
class ENGINE_DLL CBlock3
{
public:
	enum class TYPE : uint8_t
	{
		AIR = 0,
		STONE,
		BEDROCK,
		GRASS,
		STONE_REDSTONE_ORE,
		STONE_LAPIS_ORE,
		STONE_IRON_ORE,
		STONE_GOLD_ORE,
		STONE_EMERALD_ORE,
		STONE_DIAMOND_ORE,
		STONE_COPPER_ORE,
		STONE_COAL_ORE,
		DEEPSLATE,
		DEEPSLATE_COAL_ORE,
		DEEPSLATE_COPPER_ORE,
		DEEPSLATE_DIAMOND_ORE,
		DEEPSLATE_EMERALD_ORE,
		DEEPSLATE_GOLD_ORE,
		DEEPSLATE_IRON_ORE,
		DEEPSLATE_LAPIS_ORE,
		DEEPSLATE_REDSTONE_ORE,
		DIRT,
		SAND,
		WATER_PLACE_HOLDER,

		TORCH_ON,
		REDSTONE_TORCH_OFF,
		REDSTONE_TORCH_ON,

		LOG_CHERRY,
		LOG_OAK,
		LOG_BIRCH,
		LOG_ACACIA,
		PLANK_CHERRY,
		PLANK_OAK,
		PLANK_BIRCH,
		PLANK_ACACIA,
		LEAVES_CHERRY,
		LEAVES_OAK,
		LEAVES_BIRCH,
		LEAVES_ACACIA,
		FLOWER_ALLIUM,
		FLOWER_BLUE_ORCHID,
		FLOWER_CORNFLOWER,
		FLOWER_DANDELION,
		FLOWER_HOUSTONIA,
		FLOWER_LILY_OF_THE_VALLEY,
		FLOWER_OXEYE_DAISY,
		FLOWER_PAEONIA,
		FLOWER_ROSE,
		FLOWER_ROSE_BLUE,
		FLOWER_TULIP_ORANGE,
		FLOWER_TULIP_PINK,
		FLOWER_TULIP_RED,
		FLOWER_WITHER_ROSE,
		FIJI_SHORT_DRY_GRASS,
		FIJI_SHORT_GRASS,
		FIJI_TALL_DRY_GRASS,
		FIJI_TALL_GRASS_BOTTOM,
		FIJI_TALL_GRASS_TOP,

		SLAP_PLANK_CHERRY,
		SLAP_PLANK_OAK,
		SLAP_PLANK_BIRCH,
		SLAP_PLANK_ACACIA,

		STAIR_PLANK_CHERRY,
		STAIR_PLANK_OAK,
		STAIR_PLANK_BIRCH,
		STAIR_PLANK_ACACIA,
		OBSIDIAN,
		CHEST,
		CRAFTING_TABLE,
		FURNACE,
		TNT,
		ENCHANTING_TABLE,
		BOOKSHELF,
		LADDER,
		COBBLESTONE,
		COBBLED_DEEPSLATE,
		WATER_STILL,
		WATER_FLOW,

		LAVA_PLACE_HOLDER,
		LAVA_STILL,
		LAVA_FLOW,
		END
	};

	enum class TEX_TYPE : uint8_t
	{
		DIRT = 0,
		STONE,
		SAND,
		BEDROCK,
		STONE_REDSTONE_ORE,
		STONE_LAPIS_ORE,
		STONE_IRON_ORE,
		STONE_GOLD_ORE,
		STONE_EMERALD_ORE,
		STONE_DIAMOND_ORE,
		STONE_COPPER_ORE,
		STONE_COAL_ORE,
		DEEPSLATE,
		DEEPSLATE_COAL_ORE,
		DEEPSLATE_COPPER_ORE,
		DEEPSLATE_DIAMOND_ORE,
		DEEPSLATE_EMERALD_ORE,
		DEEPSLATE_GOLD_ORE,
		DEEPSLATE_IRON_ORE,
		DEEPSLATE_LAPIS_ORE,
		DEEPSLATE_REDSTONE_ORE,

		GRASS_TOP_CARRIED,
		GRASS_SIDE_CARRIED,
		GRASS_SIDE_SNOW,
		WATER_PLACE_HOLDER,

		TORCH_ON,
		REDSTONE_TORCH_OFF,
		REDSTONE_TORCH_ON,

		LOG_CHERRY_TOP,
		LOG_CHERRY_SIDE,
		LOG_OAK_TOP,
		LOG_OAK_SIDE,
		LOG_BIRCH_TOP,
		LOG_BIRCH_SIDE,
		LOG_ACACIA_TOP,
		LOG_ACACIA_SIDE,
		PLANK_CHERRY,
		PLANK_OAK,
		PLANK_BIRCH,
		PLANK_ACACIA,
		LEAVES_CHERRY,
		LEAVES_OAK,
		LEAVES_BIRCH,
		LEAVES_ACACIA,
		FLOWER_ALLIUM,
		FLOWER_BLUE_ORCHID,
		FLOWER_CORNFLOWER,
		FLOWER_DANDELION,
		FLOWER_HOUSTONIA,
		FLOWER_LILY_OF_THE_VALLEY,
		FLOWER_OXEYE_DAISY,
		FLOWER_PAEONIA,
		FLOWER_ROSE,
		FLOWER_ROSE_BLUE,
		FLOWER_TULIP_ORANGE,
		FLOWER_TULIP_PINK,
		FLOWER_TULIP_RED,
		FLOWER_WITHER_ROSE,
		FIJI_SHORT_DRY_GRASS,
		FIJI_SHORT_GRASS,
		FIJI_TALL_DRY_GRASS,
		FIJI_TALL_GRASS_BOTTOM,
		FIJI_TALL_GRASS_TOP,
		STRIPPED_LOG_CHERRY_TOP,
		STRIPPED_LOG_CHERRY_SIDE,
		STRIPPED_LOG_OAK_TOP,
		STRIPPED_LOG_OAK_SIDE,
		STRIPPED_LOG_BIRCH_TOP,
		STRIPPED_LOG_BIRCH_SIDE,
		STRIPPED_LOG_ACACIA_TOP,
		STRIPPED_LOG_ACACIA_SIDE,
		OBSIDIAN,
		CHEST_FRONT,
		CHEST_SIDE,
		CHEST_TOP,
		CRAFTING_TABLE_FRONT,
		CRAFTING_TABLE_SIDE,
		CRAFTING_TABLE_TOP,
		FURNACE_FRONT_OFF,
		FURNACE_FRONT_ON,
		FURNACE_SIDE,
		FURNACE_TOP,
		TNT_TOP,
		TNT_SIDE,
		TNT_BOTTOM,
		ENCHANTING_TABLE_TOP,
		ENCHANTING_TABLE_SIDE,
		ENCHANTING_TABLE_BOTTOM,
		BOOKSHELF,
		LADDER,
		COBBLESTONE,
		COBBLED_DEEPSLATE,
		LAVA_PLACE_HOLDER,
		END
	};

	enum class GEO_TYPE : uint8_t {
		CUBE,        // 흙, 돌, 나뭇잎 등 일반적인 1x1x1 6면체
		CROSS,       // 잔디, 꽃, 묘목 등 사각형 2장을 십자로 교차하는 형태
		TORCH,       // 횃불처럼 중심에 수축된 기둥 형태
		CROSS_FLAT,  // 수평으로 바닥에 누운 풀이나 카펫 형태 (필요시)
		SLAP,
		STAIR,
		ENCHANTING_TABLE,
		LADDER,
		WATER,
		//LAVA,
		END
	};


public:
	CBlock3() = default;
	CBlock3(TYPE type) : m_eType{ type } {};

	static inline GEO_TYPE GetGeoType(TYPE e);
	static inline uint32_t GetBaseColor(CBlock3::TYPE eType);
	static inline TEX_TYPE GetTexType(TYPE blockType, FACE_DIR faceDir, uint8_t iFlag = 0);


	static inline _bool IsFlower(TYPE eType);
	static inline TYPE GetRandomTypeFlower();

	static inline _bool IsNeedAlphaTest(TYPE e);
	static inline std::pair<_float3, _float3> GetOutlineExtents(TYPE e);
	static inline uint8_t GetBlockLightByType(TYPE e);

	// 불투명한가?
	//inline _bool IsOpaque() const;
	;
	static inline _bool IsAOOclluder(TYPE eType);
	static inline _bool IsOpaque(TYPE eType);
	static inline _bool IsWater(TYPE eType);
	//inline  _bool IsWater() const;

	TYPE GetType() const { return m_eType; }
	void SetType(TYPE e) { m_eType = e; }

	uint8_t GetLight() const { return m_iLight; }
	uint8_t GetSkyLight() const { return (m_iLight >> 4) & 0xF; }
	uint8_t GetBlockLight() const { return m_iLight & 0xF; }
	void SetSkyLight(uint8_t v) { m_iLight = (m_iLight & 0x0F) | (v << 4); }
	void SetBlockLight(uint8_t v) { m_iLight = (m_iLight & 0xF0) | v; }

	uint8_t GetFlag() const { return m_iFlag; }
	void SetFlag(uint8_t iFlag) { m_iFlag = iFlag; }
private:
	TYPE m_eType{ TYPE::AIR };
	//햇빛 15 → 14 → 13 → 12 → ... → 0 (완전 어둠)
	uint8_t m_iLight{}; // 상위 4bit: skylight, 하위4bit: blocklight
	uint8_t  m_iFlag{}; // etc
};

NS_END

NS_BEGIN(Engine)

inline _bool CBlock3::IsAOOclluder(TYPE eType)
{
	// 1. 공기(AIR)는 당연히 AO를 만들지 않음
	if (eType == TYPE::AIR) return false;

	// 2. 기본적으로 흙, 돌 같은 완전 불투명(Opaque) 블록은 무조건 AO를 만듦
	if (IsOpaque(eType)) return true;

	// 3. 반투명(Translucent) 블록인 물, 유리는 '빛을 차단하지 않으므로' AO 생성에서 제외(false)
	//if (IsTranslucent(eType)) return false;

	// 4. 나뭇잎(AlphaTest)의 경우: 
	// - 주변 흙을 시커멓게 만드는 게 싫다면 -> return false;
	// - 나뭇잎끼리 뭉쳤을 때 입체감을 주고 싶다면 -> return true;
	// (여기서는 나뭇잎도 고체이므로 true를 준다고 가정)
	if (IsNeedAlphaTest(eType)) return false;

	return false;
}

inline  _bool CBlock3::IsOpaque(TYPE eType)
{
	if (IsFlower(eType))
	{
		return false;
	}

	if (IsWater(eType))
	{
		return false;
	}

	switch (eType)
	{
	case TYPE::AIR:
	case TYPE::TORCH_ON:
	case TYPE::FIJI_SHORT_GRASS:
	case TYPE::FIJI_TALL_GRASS_BOTTOM:
	case TYPE::FIJI_TALL_GRASS_TOP:
	case TYPE::FLOWER_OXEYE_DAISY:
		return false;

	case TYPE::LEAVES_CHERRY:
	case TYPE::LEAVES_OAK:
	case TYPE::LEAVES_BIRCH:
	case TYPE::LEAVES_ACACIA:
		return false;
	default:
		return true;
	}
}

inline  _bool CBlock3::IsWater(TYPE e)
{
	switch (e)
	{
	case  TYPE::WATER_PLACE_HOLDER:
	case TYPE::WATER_FLOW:
	case TYPE::WATER_STILL:

	case TYPE::LAVA_PLACE_HOLDER:
	case TYPE::LAVA_FLOW:
	case TYPE::LAVA_STILL:
		return true;
	}

	return false;
}

inline uint8_t CBlock3::GetBlockLightByType(TYPE e)
{
	switch (e)
	{
	case TYPE::TORCH_ON:
		return 15;

	case TYPE::LAVA_PLACE_HOLDER:
	case TYPE::LAVA_FLOW:
	case TYPE::LAVA_STILL:
		return 15;
	}

	return 0;
}

// origin, extents
inline std::pair<_float3, _float3> CBlock3::GetOutlineExtents(TYPE e)
{
	switch (e)
	{
	case TYPE::TORCH_ON:
		return { {0.f, -0.1f, 0.f},{0.2f, 0.4f, 0.2f} };
	}
	return { {0.f, 0.f, 0.f},{0.5f, 0.5f, 0.5f} };
}

inline _bool CBlock3::IsNeedAlphaTest(TYPE e)
{
	switch (e)
	{
	case TYPE::LEAVES_CHERRY:
	case TYPE::LEAVES_OAK:
	case TYPE::LEAVES_BIRCH:
	case TYPE::LEAVES_ACACIA:
	case TYPE::FLOWER_ALLIUM:
	case TYPE::FLOWER_BLUE_ORCHID:
	case TYPE::FLOWER_CORNFLOWER:
	case TYPE::FLOWER_DANDELION:
	case TYPE::FLOWER_HOUSTONIA:
	case TYPE::FLOWER_LILY_OF_THE_VALLEY:
	case TYPE::FLOWER_OXEYE_DAISY:
	case TYPE::FLOWER_PAEONIA:
	case TYPE::FLOWER_ROSE:
	case TYPE::FLOWER_ROSE_BLUE:
	case TYPE::FLOWER_TULIP_ORANGE:
	case TYPE::FLOWER_TULIP_PINK:
	case TYPE::FLOWER_TULIP_RED:
	case TYPE::FLOWER_WITHER_ROSE:
	case TYPE::FIJI_SHORT_DRY_GRASS:
	case TYPE::FIJI_SHORT_GRASS:
	case TYPE::FIJI_TALL_DRY_GRASS:
	case TYPE::FIJI_TALL_GRASS_BOTTOM:
	case TYPE::FIJI_TALL_GRASS_TOP:
		return true;
	}

	return false;
}

inline CBlock3::TEX_TYPE CBlock3::GetTexType(CBlock3::TYPE blockType, FACE_DIR faceDir, uint8_t iFlag)
{
	switch (blockType)
	{
	case TYPE::STONE:                   return TEX_TYPE::STONE;
	case TYPE::BEDROCK:                 return TEX_TYPE::BEDROCK;
	case TYPE::STONE_REDSTONE_ORE:      return TEX_TYPE::STONE_REDSTONE_ORE;
	case TYPE::STONE_LAPIS_ORE:         return TEX_TYPE::STONE_LAPIS_ORE;
	case TYPE::STONE_IRON_ORE:          return TEX_TYPE::STONE_IRON_ORE;
	case TYPE::STONE_GOLD_ORE:          return TEX_TYPE::STONE_GOLD_ORE;
	case TYPE::STONE_EMERALD_ORE:       return TEX_TYPE::STONE_EMERALD_ORE;
	case TYPE::STONE_DIAMOND_ORE:       return TEX_TYPE::STONE_DIAMOND_ORE;
	case TYPE::STONE_COPPER_ORE:        return TEX_TYPE::STONE_COPPER_ORE;
	case TYPE::STONE_COAL_ORE:          return TEX_TYPE::STONE_COAL_ORE;
	case TYPE::DEEPSLATE:               return TEX_TYPE::DEEPSLATE;
	case TYPE::DEEPSLATE_COAL_ORE:      return TEX_TYPE::DEEPSLATE_COAL_ORE;
	case TYPE::DEEPSLATE_COPPER_ORE:    return TEX_TYPE::DEEPSLATE_COPPER_ORE;
	case TYPE::DEEPSLATE_DIAMOND_ORE:   return TEX_TYPE::DEEPSLATE_DIAMOND_ORE;
	case TYPE::DEEPSLATE_EMERALD_ORE:   return TEX_TYPE::DEEPSLATE_EMERALD_ORE;
	case TYPE::DEEPSLATE_GOLD_ORE:      return TEX_TYPE::DEEPSLATE_GOLD_ORE;
	case TYPE::DEEPSLATE_IRON_ORE:      return TEX_TYPE::DEEPSLATE_IRON_ORE;
	case TYPE::DEEPSLATE_LAPIS_ORE:     return TEX_TYPE::DEEPSLATE_LAPIS_ORE;
	case TYPE::DEEPSLATE_REDSTONE_ORE:  return TEX_TYPE::DEEPSLATE_REDSTONE_ORE;
	case TYPE::DIRT:                    return TEX_TYPE::DIRT;
	case TYPE::SAND:                    return TEX_TYPE::SAND;
	case TYPE::WATER_PLACE_HOLDER:      return TEX_TYPE::WATER_PLACE_HOLDER;
	case TYPE::TORCH_ON:                return TEX_TYPE::TORCH_ON;
	case TYPE::REDSTONE_TORCH_OFF:      return TEX_TYPE::REDSTONE_TORCH_OFF;
	case TYPE::REDSTONE_TORCH_ON:       return TEX_TYPE::REDSTONE_TORCH_ON;
	case TYPE::PLANK_CHERRY:            return TEX_TYPE::PLANK_CHERRY;
	case TYPE::PLANK_OAK:               return TEX_TYPE::PLANK_OAK;
	case TYPE::PLANK_BIRCH:             return TEX_TYPE::PLANK_BIRCH;
	case TYPE::PLANK_ACACIA:            return TEX_TYPE::PLANK_ACACIA;
	case TYPE::LEAVES_CHERRY:           return TEX_TYPE::LEAVES_CHERRY;
	case TYPE::LEAVES_OAK:              return TEX_TYPE::LEAVES_OAK;
	case TYPE::LEAVES_BIRCH:            return TEX_TYPE::LEAVES_BIRCH;
	case TYPE::LEAVES_ACACIA:           return TEX_TYPE::LEAVES_ACACIA;
	case TYPE::FLOWER_ALLIUM:           return TEX_TYPE::FLOWER_ALLIUM;
	case TYPE::FLOWER_BLUE_ORCHID:      return TEX_TYPE::FLOWER_BLUE_ORCHID;
	case TYPE::FLOWER_CORNFLOWER:       return TEX_TYPE::FLOWER_CORNFLOWER;
	case TYPE::FLOWER_DANDELION:        return TEX_TYPE::FLOWER_DANDELION;
	case TYPE::FLOWER_HOUSTONIA:        return TEX_TYPE::FLOWER_HOUSTONIA;
	case TYPE::FLOWER_LILY_OF_THE_VALLEY: return TEX_TYPE::FLOWER_LILY_OF_THE_VALLEY;
	case TYPE::FLOWER_OXEYE_DAISY:      return TEX_TYPE::FLOWER_OXEYE_DAISY;
	case TYPE::FLOWER_PAEONIA:          return TEX_TYPE::FLOWER_PAEONIA;
	case TYPE::FLOWER_ROSE:             return TEX_TYPE::FLOWER_ROSE;
	case TYPE::FLOWER_ROSE_BLUE:        return TEX_TYPE::FLOWER_ROSE_BLUE;
	case TYPE::FLOWER_TULIP_ORANGE:     return TEX_TYPE::FLOWER_TULIP_ORANGE;
	case TYPE::FLOWER_TULIP_PINK:       return TEX_TYPE::FLOWER_TULIP_PINK;
	case TYPE::FLOWER_TULIP_RED:        return TEX_TYPE::FLOWER_TULIP_RED;
	case TYPE::FLOWER_WITHER_ROSE:      return TEX_TYPE::FLOWER_WITHER_ROSE;
	case TYPE::FIJI_SHORT_DRY_GRASS:    return TEX_TYPE::FIJI_SHORT_DRY_GRASS;
	case TYPE::FIJI_SHORT_GRASS:        return TEX_TYPE::FIJI_SHORT_GRASS;
	case TYPE::FIJI_TALL_DRY_GRASS:     return TEX_TYPE::FIJI_TALL_DRY_GRASS;
	case TYPE::FIJI_TALL_GRASS_BOTTOM:  return TEX_TYPE::FIJI_TALL_GRASS_BOTTOM;
	case TYPE::FIJI_TALL_GRASS_TOP:     return TEX_TYPE::FIJI_TALL_GRASS_TOP;

	case TYPE::SLAP_PLANK_CHERRY:		return TEX_TYPE::PLANK_CHERRY;
	case TYPE::SLAP_PLANK_OAK:			return TEX_TYPE::PLANK_OAK;
	case TYPE::SLAP_PLANK_BIRCH:		return TEX_TYPE::PLANK_BIRCH;
	case TYPE::SLAP_PLANK_ACACIA:		return TEX_TYPE::PLANK_ACACIA;

	case TYPE::STAIR_PLANK_CHERRY:		return TEX_TYPE::PLANK_CHERRY;
	case TYPE::STAIR_PLANK_OAK:			return TEX_TYPE::PLANK_OAK;
	case TYPE::STAIR_PLANK_BIRCH:		return TEX_TYPE::PLANK_BIRCH;
	case TYPE::STAIR_PLANK_ACACIA:		return TEX_TYPE::PLANK_ACACIA;

	case TYPE::OBSIDIAN:				return TEX_TYPE::OBSIDIAN;
	case TYPE::LADDER:					return TEX_TYPE::LADDER;

	case TYPE::COBBLESTONE:				return TEX_TYPE::COBBLESTONE;
	case TYPE::COBBLED_DEEPSLATE:		return TEX_TYPE::COBBLED_DEEPSLATE;

	case TYPE::WATER_FLOW:				return static_cast<TEX_TYPE>(13);
	case TYPE::WATER_STILL:				return static_cast<TEX_TYPE>(14);

	case TYPE::LAVA_PLACE_HOLDER:		return TEX_TYPE::LAVA_PLACE_HOLDER;
	case TYPE::LAVA_FLOW:				return static_cast<TEX_TYPE>(15);
	case TYPE::LAVA_STILL:				return static_cast<TEX_TYPE>(16);

	case TYPE::GRASS:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
			return TEX_TYPE::GRASS_TOP_CARRIED;
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::DIRT;
		default:
			return TEX_TYPE::GRASS_SIDE_CARRIED;
		}

	case TYPE::LOG_CHERRY:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::LOG_CHERRY_TOP;
		default:
			return TEX_TYPE::LOG_CHERRY_SIDE;
		}

	case TYPE::LOG_OAK:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::LOG_OAK_TOP;
		default:
			return TEX_TYPE::LOG_OAK_SIDE;
		}

	case TYPE::LOG_BIRCH:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::LOG_BIRCH_TOP;
		default:
			return TEX_TYPE::LOG_BIRCH_SIDE;
		}

	case TYPE::LOG_ACACIA:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::LOG_ACACIA_TOP;
		default:
			return TEX_TYPE::LOG_ACACIA_SIDE;
		}

	case TYPE::CHEST:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::CHEST_TOP;
		case FACE_DIR::POS_X:
		case FACE_DIR::NEG_X:
			return TEX_TYPE::CHEST_SIDE;
		default:
			return TEX_TYPE::CHEST_FRONT;
		}

	case TYPE::FURNACE:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::FURNACE_TOP;
		case FACE_DIR::NEG_Z:
			return TEX_TYPE::FURNACE_FRONT_OFF;
		default:
			return TEX_TYPE::FURNACE_SIDE;
		}

	case TYPE::CRAFTING_TABLE:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::CRAFTING_TABLE_TOP;
		case FACE_DIR::NEG_Z:
			return TEX_TYPE::CRAFTING_TABLE_SIDE;
		default:
			return TEX_TYPE::CRAFTING_TABLE_FRONT;
		}

	case TYPE::TNT:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
			return TEX_TYPE::TNT_TOP;
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::TNT_BOTTOM;
		default:
			return TEX_TYPE::TNT_SIDE;
		}

	case TYPE::ENCHANTING_TABLE:
		switch (faceDir)
		{
		case FACE_DIR::POS_Y:
			return TEX_TYPE::ENCHANTING_TABLE_TOP;
		case FACE_DIR::NEG_Y:
			return TEX_TYPE::ENCHANTING_TABLE_BOTTOM;
		default:
			return TEX_TYPE::ENCHANTING_TABLE_SIDE;
		}
	}

	return TEX_TYPE::END;
}

inline uint32_t CBlock3::GetBaseColor(CBlock3::TYPE eType)
{
	switch (eType)
	{
	case TYPE::FIJI_SHORT_GRASS:
	case TYPE::FIJI_TALL_GRASS_TOP:
	case TYPE::FIJI_TALL_GRASS_BOTTOM:
		return PackRGBA(100, 200, 100, 255); // ABGR 표현 (리틀 엔디안: R=120, G=210, B=120, A=255)

	case TYPE::LEAVES_OAK:
		return PackRGBA(76, 168, 37, 255);
	case TYPE::LEAVES_BIRCH:
		return PackRGBA(128, 180, 93, 255);
	case TYPE::LEAVES_ACACIA:
		return PackRGBA(106, 144, 38, 255);
	}

	return 0xFFFFFFFF; // 순수 흰색 (원래 텍스처 색상 그대로)
}

inline CBlock3::TYPE CBlock3::GetRandomTypeFlower()
{
	// 1. 사용할 꽃 종류들을 배열(또는 std::vector)로 묶어줍니다.
	static const CBlock3::TYPE flowerTypes[] = {
		CBlock3::TYPE::FLOWER_ALLIUM,
		CBlock3::TYPE::FLOWER_BLUE_ORCHID,
		CBlock3::TYPE::FLOWER_CORNFLOWER,
		CBlock3::TYPE::FLOWER_DANDELION,
		CBlock3::TYPE::FLOWER_HOUSTONIA,
		CBlock3::TYPE::FLOWER_LILY_OF_THE_VALLEY,
		CBlock3::TYPE::FLOWER_OXEYE_DAISY,
		CBlock3::TYPE::FLOWER_PAEONIA,
		CBlock3::TYPE::FLOWER_ROSE,
		CBlock3::TYPE::FLOWER_ROSE_BLUE,
		CBlock3::TYPE::FLOWER_TULIP_ORANGE,
		CBlock3::TYPE::FLOWER_TULIP_PINK,
		CBlock3::TYPE::FLOWER_TULIP_RED,
		CBlock3::TYPE::FLOWER_WITHER_ROSE
	};

	constexpr size_t flowerCount = sizeof(flowerTypes) / sizeof(flowerTypes[0]);

	// 2. 현대적인 C++ 난수 생성기 세팅 (스레드별로 독립적인 시드 가짐 -> 멀티스레드 안전)
	static thread_local std::mt19937 generator(std::random_device{}());
	static std::uniform_int_distribution<size_t> distribution(0, flowerCount - 1);

	// 3. 무작위 인덱스를 뽑아 꽃 타입 반환
	return flowerTypes[distribution(generator)];
}

inline _bool CBlock3::IsFlower(TYPE eType)
{
	switch (eType)
	{
	case CBlock3::TYPE::FLOWER_ALLIUM:
	case CBlock3::TYPE::FLOWER_BLUE_ORCHID:
	case CBlock3::TYPE::FLOWER_CORNFLOWER:
	case CBlock3::TYPE::FLOWER_DANDELION:
	case CBlock3::TYPE::FLOWER_HOUSTONIA:
	case CBlock3::TYPE::FLOWER_LILY_OF_THE_VALLEY:
	case CBlock3::TYPE::FLOWER_OXEYE_DAISY:
	case CBlock3::TYPE::FLOWER_PAEONIA:
	case CBlock3::TYPE::FLOWER_ROSE:
	case CBlock3::TYPE::FLOWER_ROSE_BLUE:
	case CBlock3::TYPE::FLOWER_TULIP_ORANGE:
	case CBlock3::TYPE::FLOWER_TULIP_PINK:
	case CBlock3::TYPE::FLOWER_TULIP_RED:
	case CBlock3::TYPE::FLOWER_WITHER_ROSE:
		return true;
	}
	return false;
}


inline CBlock3::GEO_TYPE CBlock3::GetGeoType(TYPE e)
{
	switch (e)
	{
	case TYPE::BEDROCK:
	case TYPE::GRASS:
	case TYPE::STONE_REDSTONE_ORE:
	case TYPE::STONE_LAPIS_ORE:
	case TYPE::STONE_IRON_ORE:
	case TYPE::STONE_GOLD_ORE:
	case TYPE::STONE_EMERALD_ORE:
	case TYPE::STONE_DIAMOND_ORE:
	case TYPE::STONE_COPPER_ORE:
	case TYPE::STONE_COAL_ORE:
	case TYPE::DEEPSLATE:
	case TYPE::DEEPSLATE_COAL_ORE:
	case TYPE::DEEPSLATE_COPPER_ORE:
	case TYPE::DEEPSLATE_DIAMOND_ORE:
	case TYPE::DEEPSLATE_EMERALD_ORE:
	case TYPE::DEEPSLATE_GOLD_ORE:
	case TYPE::DEEPSLATE_IRON_ORE:
	case TYPE::DEEPSLATE_LAPIS_ORE:
	case TYPE::DEEPSLATE_REDSTONE_ORE:
	case TYPE::DIRT:
	case TYPE::SAND:
	case TYPE::LOG_CHERRY:
	case TYPE::LOG_OAK:
	case TYPE::LOG_BIRCH:
	case TYPE::LOG_ACACIA:
	case TYPE::PLANK_CHERRY:
	case TYPE::PLANK_OAK:
	case TYPE::PLANK_BIRCH:
	case TYPE::PLANK_ACACIA:
	case TYPE::LEAVES_CHERRY:
	case TYPE::LEAVES_OAK:
	case TYPE::LEAVES_BIRCH:
	case TYPE::LEAVES_ACACIA:
	case TYPE::OBSIDIAN:
	case TYPE::CHEST:
	case TYPE::CRAFTING_TABLE:
	case TYPE::FURNACE:
	case TYPE::TNT:
	case TYPE::BOOKSHELF:
	case TYPE::COBBLED_DEEPSLATE:
	case TYPE::COBBLESTONE:
		return GEO_TYPE::CUBE;


	case TYPE::WATER_PLACE_HOLDER:
	case TYPE::WATER_FLOW:
	case TYPE::WATER_STILL:
	case TYPE::LAVA_PLACE_HOLDER:
	case TYPE::LAVA_FLOW:
	case TYPE::LAVA_STILL:
		return GEO_TYPE::WATER;

	case TYPE::SLAP_PLANK_CHERRY:
	case TYPE::SLAP_PLANK_OAK:
	case TYPE::SLAP_PLANK_BIRCH:
	case TYPE::SLAP_PLANK_ACACIA:
		return GEO_TYPE::SLAP;

	case TYPE::ENCHANTING_TABLE:
		return GEO_TYPE::ENCHANTING_TABLE;

	case TYPE::LADDER:
		return GEO_TYPE::LADDER;

	case TYPE::STAIR_PLANK_CHERRY:
	case TYPE::STAIR_PLANK_OAK:
	case TYPE::STAIR_PLANK_BIRCH:
	case TYPE::STAIR_PLANK_ACACIA:
		return GEO_TYPE::STAIR;

	case TYPE::TORCH_ON:
	case TYPE::REDSTONE_TORCH_OFF:
	case TYPE::REDSTONE_TORCH_ON:
		return GEO_TYPE::TORCH;

	case  TYPE::FLOWER_ALLIUM:
	case  TYPE::FLOWER_BLUE_ORCHID:
	case  TYPE::FLOWER_CORNFLOWER:
	case  TYPE::FLOWER_DANDELION:
	case  TYPE::FLOWER_HOUSTONIA:
	case  TYPE::FLOWER_LILY_OF_THE_VALLEY:
	case  TYPE::FLOWER_OXEYE_DAISY:
	case  TYPE::FLOWER_PAEONIA:
	case  TYPE::FLOWER_ROSE:
	case  TYPE::FLOWER_ROSE_BLUE:
	case  TYPE::FLOWER_TULIP_ORANGE:
	case  TYPE::FLOWER_TULIP_PINK:
	case  TYPE::FLOWER_TULIP_RED:
	case  TYPE::FLOWER_WITHER_ROSE:
	case  TYPE::FIJI_SHORT_DRY_GRASS:
	case  TYPE::FIJI_SHORT_GRASS:
	case  TYPE::FIJI_TALL_DRY_GRASS:
	case  TYPE::FIJI_TALL_GRASS_BOTTOM:
	case  TYPE::FIJI_TALL_GRASS_TOP:
		return GEO_TYPE::CROSS;


	default:
		return GEO_TYPE::END;
	}
}
NS_END