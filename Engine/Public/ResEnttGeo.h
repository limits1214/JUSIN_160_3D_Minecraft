#pragma once

#include "Resource.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResEnttGeo : public CResource
{

public:
	DECLARE_DERIVED_TYPE(CResEnttGeo, CResource)

protected:
	explicit CResEnttGeo(const _string& sPath);
	~CResEnttGeo() override;

public:
	const ENTITY_GEOMETRY& GetGeometry() const { return m_entityGeometry; }

protected:
	void RightToLeft();
	void Scaling();
protected:
	ENTITY_GEOMETRY m_entityGeometry{};

};

NS_END