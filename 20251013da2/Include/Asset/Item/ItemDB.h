#pragma once
#include "../../GameInfo.h"

class CItemDB
{
public:
    CItemDB() = default;
    ~CItemDB() = default;

public:
    bool InitHardcoded();                 // 하드코딩 등록
    void Clear();

    // 조회
    const FItemDef* Find(int ID) const;

    // 등록(중복/유효성 체크)
    bool Register(const FItemDef& Def);

    // 전체 순회(아이콘 검증/디버그용)
    const std::unordered_map<int, FItemDef>& GetAll() const { return mItemDefs; }

private:
    std::unordered_map<int, FItemDef> mItemDefs;
};
