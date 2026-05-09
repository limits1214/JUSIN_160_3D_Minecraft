#pragma once

#include "Engine_Defines.h"

namespace Client
{
	enum class LEVEL {
		LOADING,
		LOGO,
		MENU,
		THE_OVERWORLD,
		THE_NETHER,
		THE_END,
		TEST_SIMPLE_GREEDY,
		TEST_WORLD,
	};

	enum class LEVEL_LOADING_LAYERS
	{
		OBJECTS,
		CAMERA,
		END
	};

	inline std::string LevelLoadingLayersToString(uint32_t idx)
	{
		switch (idx)
		{
		case E::ETOUI(LEVEL_LOADING_LAYERS::OBJECTS): return "OBJECTS";
		case E::ETOUI(LEVEL_LOADING_LAYERS::CAMERA): return "CAMERA";
		};

		return "END";
	}


	enum class LEVEL_TEST_SIMPLE_GREEDY_LAYERS
	{
		OBJECTS,
		CAMERA,
		END
	};

	inline std::string LevelTestSimpleGreedyLayersToString(uint32_t idx)
	{
		switch (idx)
		{
		case E::ETOUI(LEVEL_TEST_SIMPLE_GREEDY_LAYERS::OBJECTS): return "OBJECTS";
		case E::ETOUI(LEVEL_TEST_SIMPLE_GREEDY_LAYERS::CAMERA): return "CAMERA";
		};

		return "END";
	}


	enum class LEVEL_TEST_WORLD_LAYERS
	{
		ENTITY,
		CAMERA,
		END
	};

	inline std::string LevelTestWorldLayersToString(uint32_t idx)
	{
		switch (idx)
		{
		case E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY): return "ENTITY";
		case E::ETOUI(LEVEL_TEST_WORLD_LAYERS::CAMERA): return "CAMERA";
		};

		return "END";
	}
}