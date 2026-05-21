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
		END
	};

	// 불투명한가?
	_bool IsOpaque() const
	{
		if (m_eType == TYPE::AIR)
		{
			return false;
		}
		return true;
	}
	TYPE GetType() const { return m_eType; }
	void SetType(TYPE e) { m_eType = e; }
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
		return TEX_TYPE::END;
	}

private:
	TYPE m_eType{ TYPE::GRASS };
	uint8_t  m_iFlag{}; // 1바이트: 상위 4비트(밝기), 하위 4비트(상태/회전)
};

NS_END