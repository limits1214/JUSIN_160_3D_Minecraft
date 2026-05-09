#include "PigEntity.h"

#include "Resources.h"
#include "GameInstance.h"

NS_USING(Engine)

CPigEntity::CPigEntity()
{
}

CPigEntity::~CPigEntity()
{
}

HRESULT CPigEntity::Initialize(void* pArg)
{
    if (FAILED(CAnimalEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }



	return S_OK;
}

void CPigEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CPigEntity::Update(E::_float fTimeDelta)
{
}

void CPigEntity::LateUpdate(E::_float fTimeDelta)
{
}

UPtr<CPigEntity> CPigEntity::Create()
{
    auto pInstance = ToUPtr(new CPigEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CPigEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CPigEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CPigEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPigEntity");
        return nullptr;
    }

    return pInstance;
}
