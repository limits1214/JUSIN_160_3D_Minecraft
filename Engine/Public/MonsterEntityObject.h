#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMonsterEntityObject : public CEntityObject
{
public:
	typedef struct tagDesc : CEntityObject::DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CMonsterEntityObject, CEntityObject)

protected:
	explicit CMonsterEntityObject();
	~CMonsterEntityObject() override;

public:
	HRESULT Initialize(void* pArg) override;

};

NS_END