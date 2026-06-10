#include "ItemObject.h"

NS_USING(Engine)

void CItemObject::RecipeInitialize()
{
    if (CItemObject::s_bRecipiesInitialize) return;
    CItemObject::s_bRecipiesInitialize = true;

    std::vector<std::pair<CBlock3, CBlock3>> toRecipeLogPlank{
        {CBlock3(CBlock3::TYPE::LOG_OAK), CBlock3(CBlock3::TYPE::PLANK_OAK)},
        {CBlock3(CBlock3::TYPE::LOG_BIRCH), CBlock3(CBlock3::TYPE::PLANK_BIRCH)},
        {CBlock3(CBlock3::TYPE::LOG_ACACIA), CBlock3(CBlock3::TYPE::PLANK_ACACIA)},
        {CBlock3(CBlock3::TYPE::LOG_CHERRY), CBlock3(CBlock3::TYPE::PLANK_CHERRY)},
    };

    for (const auto& toRecipe : toRecipeLogPlank)
    {
        ItemInfo log{};
        log.block = toRecipe.first;
        log.iCnt = 1;

        ItemInfo plank{};
        plank.block = toRecipe.second;
        plank.iCnt = 4;

        SRecipe recipe{};
        recipe.pattern[0].first = log;
        recipe.result = plank;

        CItemObject::s_vecRecipies.push_back(recipe);
    }
    
    std::vector<CBlock3> toRecipePlank{
       {CBlock3(CBlock3::TYPE::PLANK_OAK)},
       {CBlock3(CBlock3::TYPE::PLANK_BIRCH)},
       {CBlock3(CBlock3::TYPE::PLANK_ACACIA)},
       {CBlock3(CBlock3::TYPE::PLANK_CHERRY)},
    };

    for (const auto& toRecipe : toRecipePlank)
    {
        ItemInfo plank{};
        plank.block = toRecipe;
        plank.iCnt = 1;

        ItemInfo stick{};
        stick.eItemType = CItemObject::ITEM_TYPE::ITEM_Stick;
        stick.iCnt = 4;

        SRecipe recipe{};
        recipe.pattern[0].first = plank;
        recipe.pattern[3].first = plank;
        recipe.result = stick;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    for (const auto& toRecipe : toRecipePlank)
    {
        ItemInfo plank{};
        plank.block = toRecipe;
        plank.iCnt = 1;

        ItemInfo craftTable{};
        craftTable.block = CBlock3(CBlock3::TYPE::CRAFTING_TABLE);
        craftTable.iCnt = 1;

        SRecipe recipe{};
        recipe.pattern[0].first = plank;
        recipe.pattern[1].first = plank;
        recipe.pattern[3].first = plank;
        recipe.pattern[4].first = plank;
        recipe.result = craftTable;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    for (const auto& toRecipe : toRecipePlank)
    {
        {
            ItemInfo plank{};
            plank.block = toRecipe;
            plank.iCnt = 1;

            ItemInfo chest{};
            chest.block = CBlock3(CBlock3::TYPE::CHEST);
            chest.iCnt = 1;

            SRecipe recipe{};
            recipe.pattern[0].first = plank;
            recipe.pattern[1].first = plank;
            recipe.pattern[2].first = plank;
            recipe.pattern[3].first = plank;
            recipe.pattern[5].first = plank;
            recipe.pattern[6].first = plank;
            recipe.pattern[7].first = plank;
            recipe.pattern[8].first = plank;
            recipe.result = chest;

            CItemObject::s_vecRecipies.push_back(recipe);
        }
    }

    {
        ItemInfo stick{};
        stick.eItemType = CItemObject::ITEM_TYPE::ITEM_Stick;
        stick.iCnt = 1;

        ItemInfo coal{};
        coal.eItemType = CItemObject::ITEM_TYPE::ITEM_Coal;
        coal.iCnt = 1;

        ItemInfo torch{};
        torch.eItemType = CItemObject::ITEM_TYPE::ITEM_Torch;
        torch.iCnt = 1;

        SRecipe recipe{};
        recipe.pattern[0].first = coal;
        recipe.pattern[3].first = stick;
        recipe.result = torch;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    {
        ItemInfo cobbleStone{};
        cobbleStone.block = CBlock3(CBlock3::TYPE::COBBLESTONE);
        cobbleStone.iCnt = 1;

        ItemInfo furnace{};
        furnace.block = CBlock3(CBlock3::TYPE::FURNACE);
        furnace.iCnt = 1;

        SRecipe recipe{};
        recipe.pattern[0].first = cobbleStone;
        recipe.pattern[1].first = cobbleStone;
        recipe.pattern[2].first = cobbleStone;
        recipe.pattern[3].first = cobbleStone;
        recipe.pattern[5].first = cobbleStone;
        recipe.pattern[6].first = cobbleStone;
        recipe.pattern[7].first = cobbleStone;
        recipe.pattern[8].first = cobbleStone;
        recipe.result = furnace;

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
