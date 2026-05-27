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
		PLANK_ACAICA,
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
		PLANK_ACAICA,
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
		END
	};

	enum class GEO_TYPE : uint8_t {
		CUBE,        // 흙, 돌, 나뭇잎 등 일반적인 1x1x1 6면체
		CROSS,       // 잔디, 꽃, 묘목 등 사각형 2장을 십자로 교차하는 형태
		TORCH,       // 횃불처럼 중심에 수축된 기둥 형태
		CROSS_FLAT,  // 수평으로 바닥에 누운 풀이나 카펫 형태 (필요시)
		END
	};

	static GEO_TYPE GetGeoType(TYPE e)
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
		case TYPE::WATER_PLACE_HOLDER:
		case TYPE::LOG_CHERRY:
		case TYPE::LOG_OAK:
		case TYPE::LOG_BIRCH:
		case TYPE::LOG_ACACIA:
		case TYPE::PLANK_CHERRY:
		case TYPE::PLANK_OAK:
		case TYPE::PLANK_BIRCH:
		case TYPE::PLANK_ACAICA:
		case TYPE::LEAVES_CHERRY:
		case TYPE::LEAVES_OAK:
		case TYPE::LEAVES_BIRCH:
		case TYPE::LEAVES_ACACIA:
			return GEO_TYPE::CUBE;

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

	static uint32_t GetBaseColor(CBlock3::TYPE eType)
	{
		switch (eType)
		{
		case TYPE::FIJI_SHORT_GRASS:
			return PackRGBA(100, 200, 100, 255); // ABGR 표현 (리틀 엔디안: R=120, G=210, B=120, A=255)

		case TYPE::LEAVES_OAK:
			return PackRGBA(76, 168, 37, 255);
		case TYPE::LEAVES_BIRCH:
			return PackRGBA(128, 180, 93, 255);
		case TYPE::LEAVES_ACACIA:
			return PackRGBA(106, 144, 38, 255);
		default:
			return 0xFFFFFFFF; // 순수 흰색 (원래 텍스처 색상 그대로)
		}
	}

	
	static TEX_TYPE GetTexType(TYPE blockType, FACE_DIR faceDir)
	{
		if (blockType == TYPE::STONE)
		{
			return TEX_TYPE::STONE;
		}
		else if (blockType == TYPE::BEDROCK)
		{
			return TEX_TYPE::BEDROCK;
		}
		else if (blockType == TYPE::STONE_REDSTONE_ORE)
		{
			return TEX_TYPE::STONE_REDSTONE_ORE;
		}
		else if (blockType == TYPE::STONE_LAPIS_ORE)
		{
			return TEX_TYPE::STONE_LAPIS_ORE;
		}
		else if (blockType == TYPE::STONE_IRON_ORE)
		{
			return TEX_TYPE::STONE_IRON_ORE;
		}
		else if (blockType == TYPE::STONE_GOLD_ORE)
		{
			return TEX_TYPE::STONE_GOLD_ORE;
		}
		else if (blockType == TYPE::STONE_EMERALD_ORE)
		{
			return TEX_TYPE::STONE_EMERALD_ORE;
		}
		else if (blockType == TYPE::STONE_DIAMOND_ORE)
		{
			return TEX_TYPE::STONE_DIAMOND_ORE;
		}
		else if (blockType == TYPE::STONE_COPPER_ORE)
		{
			return TEX_TYPE::STONE_COPPER_ORE;
		}
		else if (blockType == TYPE::STONE_COAL_ORE)
		{
			return TEX_TYPE::STONE_COAL_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE)
		{
			return TEX_TYPE::DEEPSLATE;
		}
		
		else if (blockType == TYPE::DEEPSLATE_COAL_ORE)
		{
			return TEX_TYPE::DEEPSLATE_COAL_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE_COPPER_ORE)
		{
			return TEX_TYPE::DEEPSLATE_COPPER_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE_DIAMOND_ORE)
		{
			return TEX_TYPE::DEEPSLATE_DIAMOND_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE_EMERALD_ORE)
		{
			return TEX_TYPE::DEEPSLATE_EMERALD_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE_GOLD_ORE)
		{
			return TEX_TYPE::DEEPSLATE_GOLD_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE_IRON_ORE)
		{
			return TEX_TYPE::DEEPSLATE_IRON_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE_LAPIS_ORE)
		{
			return TEX_TYPE::DEEPSLATE_LAPIS_ORE;
		}
		else if (blockType == TYPE::DEEPSLATE_REDSTONE_ORE)
		{
			return TEX_TYPE::DEEPSLATE_REDSTONE_ORE;
		}
		else if (blockType == TYPE::GRASS)
		{
			if (faceDir == FACE_DIR::POS_Y)
			{
				return TEX_TYPE::GRASS_TOP_CARRIED;
			}
			else if (faceDir == FACE_DIR::POS_X
				|| faceDir == FACE_DIR::NEG_X
				|| faceDir == FACE_DIR::POS_Z
				|| faceDir == FACE_DIR::NEG_Z)
			{
				return TEX_TYPE::GRASS_SIDE_CARRIED;
			}
			else
			{
				return TEX_TYPE::DIRT;
			}
		}
		else if (blockType == TYPE::DIRT)
		{
			return TEX_TYPE::DIRT;
		}
		else if (blockType == TYPE::SAND)
		{
			return TEX_TYPE::SAND;
		}
		else if (blockType == TYPE::WATER_PLACE_HOLDER)
		{
			return TEX_TYPE::WATER_PLACE_HOLDER;
		}
		else if (blockType == TYPE::TORCH_ON)
		{
			return TEX_TYPE::TORCH_ON;
		}
		else if (blockType == TYPE::REDSTONE_TORCH_OFF)
		{
			return TEX_TYPE::REDSTONE_TORCH_OFF;
		}
		else if (blockType == TYPE::REDSTONE_TORCH_ON)
		{
			return TEX_TYPE::REDSTONE_TORCH_ON;
		}
		else if (blockType == TYPE::LOG_CHERRY)
		{
			if (faceDir == FACE_DIR::POS_Y)
			{
				return TEX_TYPE::LOG_CHERRY_TOP;
			}
			else if (faceDir == FACE_DIR::POS_X
				|| faceDir == FACE_DIR::NEG_X
				|| faceDir == FACE_DIR::POS_Z
				|| faceDir == FACE_DIR::NEG_Z)
			{
				return TEX_TYPE::LOG_CHERRY_SIDE;
			}
			else
			{
				return TEX_TYPE::LOG_CHERRY_TOP;
			}
		}
		else if (blockType == TYPE::LOG_OAK)
		{
			if (faceDir == FACE_DIR::POS_Y)
			{
				return TEX_TYPE::LOG_OAK_TOP;
			}
			else if (faceDir == FACE_DIR::POS_X
				|| faceDir == FACE_DIR::NEG_X
				|| faceDir == FACE_DIR::POS_Z
				|| faceDir == FACE_DIR::NEG_Z)
			{
				return TEX_TYPE::LOG_OAK_SIDE;
			}
			else
			{
				return TEX_TYPE::LOG_OAK_TOP;
			}
		}
		else if (blockType == TYPE::LOG_BIRCH)
		{
			if (faceDir == FACE_DIR::POS_Y)
			{
				return TEX_TYPE::LOG_BIRCH_TOP;
			}
			else if (faceDir == FACE_DIR::POS_X
				|| faceDir == FACE_DIR::NEG_X
				|| faceDir == FACE_DIR::POS_Z
				|| faceDir == FACE_DIR::NEG_Z)
			{
				return TEX_TYPE::LOG_BIRCH_SIDE;
			}
			else
			{
				return TEX_TYPE::LOG_BIRCH_TOP;
			}
		}
		else if (blockType == TYPE::LOG_ACACIA)
		{
			if (faceDir == FACE_DIR::POS_Y)
			{
				return TEX_TYPE::LOG_ACACIA_TOP;
			}
			else if (faceDir == FACE_DIR::POS_X
				|| faceDir == FACE_DIR::NEG_X
				|| faceDir == FACE_DIR::POS_Z
				|| faceDir == FACE_DIR::NEG_Z)
			{
				return TEX_TYPE::LOG_ACACIA_SIDE;
			}
			else
			{
				return TEX_TYPE::LOG_ACACIA_TOP;
			}
		}
		else if (blockType == TYPE::PLANK_CHERRY)
		{
			return TEX_TYPE::PLANK_CHERRY;
		}
		else if (blockType == TYPE::PLANK_OAK)
		{
			return TEX_TYPE::PLANK_OAK;
		}
		else if (blockType == TYPE::PLANK_BIRCH)
		{
			return TEX_TYPE::PLANK_BIRCH;
		}
		else if (blockType == TYPE::PLANK_ACAICA)
		{
			return TEX_TYPE::PLANK_ACAICA;
		}
		else if (blockType == TYPE::LEAVES_CHERRY)
		{
			return TEX_TYPE::LEAVES_CHERRY;
		}
		else if (blockType == TYPE::LEAVES_OAK)
		{
			return TEX_TYPE::LEAVES_OAK;
		}
		else if (blockType == TYPE::LEAVES_BIRCH)
		{
			return TEX_TYPE::LEAVES_BIRCH;
		}
		else if (blockType == TYPE::LEAVES_ACACIA)
		{
			return TEX_TYPE::LEAVES_ACACIA;
		}
		else if (blockType == TYPE::FLOWER_ALLIUM)
		{
			return TEX_TYPE::FLOWER_ALLIUM;
		}
		else if (blockType == TYPE::FLOWER_BLUE_ORCHID)
		{
			return TEX_TYPE::FLOWER_BLUE_ORCHID;
		}
		else if (blockType == TYPE::FLOWER_CORNFLOWER)
		{
			return TEX_TYPE::FLOWER_CORNFLOWER;
		}
		else if (blockType == TYPE::FLOWER_DANDELION)
		{
			return TEX_TYPE::FLOWER_DANDELION;
		}
		else if (blockType == TYPE::FLOWER_HOUSTONIA)
		{
			return TEX_TYPE::FLOWER_HOUSTONIA;
		}
		else if (blockType == TYPE::FLOWER_LILY_OF_THE_VALLEY)
		{
			return TEX_TYPE::FLOWER_LILY_OF_THE_VALLEY;
		}
		else if (blockType == TYPE::FLOWER_OXEYE_DAISY)
		{
			return TEX_TYPE::FLOWER_OXEYE_DAISY;
		}
		else if (blockType == TYPE::FLOWER_PAEONIA)
		{
			return TEX_TYPE::FLOWER_PAEONIA;
		}
		else if (blockType == TYPE::FLOWER_ROSE)
		{
			return TEX_TYPE::FLOWER_ROSE;
		}
		else if (blockType == TYPE::FLOWER_ROSE_BLUE)
		{
			return TEX_TYPE::FLOWER_ROSE_BLUE;
		}
		else if (blockType == TYPE::FLOWER_TULIP_ORANGE)
		{
			return TEX_TYPE::FLOWER_TULIP_ORANGE;
		}
		else if (blockType == TYPE::FLOWER_TULIP_PINK)
		{
			return TEX_TYPE::FLOWER_TULIP_PINK;
		}
		else if (blockType == TYPE::FLOWER_TULIP_RED)
		{
			return TEX_TYPE::FLOWER_TULIP_RED;
		}
		else if (blockType == TYPE::FLOWER_WITHER_ROSE)
		{
			return TEX_TYPE::FLOWER_WITHER_ROSE;
		}

		else if (blockType == TYPE::FIJI_SHORT_DRY_GRASS)
		{
			return TEX_TYPE::FIJI_SHORT_DRY_GRASS;
		}
		else if (blockType == TYPE::FIJI_SHORT_GRASS)
		{
			return TEX_TYPE::FIJI_SHORT_GRASS;
		}
		else if (blockType == TYPE::FIJI_TALL_DRY_GRASS)
		{
			return TEX_TYPE::FIJI_TALL_DRY_GRASS;
		}
		else if (blockType == TYPE::FIJI_TALL_GRASS_BOTTOM)
		{
			return TEX_TYPE::FIJI_TALL_GRASS_BOTTOM;
		}
		else if (blockType == TYPE::FIJI_TALL_GRASS_TOP)
		{
			return TEX_TYPE::FIJI_TALL_GRASS_TOP;
		}

		return TEX_TYPE::END;
	}

	

	static _bool IsNeedAlphaTest(TYPE e)
	{
			
		if (	e == TYPE::LEAVES_CHERRY
			 || e == TYPE::LEAVES_OAK
			 || e == TYPE::LEAVES_BIRCH
			 || e == TYPE::LEAVES_ACACIA
			 || e == TYPE::FLOWER_ALLIUM
			 || e == TYPE::FLOWER_BLUE_ORCHID
			 || e == TYPE::FLOWER_CORNFLOWER
			 || e == TYPE::FLOWER_DANDELION
			 || e == TYPE::FLOWER_HOUSTONIA
			 || e == TYPE::FLOWER_LILY_OF_THE_VALLEY
			 || e == TYPE::FLOWER_OXEYE_DAISY
			 || e == TYPE::FLOWER_PAEONIA
			 || e == TYPE::FLOWER_ROSE
			 || e == TYPE::FLOWER_ROSE_BLUE
			 || e == TYPE::FLOWER_TULIP_ORANGE
			 || e == TYPE::FLOWER_TULIP_PINK
			 || e == TYPE::FLOWER_TULIP_RED
			 || e == TYPE::FLOWER_WITHER_ROSE
			 || e == TYPE::FIJI_SHORT_DRY_GRASS
			 || e == TYPE::FIJI_SHORT_GRASS
			 || e == TYPE::FIJI_TALL_DRY_GRASS
			 || e == TYPE::FIJI_TALL_GRASS_BOTTOM
			 || e == TYPE::FIJI_TALL_GRASS_TOP
			)
		{
			return true;
		}
		return false;
	}

	static uint8_t GetBlockLightByType(TYPE e)
	{
		if (e == TYPE::SAND) // test
		{
			return 15;
		}
		else if (e == TYPE::TORCH_ON)
		{
			return 15;
		}
		return 0;
	}
	// 불투명한가?
	_bool IsOpaque() const
	{
		if (m_eType == TYPE::AIR)
		{
			return false;
		}
		else if (m_eType == TYPE::WATER_PLACE_HOLDER)
		{
			return false;
		}
		else if (m_eType == TYPE::TORCH_ON)
		{
			return false;
		}
		else if (m_eType == TYPE::FIJI_SHORT_GRASS)
		{
			return false;
		}
		else if (m_eType == TYPE::FLOWER_OXEYE_DAISY)
		{
			return false;
		}
		return true;
	}
	_bool IsWater() const
	{
		if (m_eType == TYPE::WATER_PLACE_HOLDER)
		{
			return true;
		}

		return false;
	}
	TYPE GetType() const { return m_eType; }
	void SetType(TYPE e) { m_eType = e; }
	uint8_t GetSkyLight() const
	{
		return (m_iLight >> 4) & 0xF;
	}

	uint8_t GetBlockLight() const
	{
		return m_iLight & 0xF;
	}

	void SetSkyLight(uint8_t v)
	{
		m_iLight = (m_iLight & 0x0F) | (v << 4);
	}

	void SetBlockLight(uint8_t v)
	{
		m_iLight = (m_iLight & 0xF0) | v;
	}
	uint8_t GetLight() const { return m_iLight; }
private:
	TYPE m_eType{ TYPE::GRASS };
	//햇빛 15 → 14 → 13 → 12 → ... → 0 (완전 어둠)
	uint8_t m_iLight{}; // 상위 4bit: skylight, 하위4bit: blocklight
	uint8_t  m_iFlag{}; // etc
};

NS_END