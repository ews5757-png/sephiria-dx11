#pragma once
#include "Component.h"
class CInventoryComponent :
    public CComponent
{
public:
    CInventoryComponent();
    virtual ~CInventoryComponent();

public:

    std::vector<FItemSlot> mSlots; 
    std::vector<int> mSlotBuff;

    bool Init();

    FItemSlot& GetSlot(int index) { return mSlots[index]; }

    bool AddItem(int itemID);
    bool SetItem(int index, int itemID, ERotation rot = ERotation::R0);
    bool ClearItem(int index);
    bool RotateItem(int index);
    bool MoveItem(int from, int to);

    void RebuildSlotBuff();   // 인벤토리 전체 재계산(배치/회전 바뀔 때 호출)

    void ApplyPatternToBuff(int slotX, int slotY, const FItemDef& def, ERotation rot);

    void UpdateUI();

    inline void RotateCoord90(int size, int& x, int& y)
    {
        int nx = size - 1 - y;
        int ny = x;
        x = nx; y = ny;
    }

private:

    int MaxSlots = 24;
    int mWidth = 6;
    int mHeight = 4;

};

