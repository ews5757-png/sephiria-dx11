#include "InventoryWindow.h"
#include "InventoryPanel.h"
#include "../../Device.h"
#include "../Common/Image.h"
#include "../../Share/Log.h"

CInventoryWindow::CInventoryWindow()
{
}

CInventoryWindow::~CInventoryWindow()
{
}

bool CInventoryWindow::Init()
{
    CWindowWidget::Init();


    FResolution rs = CDevice::GetInst()->GetResolution();

    //SetPos(rs.Width * 0.5f, rs.Height * 0.5f);
    //SetPivot(.0f, .0f);


     // 콤보 패널
    mComboPanel = CreateChildWidget<CUserWidget>("UI_ComboPanel");
    // ComboPanel->SetPos({ 20.f, 40.f });
    mComboPanel->SetSize( 300.f, 600.f );

    //인벤토리 패널
    mInventoryPanel = CreateChildWidget<CInventoryPanel>("UI_InventoryPanel");

    mInventoryPanel->SetSize(rs.Width, rs.Height);

     // 아이템 설명 패널
     mDescPanel = CreateChildWidget<CUserWidget>("UI_ItemDescPanel");
    // DescPanel->SetPos({ 860.f, 40.f });
     mDescPanel->SetSize( 300.f, 600.f );


     ToggleInventoryUI();
    return true;
}

void CInventoryWindow::ToggleInventoryUI()
{
    mComboPanel->SetEnable(mEnable);
    mInventoryPanel->SetEnable(mEnable);
    mDescPanel->SetEnable(mEnable);
    mEnable = !mEnable;
}
void CInventoryWindow::Render()
{

    CUserWidget::Render(); // 이게 반드시 있어야 함
}