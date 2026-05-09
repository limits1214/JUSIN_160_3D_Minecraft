#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CEntityObject : public CGameObject
{
public:
	typedef struct tagDesc: GAMEOBJECT_DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CEntityObject, CGameObject)

protected:
	explicit CEntityObject();
	~CEntityObject() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END