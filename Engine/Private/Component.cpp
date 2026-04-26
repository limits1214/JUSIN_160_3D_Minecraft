#include "pch.h"

#include "Component.h"
NS_USING(Engine)

CComponent::CComponent()
{
}

CComponent::CComponent(const CComponent& Prototype)
{
}

CComponent::~CComponent()
{
}

HRESULT CComponent::Initialize(void* pArg)
{
    return S_OK;
}

void CComponent::UpdateGUI()
{
}
