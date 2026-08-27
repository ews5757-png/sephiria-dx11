#include "ItemDB.h"

bool CItemDB::InitHardcoded()
{
    Clear();

    {
        FItemDef Def;
        Def.ID = 1;
        Def.Type = EItemType::Tablet;
        Def.Grade = EItemGrade::Rare;
        Def.Name = "StoneTablet_Metastasis";
        Def.Desc = "Restore HP.";
        Def.IconTexKey = "StoneTablet_Metastasis";
        Def.Attack = 0;
        Def.Defense = 0;
        Def.Pattern = { 0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        1,1,1,1,1, 0,1,1,1,1,1,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0,
                        0,0,0,0,0,-1,0,0,0,0,0 };

        if (!Register(Def)) return false;
    }

    {
        FItemDef Def;
        Def.ID = 2;
        Def.Type = EItemType::Artifact;
        Def.Grade = EItemGrade::Normal;
        Def.Name = "Wood Sword";
        Def.Desc = "A basic sword.";
        Def.IconTexKey = "Icon_Wood_Sword";
        Def.Attack = 3;
        Def.Defense = 0;

        if (!Register(Def)) return false;
    }

    return true;
}

void CItemDB::Clear()
{
    mItemDefs.clear();
}

const FItemDef* CItemDB::Find(int ID) const
{
    auto it = mItemDefs.find(ID);
    if (it == mItemDefs.end())
        return nullptr;

    return &it->second;
}

bool CItemDB::Register(const FItemDef& Def)
{
    if (!Def.IsValid())
        return false;

    auto it = mItemDefs.find(Def.ID);
    if (it != mItemDefs.end())
        return false; // 중복 ID

    mItemDefs.insert({ Def.ID, Def });
    return true;
}