// ItemManager.h
#pragma once
#include "ItemDB.h"

class CItemManager
{
    friend class CAssetManager;
private:
    CItemManager() = default;
    ~CItemManager() = default;

public:
    bool Init();
    void Clear();

    const FItemDef* GetDef(int ID) const { return mDB.Find(ID); }

    bool IsValidItem(int ID) const { return mDB.Find(ID) != nullptr; }

    const char* GetName(int ID) const;
    const std::string& GetIconKey(int ID) const;

private:
    bool ValidateDefs() const;

private:
    CItemDB mDB;

private:
    // 없는 키 요청 시 반환할 더미(참조 반환용)
    static const std::string sEmptyString;
};
