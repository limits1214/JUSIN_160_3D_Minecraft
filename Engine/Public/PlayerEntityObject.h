#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPlayerEntityObject : public CEntityObject
{
public:
	typedef struct tagDesc : CEntityObject::DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CPlayerEntityObject, CEntityObject)

protected:
	explicit CPlayerEntityObject();
	~CPlayerEntityObject() override;

public:
	HRESULT Initialize(void* pArg) override;

};

NS_END