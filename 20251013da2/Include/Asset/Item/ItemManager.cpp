// ItemManager.cpp
#include "ItemManager.h"

const std::string CItemManager::sEmptyString = "";

bool CItemManager::Init()
{
    if (!mDB.InitHardcoded())
        return false;

    //if (!ValidateDefs())
    //    return false;

    return true;
}

void CItemManager::Clear()
{
    mDB.Clear();
}

bool CItemManager::ValidateDefs() const
{
    // 최소 검증: 이름/아이콘키 비었는지 정도만
    // (여기서 AssetManager의 TextureManager에 실제 존재 여부까지 체크하고 싶으면 추가하면 됨)
    for (const auto& pair : mDB.GetAll())
    {
        const FItemDef& Def = pair.second;

        if (!Def.IsValid())
            return false;

        if (Def.Name.empty())
            return false;

        // 아이콘을 반드시 쓰는 게임이면 비어있으면 실패 처리
        if (Def.IconTexKey.empty())
            return false;
    }

    return true;
}

const char* CItemManager::GetName(int ID) const
{
    const FItemDef* Def = mDB.Find(ID);
    if (!Def) return "";
    return Def->Name.c_str();
}

const std::string& CItemManager::GetIconKey(int ID) const
{
    const FItemDef* Def = mDB.Find(ID);
    if (!Def) return sEmptyString;
    return Def->IconTexKey;
}
