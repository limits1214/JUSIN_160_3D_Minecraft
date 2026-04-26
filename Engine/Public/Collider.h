#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider: public CEngineBase
{
public:
	DECLARE_DERIVED_TYPE(CCollider, CEngineBase)

public:
	enum class CollType { Box,OrientedBox,Sphere,Frustum };
	CollType GetCollType() const { return m_eCollType; }

public:
	CEngineBase* GetInnerPointer() const { return m_pInner; }
	void SetInnerPointer(CEngineBase* p) { m_pInner = p ; }
	StringID GetInnerHint() const { return m_InnerHint; }
	void SetInnerHint(StringID hint) { m_InnerHint = hint; }

public:
	void SetOriginalColor(const _float4& color) { m_vOriginalColor = color; }
	void SetIntersectColor(const _float4& color) { m_vIntersectColor = color; }
	const _float4& GetOriginalColor() const { return m_vOriginalColor; }
	const _float4& GetIntersectColor() const { return m_vIntersectColor; }

protected:
	explicit CCollider(CollType eCollType);
	~CCollider() override;

public:
	virtual _bool Intersect(const CCollider& collider) const = 0;
	virtual void Transform(_fmatrix wordMatrix) = 0;

protected:
	const CollType m_eCollType{};
	CEngineBase* m_pInner{};
	StringID m_InnerHint{};

protected:
	_float4 m_vOriginalColor{ Colors::Green };
	_float4 m_vIntersectColor{ Colors::Red };
};

NS_END