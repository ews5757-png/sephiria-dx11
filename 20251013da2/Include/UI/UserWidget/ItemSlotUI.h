#pragma once
#include "UserWidget.h"
class CImage;
class CTextBlock;
class CSlotDragButton;
class CButton;

class CItemSlotUI :
    public CUserWidget
{
public:
    CItemSlotUI();
    ~CItemSlotUI();
    bool Init();

    void UpdateSlotUI(int Itemindex,int slotEnhance,int SlotIndex);

    void SetSlotIndex(int index) { mSlotIndex = index; }

    int GetSlotIndex() { return mSlotIndex; }

private:
    CImage* mSlotBase;

    CSlotDragButton* mDragButton;
    //CButton* mDragButton2;
    CImage* mItemImage;

    CTextBlock* mEnhance;

    int mSlotIndex;
};

