#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAnimalEntityObject : public CEntityObject
{
public:
	typedef struct tagDesc: CEntityObject::DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CAnimalEntityObject, CEntityObject)

protected:
	explicit CAnimalEntityObject();
	~CAnimalEntityObject() override;

public:
	HRESULT Initialize(void* pArg) override;

};

NS_END