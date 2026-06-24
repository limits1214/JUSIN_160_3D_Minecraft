#pragma once

namespace Engine
{
	enum class WINMODE { FULL, WIN };
	enum class MOUSEKEYSTATE { LB, RB, MB, END };
	enum class MOUSEMOVESTATE { X, Y, Z, END };
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, SKYBOX, COLLIDER, UI, UI_ONCURSOR, UI_TOOLTIP, UI_TEXT_AFTER_FILTERRED, END };
	enum class RENDERPASS : uint32_t
	{
		DEFAULT = 1 << 0,
		SHADOW = 1 << 1,
		PLAYER_INVEN_UI = 1 << 2,
	};
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	//enum class VSYNC{ OFF, ON };

	enum class VOXEL_MANAGER_STATE
	{
		WORLD_CREATE,
		WORLD_DESTROY,
	};

	enum class VOXEL_NOISE_TYPE
	{
		HEIGHT
	};

	enum class FACE_DIR
	{
		POS_X = 0, NEG_X,
		POS_Y, NEG_Y,
		POS_Z, NEG_Z,
		END
	};

	enum class CHUNK_DIR
	{
		POS_X = 0, NEG_X,
		POS_Z, NEG_Z,
		POS_X_POS_Z, POS_X_NEG_Z,
		NEG_X_POS_Z, NEG_X_NEG_Z,
		CENTER,
		END
	};

	enum class NOISE_TYPE
	{
		HEIGHT,
		BEDROCK,
		CAVE,
		ORE_IRON,
		ORE_DIAMOND,
		ORE_COAL,
		TEMPERATURE,
		HUMIDITY,
		PLANT_DECO_FIJI_SHORT_GRASS,
		TREE_DENSITY,
		CLOUD,
		END
	};

	enum class BIOME_TYPE : uint8_t
	{
		SNOW,    // 눈 덮인 지대
		DESERT,  // 사막
		PLAINS,  // 평지
		END
	};
}