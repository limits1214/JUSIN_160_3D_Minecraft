#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVoxelObject : public CGameObject
{
public:
	typedef struct tagDesc : GAMEOBJECT_DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CVoxelObject, CGameObject)

protected:
	explicit CVoxelObject();
	~CVoxelObject() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END