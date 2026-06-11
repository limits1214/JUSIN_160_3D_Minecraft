#include "pch.h"

#include "FurnaceStorage.h"

NS_USING(Engine)

CFurnaceStorage::CFurnaceStorage()
{
}

CFurnaceStorage::~CFurnaceStorage()
{
}

HRESULT CFurnaceStorage::Initialize(void* pArg)
{

    if (FAILED(CVoxelObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}

void CFurnaceStorage::PriorityUpdate(E::_float fTimeDelta)
{
}

void CFurnaceStorage::Update(E::_float fTimeDelta)
{
    for (auto& [location, storage] : m_mapLocation)
    {
        if (!storage.ingredient.has_value())
        {
            storage.progress = 0.f;
            continue;
        }
        auto smeltResult = GetSmeltResult(*storage.ingredient);
        if (!smeltResult.has_value())
        {
            storage.progress = 0.f;
            continue;
        }
        if (storage.result.has_value())
        {
            bool bSameType = false;
            if (storage.result->block.has_value() && smeltResult->block.has_value())
                bSameType = storage.result->block->GetType() == smeltResult->block->GetType();
            else if (!storage.result->block.has_value() && !smeltResult->block.has_value())
                bSameType = storage.result->eItemType == smeltResult->eItemType;

            if (!bSameType || storage.result->iCnt >= MAX_STACK)
            {
                storage.progress = 0.f;
                continue;
            }
        }

        // 연료 소진 시 새 연료 소모
        if (storage.fuelProgress <= 0.f)
        {
            if (!storage.fuel.has_value())
            {
                storage.progress = 0.f;
                continue;
            }
            auto duration = GetFuelDuration(*storage.fuel);
            if (!duration.has_value())
            {
                storage.progress = 0.f;
                continue;
            }
            if (storage.fuel->iCnt <= 1)
                storage.fuel = std::nullopt;
            else
                storage.fuel->iCnt--;

            storage.fuelDuration = *duration;
            storage.fuelProgress = 1.f;
        }

        storage.fuelProgress -= fTimeDelta / storage.fuelDuration;
        if (storage.fuelProgress < 0.f)
            storage.fuelProgress = 0.f;

        storage.progress += fTimeDelta / SMELT_TIME;
        if (storage.progress >= 1.f)
        {
            storage.progress = 0.f;

            if (storage.ingredient->iCnt <= 1)
                storage.ingredient = std::nullopt;
            else
                storage.ingredient->iCnt--;

            if (!storage.result.has_value())
                storage.result = *smeltResult;
            else
                storage.result->iCnt++;
        }
    }
}

void CFurnaceStorage::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CFurnaceStorage::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

UPtr<CFurnaceStorage> CFurnaceStorage::Create()
{
    auto pInstance = ToUPtr(new CFurnaceStorage{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CFurnaceStorage");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CFurnaceStorage::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CFurnaceStorage{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CFurnaceStorage");
        return nullptr;
    }

    return pInstance;
}

std::optional<CItemObject::ItemInfo> CFurnaceStorage::GetSmeltResult(const CItemObject::ItemInfo& ingredient)
{
    // 블록 재료
    if (ingredient.block.has_value())
    {
        switch (ingredient.block->GetType())
        {
        //case CBlock3::TYPE::STONE_IRON_ORE:
        //case CBlock3::TYPE::DEEPSLATE_IRON_ORE:
        //    return CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_IronIngot, 1 };
        //case CBlock3::TYPE::STONE_GOLD_ORE:
        //case CBlock3::TYPE::DEEPSLATE_GOLD_ORE:
        //    return CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_GoldIngot, 1 };
        case CBlock3::TYPE::STONE_COAL_ORE:
        case CBlock3::TYPE::DEEPSLATE_COAL_ORE:
            return CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_Coal, 1 };
        case CBlock3::TYPE::LOG_OAK:
        case CBlock3::TYPE::LOG_BIRCH:
        case CBlock3::TYPE::LOG_CHERRY:
        case CBlock3::TYPE::LOG_ACACIA:
            return CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_Coal, 1 };
        //case CBlock3::TYPE::SAND:
        //    return CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_Glass, 1 };
        //case CBlock3::TYPE::COBBLESTONE:
        //    return CItemObject::ItemInfo{ CBlock3{ CBlock3::TYPE::STONE }, 1 };
        default:
            return std::nullopt;
        }
    }

    // 아이템 재료
    switch (ingredient.eItemType)
    {
    case CItemObject::ITEM_TYPE::ITEM_Coal: // 예시: 그 외 아이템 제련
        return std::nullopt;
    default:
        return std::nullopt;
    }
}

std::optional<float> CFurnaceStorage::GetFuelDuration(const CItemObject::ItemInfo& fuel)
{
    if (fuel.block.has_value())
    {
        switch (fuel.block->GetType())
        {
        case CBlock3::TYPE::LOG_OAK:
        case CBlock3::TYPE::LOG_BIRCH:
        case CBlock3::TYPE::LOG_CHERRY:
        case CBlock3::TYPE::LOG_ACACIA:
            return 15.f;
        case CBlock3::TYPE::PLANK_OAK:
        case CBlock3::TYPE::PLANK_BIRCH:
        case CBlock3::TYPE::PLANK_CHERRY:
        case CBlock3::TYPE::PLANK_ACACIA:
            return 15.f;
        default:
            return std::nullopt;
        }
    }

    switch (fuel.eItemType)
    {
    case CItemObject::ITEM_TYPE::ITEM_Coal:    return 80.f;
    case CItemObject::ITEM_TYPE::ITEM_Stick:   return 5.f;
    default:
        return std::nullopt;
    }
}
