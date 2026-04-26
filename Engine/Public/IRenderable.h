#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CCameraObject;
typedef struct tagRenderContext
{
	//const CCameraObject* cam;
	RENDERPASS pass;
	E::_vector eye{};
	E::_matrix matView{};
	E::_matrix matProj{};
	E::_matrix matViewProj{};
} RENDER_CTX;

class ENGINE_DLL IRenderable
{
public:
	virtual ~IRenderable() = default;
	virtual HRESULT Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) = 0;
	virtual bool HasRenderPass(RENDERPASS ePass) const = 0;
};

NS_END