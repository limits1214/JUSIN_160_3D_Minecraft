#pragma once

#include "GameObject.h"
NS_BEGIN(Engine)

class ENGINE_DLL CUIObject : public CGameObject
{
public:
	DECLARE_DERIVED_TYPE(CUIObject, CGameObject)

public:
	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float			fX, fY, fSizeX, fSizeY;
	}UIOBJECT_DESC;

protected:
	CUIObject();
	~CUIObject() override;

public:
	HRESULT Initialize(void* pArg) override;

protected:
	_float m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
};

NS_END
