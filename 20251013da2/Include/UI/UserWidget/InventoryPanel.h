#pragma once
#include "UserWidget.h"

class CImage;
class CItemSlotUI;

class CInventoryPanel :
    public CUserWidget
{

public:
    bool Init();
    virtual void Render();
    void Update(float DeltaTime)override;

    void UpdateSlotUI();

    void SetInventoty(class CInventoryComponent* inven) { mInventory = inven; }
protected:
    CImage* mInventoryBase;

    CUserWidget* mSlotsPanel;

    std::vector<CItemSlotUI*> mSlots;

    class CInventoryComponent* mInventory = nullptr;

};

