#include "pch.h"

#include "ChestStorage.h"

NS_USING(Engine)

CChestStorage::CChestStorage()
{
}

CChestStorage::~CChestStorage()
{
}


void CChestStorage::Update(E::_float fTimeDelta)
{
}

UPtr<CChestStorage> CChestStorage::Create()
{
    auto pInstance = ToUPtr(new CChestStorage{});
    
    return pInstance;
}