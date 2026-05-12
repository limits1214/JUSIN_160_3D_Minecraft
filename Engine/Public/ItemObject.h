#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CItemObject : public CGameObject
{
public:
	typedef struct tagDesc : GAMEOBJECT_DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CItemObject, CGameObject)

protected:
	explicit CItemObject();
	~CItemObject() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END