#include "ItemObject.h"
#include "GameInstance.h"
#include "DropItemObject.h"
#include "DropBlock.h"
#include "WorldManager.h"
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
        ItemInfo stick{};
        stick.eItemType = CItemObject::ITEM_TYPE::ITEM_Stick;
        stick.iCnt = 1;

        ItemInfo charcoal{};
        charcoal.eItemType = CItemObject::ITEM_TYPE::ITEM_Charcoal;
        charcoal.iCnt = 1;

        ItemInfo torch{};
        torch.eItemType = CItemObject::ITEM_TYPE::ITEM_Torch;
        torch.iCnt = 1;

        SRecipe recipe{};
        recipe.pattern[0].first = charcoal;
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

    // make armor
    {
        std::vector<ITEM_TYPE> ingots{
           { ITEM_TYPE::ITEM_Copper_Ingot },
           { ITEM_TYPE::ITEM_Iron_Ingot },
           { ITEM_TYPE::ITEM_Gold_Ingot },
           { ITEM_TYPE::ITEM_Diamond },
           { ITEM_TYPE::ITEM_Netherite_Ingot },
        };

        std::vector<ITEM_TYPE> resHelmet{
           { ITEM_TYPE::ITEM_CopperHelmet },
           { ITEM_TYPE::ITEM_IronHelmet },
           { ITEM_TYPE::ITEM_GoldHelmet },
           { ITEM_TYPE::ITEM_DiamondHelmet },
           { ITEM_TYPE::ITEM_NetheriteHelmet },
        };

        std::vector<ITEM_TYPE> resChestplate{
           { ITEM_TYPE::ITEM_CopperChestplate },
           { ITEM_TYPE::ITEM_IronChestplate },
           { ITEM_TYPE::ITEM_GoldChestplate },
           { ITEM_TYPE::ITEM_DiamondChestplate },
           { ITEM_TYPE::ITEM_NetheriteChestplate },
        };

        std::vector<ITEM_TYPE> resLeggings{
          { ITEM_TYPE::ITEM_CopperLeggings },
          { ITEM_TYPE::ITEM_IronLeggings },
          { ITEM_TYPE::ITEM_GoldLeggings },
          { ITEM_TYPE::ITEM_DiamondLeggings },
          { ITEM_TYPE::ITEM_NetheriteLeggings },
        };

        std::vector<ITEM_TYPE> resBoots{
          { ITEM_TYPE::ITEM_CopperBoots },
          { ITEM_TYPE::ITEM_IronBoots },
          { ITEM_TYPE::ITEM_GoldBoots },
          { ITEM_TYPE::ITEM_DiamondBoots },
          { ITEM_TYPE::ITEM_NetheriteBoots },
        };

        // helmet
        for (uint32_t i = 0; i < ingots.size(); ++i)
        {
            ItemInfo ingot{};
            ingot.eItemType = ingots[i];
            ingot.iCnt = 1;

            ItemInfo res{};
            res.eItemType = resHelmet[i];

            SRecipe recipe{};
            recipe.pattern[3].first = ingot;
            recipe.pattern[4].first = ingot;
            recipe.pattern[5].first = ingot;
            recipe.pattern[6].first = ingot;
            recipe.pattern[8].first = ingot;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }

        // chestplate
        for (uint32_t i = 0; i < ingots.size(); ++i)
        {
            ItemInfo ingot{};
            ingot.eItemType = ingots[i];
            ingot.iCnt = 1;

            ItemInfo res{};
            res.eItemType = resChestplate[i];

            SRecipe recipe{};
            recipe.pattern[0].first = ingot;
            recipe.pattern[2].first = ingot;
            recipe.pattern[3].first = ingot;
            recipe.pattern[4].first = ingot;
            recipe.pattern[5].first = ingot;
            recipe.pattern[6].first = ingot;
            recipe.pattern[7].first = ingot;
            recipe.pattern[8].first = ingot;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }

        // leggings
        for (uint32_t i = 0; i < ingots.size(); ++i)
        {
            ItemInfo ingot{};
            ingot.eItemType = ingots[i];
            ingot.iCnt = 1;

            ItemInfo res{};
            res.eItemType = resLeggings[i];

            SRecipe recipe{};
            recipe.pattern[0].first = ingot;
            recipe.pattern[1].first = ingot;
            recipe.pattern[2].first = ingot;
            recipe.pattern[3].first = ingot;
            recipe.pattern[5].first = ingot;
            recipe.pattern[6].first = ingot;
            recipe.pattern[8].first = ingot;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }

        // boots
        for (uint32_t i = 0; i < ingots.size(); ++i)
        {
            ItemInfo ingot{};
            ingot.eItemType = ingots[i];
            ingot.iCnt = 1;

            ItemInfo res{};
            res.eItemType = resBoots[i];

            SRecipe recipe{};
            recipe.pattern[3].first = ingot;
            recipe.pattern[5].first = ingot;
            recipe.pattern[6].first = ingot;
            recipe.pattern[8].first = ingot;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }
    }

    // pixaxe, axe, hoe, shovel, sword
    {
        std::vector<ItemInfo> ingredients{
            ItemInfo(CBlock3::TYPE::PLANK_ACACIA, 1),
            ItemInfo(CBlock3::TYPE::PLANK_OAK, 1),
            ItemInfo(CBlock3::TYPE::PLANK_BIRCH, 1),
            ItemInfo(CBlock3::TYPE::PLANK_CHERRY, 1),
            ItemInfo(CBlock3::TYPE::COBBLESTONE, 1),
            ItemInfo(ITEM_TYPE::ITEM_Copper_Ingot, 1),
            ItemInfo(ITEM_TYPE::ITEM_Iron_Ingot, 1),
            ItemInfo(ITEM_TYPE::ITEM_Gold_Ingot, 1),
            ItemInfo(ITEM_TYPE::ITEM_Diamond, 1),
            ItemInfo(ITEM_TYPE::ITEM_Netherite_Ingot, 1),
        };


        std::vector<ItemInfo> resPixaxe{
            ItemInfo(ITEM_TYPE::ITEM_WoodPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_WoodPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_WoodPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_WoodPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_StonePickaxe),
            ItemInfo(ITEM_TYPE::ITEM_CopperPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_IronPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_GoldPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_DiamondPickaxe),
            ItemInfo(ITEM_TYPE::ITEM_NetheritePickaxe),
        };

        std::vector<ItemInfo> resAxe{
            ItemInfo(ITEM_TYPE::ITEM_WoodAxe),
            ItemInfo(ITEM_TYPE::ITEM_WoodAxe),
            ItemInfo(ITEM_TYPE::ITEM_WoodAxe),
            ItemInfo(ITEM_TYPE::ITEM_WoodAxe),
            ItemInfo(ITEM_TYPE::ITEM_StoneAxe),
            ItemInfo(ITEM_TYPE::ITEM_CopperAxe),
            ItemInfo(ITEM_TYPE::ITEM_IronAxe),
            ItemInfo(ITEM_TYPE::ITEM_GoldAxe),
            ItemInfo(ITEM_TYPE::ITEM_DiamondAxe),
            ItemInfo(ITEM_TYPE::ITEM_NetheriteAxe),
        };

        std::vector<ItemInfo> resHoe{
           ItemInfo(ITEM_TYPE::ITEM_WoodHoe),
           ItemInfo(ITEM_TYPE::ITEM_WoodHoe),
           ItemInfo(ITEM_TYPE::ITEM_WoodHoe),
           ItemInfo(ITEM_TYPE::ITEM_WoodHoe),
           ItemInfo(ITEM_TYPE::ITEM_StoneHoe),
           ItemInfo(ITEM_TYPE::ITEM_CopperHoe),
           ItemInfo(ITEM_TYPE::ITEM_IronHoe),
           ItemInfo(ITEM_TYPE::ITEM_GoldHoe),
           ItemInfo(ITEM_TYPE::ITEM_DiamondHoe),
           ItemInfo(ITEM_TYPE::ITEM_NetheriteHoe),
        };

        std::vector<ItemInfo> resShovel{
           ItemInfo(ITEM_TYPE::ITEM_WoodShovel),
           ItemInfo(ITEM_TYPE::ITEM_WoodShovel),
           ItemInfo(ITEM_TYPE::ITEM_WoodShovel),
           ItemInfo(ITEM_TYPE::ITEM_WoodShovel),
           ItemInfo(ITEM_TYPE::ITEM_StoneShovel),
           ItemInfo(ITEM_TYPE::ITEM_CopperShovel),
           ItemInfo(ITEM_TYPE::ITEM_IronShovel),
           ItemInfo(ITEM_TYPE::ITEM_GoldShovel),
           ItemInfo(ITEM_TYPE::ITEM_DiamondShovel),
           ItemInfo(ITEM_TYPE::ITEM_NetheriteShovel),
        };

        std::vector<ItemInfo> resSword{
           ItemInfo(ITEM_TYPE::ITEM_WoodSword),
           ItemInfo(ITEM_TYPE::ITEM_WoodSword),
           ItemInfo(ITEM_TYPE::ITEM_WoodSword),
           ItemInfo(ITEM_TYPE::ITEM_WoodSword),
           ItemInfo(ITEM_TYPE::ITEM_StoneSword),
           ItemInfo(ITEM_TYPE::ITEM_CopperSword),
           ItemInfo(ITEM_TYPE::ITEM_IronSword),
           ItemInfo(ITEM_TYPE::ITEM_GoldSword),
           ItemInfo(ITEM_TYPE::ITEM_DiamondSword),
           ItemInfo(ITEM_TYPE::ITEM_NetheriteSword),
        };

        assert(ingredients.size() == resPixaxe.size());
        assert(ingredients.size() == resAxe.size());
        assert(ingredients.size() == resHoe.size());
        assert(ingredients.size() == resShovel.size());
        assert(ingredients.size() == resSword.size());

        // pickaxe
        for (uint32_t i = 0; i < ingredients.size(); ++i)
        {
            ItemInfo ingredient{ ingredients[i] };

            ItemInfo stick = ItemInfo(ITEM_TYPE::ITEM_Stick, 1);

            ItemInfo res{ resPixaxe[i] };

            SRecipe recipe{};
            recipe.pattern[0].first = ingredient;
            recipe.pattern[1].first = ingredient;
            recipe.pattern[2].first = ingredient;
            recipe.pattern[4].first = stick;
            recipe.pattern[7].first = stick;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }

        // axe
        for (uint32_t i = 0; i < ingredients.size(); ++i)
        {
            ItemInfo ingredient{ ingredients[i] };

            ItemInfo stick = ItemInfo(ITEM_TYPE::ITEM_Stick, 1);

            ItemInfo res{ resAxe[i] };

            SRecipe recipe{};
            recipe.pattern[0].first = ingredient;
            recipe.pattern[1].first = ingredient;
            recipe.pattern[3].first = ingredient;
            recipe.pattern[4].first = stick;
            recipe.pattern[7].first = stick;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }

        // hoe
        for (uint32_t i = 0; i < ingredients.size(); ++i)
        {
            ItemInfo ingredient{ ingredients[i] };

            ItemInfo stick = ItemInfo(ITEM_TYPE::ITEM_Stick, 1);

            ItemInfo res{ resHoe[i] };

            SRecipe recipe{};
            recipe.pattern[0].first = ingredient;
            recipe.pattern[1].first = ingredient;
            recipe.pattern[4].first = stick;
            recipe.pattern[7].first = stick;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }

        // shovel
        for (uint32_t i = 0; i < ingredients.size(); ++i)
        {
            ItemInfo ingredient{ ingredients[i] };

            ItemInfo stick = ItemInfo(ITEM_TYPE::ITEM_Stick, 1);

            ItemInfo res{ resShovel[i] };

            SRecipe recipe{};
            recipe.pattern[1].first = ingredient;
            recipe.pattern[4].first = stick;
            recipe.pattern[7].first = stick;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }

        // sword
        for (uint32_t i = 0; i < ingredients.size(); ++i)
        {
            ItemInfo ingredient{ ingredients[i] };

            ItemInfo stick = ItemInfo(ITEM_TYPE::ITEM_Stick, 1);

            ItemInfo res{ resSword[i] };

            SRecipe recipe{};
            recipe.pattern[1].first = ingredient;
            recipe.pattern[4].first = ingredient;
            recipe.pattern[7].first = stick;
            recipe.result = res;

            CItemObject::s_vecRecipies.push_back(recipe);
        }
    }

    // bow
    {
        ItemInfo stick = ItemInfo(ITEM_TYPE::ITEM_Stick, 1);

        ItemInfo string = ItemInfo(ITEM_TYPE::ITEM_String, 1);

        ItemInfo res = ItemInfo(ITEM_TYPE::ITEM_Bow_Standby);

        SRecipe recipe{};
        recipe.pattern[1].first = stick;
        recipe.pattern[3].first = stick;
        recipe.pattern[7].first = stick;
        recipe.pattern[2].first = string;
        recipe.pattern[5].first = string;
        recipe.pattern[8].first = string;
        recipe.result = res;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    // arrow
    {
        ItemInfo stick = ItemInfo(ITEM_TYPE::ITEM_Stick, 1);

        ItemInfo feather = ItemInfo(ITEM_TYPE::ITEM_Feather, 1);

        ItemInfo flint = ItemInfo(ITEM_TYPE::ITEM_Flint, 1);

        ItemInfo res = ItemInfo(ITEM_TYPE::ITEM_Arrow, 4);

        SRecipe recipe{};
        recipe.pattern[1].first = flint;
        recipe.pattern[4].first = stick;
        recipe.pattern[7].first = feather;
        recipe.result = res;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    // flintAndSteel
    {
        ItemInfo flint = ItemInfo(ITEM_TYPE::ITEM_Flint, 1);

        ItemInfo ironIngot = ItemInfo(ITEM_TYPE::ITEM_Iron_Ingot, 1);


        ItemInfo res = ItemInfo(ITEM_TYPE::ITEM_FlintAndSteel);

        SRecipe recipe{};
        recipe.pattern[1].first = ironIngot;
        recipe.pattern[3].first = flint;
        recipe.result = res;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    // tnt
    {
        ItemInfo gunpowder = ItemInfo(ITEM_TYPE::ITEM_Gunpowder, 1);

        ItemInfo sand = ItemInfo(CBlock3(CBlock3::TYPE::SAND), 1);


        ItemInfo res = ItemInfo(CBlock3(CBlock3::TYPE::TNT), 1);

        SRecipe recipe{};
        recipe.pattern[0].first = gunpowder;
        recipe.pattern[1].first = sand;
        recipe.pattern[2].first = gunpowder;
        recipe.pattern[3].first = sand;
        recipe.pattern[4].first = gunpowder;
        recipe.pattern[5].first = sand;
        recipe.pattern[6].first = gunpowder;
        recipe.pattern[7].first = sand;
        recipe.pattern[8].first = gunpowder;
        recipe.result = res;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    // bread
    {
        ItemInfo wheat = ItemInfo(ITEM_TYPE::ITEM_Wheat, 1);
        ItemInfo res = ItemInfo(ITEM_TYPE::ITEM_Bread, 1);

        SRecipe recipe{};
        recipe.pattern[0].first = wheat;
        recipe.pattern[1].first = wheat;
        recipe.pattern[2].first = wheat;
       
        recipe.result = res;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

    // bucket
    {
        ItemInfo iron = ItemInfo(ITEM_TYPE::ITEM_Iron_Ingot, 1);
        ItemInfo res = ItemInfo(ITEM_TYPE::ITEM_Bucket_Empty, 1);

        SRecipe recipe{};
        recipe.pattern[0].first = iron;
        recipe.pattern[2].first = iron;
        recipe.pattern[4].first = iron;

        recipe.result = res;

        CItemObject::s_vecRecipies.push_back(recipe);
    }

}

void CItemObject::SpawnDropItemObject(const CItemObject::ItemInfo& info, _float3 pos, _float3 vel)

{
    if (info.block)
    {
        E::CDropItemObject::DESC Desc{};
        Desc.sObjectTag = "CDropBlock_Cube";
        Desc.viBufferId = { "MC_ITEM_VIBuffer", CDropItemObject::GetVIBufferName(info) };

        if (auto pLayer = CGameInstance::Get().GetGameObjectLayer(CDropItemObject::GetDropItemLayer(info), "ITEM", "Prototype_GameObject_DropBlock", &Desc))
        {
            if (!pLayer->empty())
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CDropBlock>(pLayer->front()))
                {
                    auto type = info.block->GetType();

                    std::vector<uint32_t> texs{};
                    for (uint32_t i = 0; i < ETOUI(FACE_DIR::END); ++i)
                    {
                        texs.push_back(PackTexId(9, ETOUI(CBlock3::GetTexType(type, static_cast<FACE_DIR>(i)))));
                    }

                    pObj->AddDropItemObject(info, pos, vel, texs);
                }
            }
        }
    }
    else
    {
        {
            E::CDropItemObject::DESC Desc{};
            Desc.sObjectTag = "CDropItem";
            Desc.viBufferId = { "MC_ITEM_VIBuffer",  CDropItemObject::GetVIBufferName(info) };

            if (auto pLayer = CGameInstance::Get().GetGameObjectLayer(CDropItemObject::GetDropItemLayer(info), "ITEM", "Prototype_GameObject_DropItem", &Desc))
            {
                if (!pLayer->empty())
                {
                    if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CDropItemObject>(pLayer->front()))
                    {
                        pObj->AddDropItemObject(info, pos, vel, { CItemObject::GetPackedTexIdByType(info.eItemType) });
                    }
                }
            }
        }
    }
}

void CItemObject::DestoryBlockAfterProcess(const CItemObject::ItemInfo& info, const XMINT3& wbLocatoin)
{
    switch (info.block->GetType())
    {
    case CBlock3::TYPE::FURNACE:
    {
        auto pStorage = CGameInstance::Get().GetWorldFurnaceStorage()->GetStorage(wbLocatoin);
        _float3 wbFloat = { (float)wbLocatoin.x,(float)wbLocatoin.y, (float)wbLocatoin.z };
        wbFloat.x += 0.5f;
        wbFloat.y += 0.5f;
        wbFloat.z += 0.5f;
        if (pStorage->fuel)
        {
            for (uint32_t i = 0; i < pStorage->fuel->iCnt; ++i)
            {
                auto copy = pStorage->fuel.value();
                copy.iCnt = 1;
                CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
            }
        }
        if (pStorage->ingredient)
        {
            for (uint32_t i = 0; i < pStorage->ingredient->iCnt; ++i)
            {
                auto copy = pStorage->ingredient.value();
                copy.iCnt = 1;
                CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
            }
        }
        if (pStorage->result)
        {
            for (uint32_t i = 0; i < pStorage->result->iCnt; ++i)
            {
                auto copy = pStorage->result.value();
                copy.iCnt = 1;
                CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
            }
        }
        CGameInstance::Get().GetWorldFurnaceStorage()->DelStorage(wbLocatoin);
    }
    return;

    case CBlock3::TYPE::CHEST:
    {
        auto pStorage = CGameInstance::Get().GetWorldChestStorage()->GetStorage(wbLocatoin);
        _float3 wbFloat = { (float)wbLocatoin.x,(float)wbLocatoin.y, (float)wbLocatoin.z };
        wbFloat.x += 0.5f;
        wbFloat.y += 0.5f;
        wbFloat.z += 0.5f;

        for (uint32_t i = 0; i < pStorage->items.size(); ++i)
        {
            if (pStorage->items[i].has_value())
            {
                uint32_t cnt = pStorage->items[i]->iCnt;
                for (uint32_t j = 0; j < cnt; ++j)
                {
                    auto copy = pStorage->items[i].value();
                    copy.iCnt = 1;
                    CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
                }
            }
        }


        CGameInstance::Get().GetWorldChestStorage()->DelStorage(wbLocatoin);
    }

    return;
    }
}

_bool CItemObject::DestoryBlockItemConverter(CItemObject::ItemInfo& info)
{    
    switch (info.block->GetType())
    {
    case CBlock3::TYPE::GRASS:
        info.block->SetType(CBlock3::TYPE::DIRT);
        return true;
    case CBlock3::TYPE::STONE:
        info.block->SetType(CBlock3::TYPE::COBBLESTONE);
        return true;
    case CBlock3::TYPE::STONE_COAL_ORE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Coal;
        return true;
    case CBlock3::TYPE::STONE_COPPER_ORE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Raw_Copper;
        return true;
    case CBlock3::TYPE::STONE_IRON_ORE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Raw_Iron;
        return true;
    case CBlock3::TYPE::STONE_GOLD_ORE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Raw_Gold;
        return true;
    case CBlock3::TYPE::STONE_DIAMOND_ORE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Diamond;
        return true;
    case CBlock3::TYPE::TORCH_ON:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Torch;
        return true;

    case CBlock3::TYPE::LEAVES_ACACIA:
    case CBlock3::TYPE::LEAVES_BIRCH:
    case CBlock3::TYPE::LEAVES_CHERRY:
    case CBlock3::TYPE::LEAVES_OAK:
        if (Randf(0.f, 1.f) > 0.6f)
        {
            info.block = std::nullopt;
            info.eItemType = CItemObject::ITEM_TYPE::ITEM_Apple;
            return true;
        }
        else
        {
            return false;
        }

    case CBlock3::TYPE::FIJI_SHORT_DRY_GRASS:
    case CBlock3::TYPE::FIJI_SHORT_GRASS:
    case CBlock3::TYPE::FIJI_TALL_DRY_GRASS:
    case CBlock3::TYPE::FIJI_TALL_GRASS_BOTTOM:
    case CBlock3::TYPE::FIJI_TALL_GRASS_TOP:
        if (Randf(0.f, 1.f) > 0.8f)
        {
            info.block = std::nullopt;
            info.eItemType = CItemObject::ITEM_TYPE::ITEM_Seeds_Wheat;
            return true;
        }
        else
        {
            return false;
        }

    case CBlock3::TYPE::FLOWER_ALLIUM:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Allium;
        return true;
    case CBlock3::TYPE::FLOWER_BLUE_ORCHID:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Blue_Orchid;
        return true;
    case CBlock3::TYPE::FLOWER_CORNFLOWER:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_CornFlower;
        return true;
    case CBlock3::TYPE::FLOWER_DANDELION:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Dandelion;
        return true;
    case CBlock3::TYPE::FLOWER_HOUSTONIA:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Houstonia;
        return true;
    case CBlock3::TYPE::FLOWER_LILY_OF_THE_VALLEY:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Lily_Of_The_Valley;
        return true;
    case CBlock3::TYPE::FLOWER_OXEYE_DAISY:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Oxeye_Daisy;
        return true;
    case CBlock3::TYPE::FLOWER_PAEONIA:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Paeonia;
        return true;
    case CBlock3::TYPE::FLOWER_ROSE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Rose;
        return true;
    case CBlock3::TYPE::FLOWER_ROSE_BLUE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Rose_Blue;
        return true;
    case CBlock3::TYPE::FLOWER_TULIP_ORANGE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Tulip_Orange;
        return true;
    case CBlock3::TYPE::FLOWER_TULIP_PINK:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Tulip_Pink;
        return true;
    case CBlock3::TYPE::FLOWER_TULIP_RED:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_Tulip_Red;
        return true;
    case CBlock3::TYPE::FLOWER_WITHER_ROSE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Flower_WIther_Rose;
        return true;
    }

    return true;
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
