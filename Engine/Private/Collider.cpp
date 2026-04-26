#include "pch.h"
#include "Collider.h"
#include "CollBox.h"

NS_USING(Engine)

CCollider::CCollider(CollType eCollType)
	: m_eCollType(eCollType)
{
}

CCollider::~CCollider()
{
}
