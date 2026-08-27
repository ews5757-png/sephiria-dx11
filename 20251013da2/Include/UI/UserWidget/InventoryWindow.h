#pragma once
#include "WindowWidget.h"

class CUserWidget;
class CInventoryPanel;

class CInventoryWindow :
    public CWindowWidget
{
public:
    CInventoryWindow();
    virtual ~CInventoryWindow();
private:
    CUserWidget* mComboPanel = nullptr;

    //인벤토리 패널
    CInventoryPanel* mInventoryPanel = nullptr;

    CUserWidget* mDescPanel = nullptr;

    bool mEnable = false;
public:
    bool Init();
    virtual void Render();

    void ToggleInventoryUI();

    CInventoryPanel* GetInventoryPanel() const { return mInventoryPanel; }

    // Setter
    void SetInventoryPanel(CInventoryPanel* panel) { mInventoryPanel = panel; }




};

