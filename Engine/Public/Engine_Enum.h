#pragma once

namespace Engine
{
	enum class WINMODE { FULL, WIN };
	enum class MOUSEKEYSTATE { LB, RB, MB, END };
	enum class MOUSEMOVESTATE { X, Y, Z, END };
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, COLLIDER, UI, END };
	enum class RENDERPASS : uint32_t
	{
		DEFAULT = 1 << 0,
		SHADOW = 1 << 1,
		SSAO = 1 << 2,
	};
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	//enum class VSYNC{ OFF, ON };

	enum class VOXEL_MANAGER_STATE
	{
		WORLD_CREATE,
		WORLD_DESTROY,
	};
}