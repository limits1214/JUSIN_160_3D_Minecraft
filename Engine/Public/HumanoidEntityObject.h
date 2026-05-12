#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CHumanoidEntityObject : public CEntityObject
{
public:
	typedef struct tagDesc : CEntityObject::DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CHumanoidEntityObject, CEntityObject)

protected:
	explicit CHumanoidEntityObject();
	~CHumanoidEntityObject() override;

public:
	HRESULT Initialize(void* pArg) override;

};

NS_END