#include "ItemObject.h"

NS_USING(Engine)

void CItemObject::RecipeInitialize()
{
    if (CItemObject::s_bRecipiesInitialize) return;
    CItemObject::s_bRecipiesInitialize = true;

    {
        ItemInfo log{};
        log.block = CBlock3(CBlock3::TYPE::LOG_OAK);
        log.iCnt = 1;

        ItemInfo plank{};
        plank.block = CBlock3(CBlock3::TYPE::PLANK_OAK);
        plank.iCnt = 4;

        SRecipe recipe{};
        recipe.pattern[0].first = log;
        recipe.result = plank;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    {
        ItemInfo log{};
        log.block = CBlock3(CBlock3::TYPE::LOG_BIRCH);
        log.iCnt = 1;

        ItemInfo plank{};
        plank.block = CBlock3(CBlock3::TYPE::PLANK_BIRCH);
        plank.iCnt = 4;

        SRecipe recipe{};
        recipe.pattern[0].first = log;
        recipe.result = plank;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    {
        ItemInfo log{};
        log.block = CBlock3(CBlock3::TYPE::LOG_ACACIA);
        log.iCnt = 1;

        ItemInfo plank{};
        plank.block = CBlock3(CBlock3::TYPE::PLANK_ACACIA);
        plank.iCnt = 4;

        SRecipe recipe{};
        recipe.pattern[0].first = log;
        recipe.result = plank;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    {
        ItemInfo log{};
        log.block = CBlock3(CBlock3::TYPE::LOG_CHERRY);
        log.iCnt = 1;

        ItemInfo plank{};
        plank.block = CBlock3(CBlock3::TYPE::PLANK_CHERRY);
        plank.iCnt = 4;

        SRecipe recipe{};
        recipe.pattern[0].first = log;
        recipe.result = plank;

        CItemObject::s_vecRecipies.push_back(recipe);
    }
}

CItemObject::CItemObject()
{
}

CItemObject::~CItemObject()
{
}

HRESULT CItemObject::Initialize(void* pArg)
{
    auto* pDesc = static_cast<DESC*>(pArg);
    //m_ItemInfo = pDesc->info;

    if (FAILED(CGameObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
