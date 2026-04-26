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
		THE_END
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;