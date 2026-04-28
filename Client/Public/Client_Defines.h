#pragma once

namespace Client
{
	static const unsigned int	g_iWinSizeX{ 1280 };
	static const unsigned int	g_iWinSizeY{ 720 };

	enum class LEVEL {
		LOADING,
		LOGO,
		MENU,
		THE_OVERWORLD,
		THE_NETHER,
		THE_END,
		TEST_SIMPLE_GREEDY,
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
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;