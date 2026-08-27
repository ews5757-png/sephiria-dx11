#include "InventoryPanel.h"
#include "ItemSlotUI.h"


#include "../../Device.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneUIManager.h"
#include "../Common/Image.h"

#include "../../Component/InventoryComponent.h"

#include "../../Share/Log.h"

bool CInventoryPanel::Init()
{
	CUserWidget::Init();

	FResolution RS = CDevice::GetInst()->GetResolution();

	SetPos(RS.Width * 0.5f, RS.Height * 0.5f);
	//SetPivot(1.0f, 1.0f);
	SetPivot(.5f, .5f);


	float inventoryBaseSizeWidth = 290.f * 2.f;
	float inventoryBaseSizeHeight = 200.f * 2.f;

	const int ColCount = 6;
	const int RowCount = 4;


	float mSlotsPanelSizeWidth = 245.f * 2.f;
	float mSlotsPanelSizeHeight = 165.f * 2.f;


	float slotWidth = mSlotsPanelSizeWidth / ColCount;
	float slotHeight = mSlotsPanelSizeHeight / RowCount;

	//배경
	mInventoryBase = CreateChildWidget<CImage>("Inventorybase");

	mInventoryBase->SetSize(inventoryBaseSizeWidth, inventoryBaseSizeHeight);
	mInventoryBase->SetPos(0.f, 0.f);
	mInventoryBase->SetPivot(.5f, .5f);

	mInventoryBase->SetTexture("InventoryBase");
	mInventoryBase->SetZOrder(1);

	mSlotsPanel = CreateChildWidget<CUserWidget>("mSlotsPanel");


	//mSlotsPanel->SetSize(RS.Width, RS.Height);


	mSlotsPanel->SetSize(mSlotsPanelSizeWidth, mSlotsPanelSizeHeight);
	mSlotsPanel->SetPos(0.f, 0.f);
	mSlotsPanel->SetPivot(1.0f, 1.0f);
	mSlotsPanel->SetZOrder(100);


	for (size_t i = 0; i < 24; i++)
	{
		std::string name = "slot_" + std::to_string(i);
		CItemSlotUI* itemslot = mSlotsPanel->CreateChildWidget<CItemSlotUI>(name);
		//CItemSlotUI* itemslot = CreateChildWidget<CItemSlotUI>(name);

		int col = i % ColCount;   // 0~5
		int row = i / ColCount;   // 0~3

		float x = col * slotWidth;
		float y = row * slotHeight;


		itemslot->SetSize(32 * 2.5f, 32 * 2.5f);
		itemslot->SetPos(x, y + -10);
		itemslot->SetPivot(0.0f, 0.0f);
		itemslot->SetSlotIndex(i);
		itemslot->SetZOrder(-10 + i);

		mSlots.push_back(itemslot);
	}




	return true;
}

void CInventoryPanel::Update(float DeltaTime)
{
	CUserWidget::Update(DeltaTime);

	int a = 0;
}

void CInventoryPanel::UpdateSlotUI()
{
	int i = mInventory->mSlots.size();
	for (size_t j = 0; j < i; ++j)
	{
		mSlots[j]->UpdateSlotUI(mInventory->mSlots[j].Item.Defins.ID, mInventory->mSlots[j].Level,j);
	}
}
void CInventoryPanel::Render()
{
	CUserWidget::Render();
}