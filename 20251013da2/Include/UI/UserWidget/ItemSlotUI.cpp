#include "ItemSlotUI.h"
#include"../Common/Image.h"
#include"../Common/TextBlock.h"
#include "../../Asset/Item/ItemDB.h"
#include "../../Scene/SceneMain.h"

#include "../../Asset/AssetManager.h"
#include "../../Asset/Item/ItemManager.h"
#include "SlotDragButton.h"

#include "../Common/Button.h"


#include "../../Scene/SceneUIManager.h"

CItemSlotUI::CItemSlotUI()
{
}

CItemSlotUI::~CItemSlotUI()
{
}

bool CItemSlotUI::Init()
{
	CUserWidget::Init();

	mSlotBase = CreateChildWidget<CImage>("SlotBase");

	mSlotBase->SetSize(32 * 2.5f, 32 * 2.5f);
	mSlotBase->SetPos(0.f, 0.f);
	mSlotBase->SetPivot(0.5f, 0.5f);

	mSlotBase->SetTexture("InventorySlot");

	mSlotBase->SetZOrder(10);

	

	mItemImage = CreateChildWidget<CImage>("ItemImage");

	mItemImage->SetSize(32 * 2.5f, 32 * 2.5f);
	mItemImage->SetPos(0.f, 0.f);
	mItemImage->SetPivot(0.5f, 0.5f);
	//mItemImage->SetTexture("InventorySlot");
	mItemImage->SetZOrder(10);

	mEnhance = CreateChildWidget<CTextBlock>("mEnhanceNum");

	mEnhance->SetText(L"0");
	mEnhance->SetFontSize(20.f);
	mEnhance->SetPos(20.f, 20.f);
	mEnhance->SetPivot(FVector2D(0.5f, 0.5f));
	mEnhance->SetAlignH(ETextAlignH::Center);
	mEnhance->SetSize(20.f, 20.f);
	mEnhance->SetTextColor(255, 255, 255, 255);
	mEnhance->SetZOrder(100);

	//mDragButton = CreateChildWidget<CSlotDragButton>("DragButton");


	mItemImage->SetEnable(false);

	std::string name = "DragButton" + mName;
	mDragButton = CreateChildWidget<CSlotDragButton>(name);
	mDragButton->SetChild(mItemImage);

	mDragButton->SetSize(32 * 2.5f, 32 * 2.5f);
	mDragButton->SetPos(0.f, 0.f);
	mDragButton->SetPivot(0.5f, 0.5f);
	mDragButton->SetZOrder(200);

	//mDragButton->SetTexture(EButtonState::Normal, "StartButton", TEXT("Texture/Start.png"));
	//mDragButton->SetTexture(EButtonState::Hovered, "StartButton");
	//mDragButton->SetTexture(EButtonState::Click, "StartButton", TEXT("Texture/Start.png"));
	//mDragButton->SetTexture(EButtonState::Disable, "StartButton", TEXT("Texture/Start.png"));
	mDragButton->SetTint(EButtonState::Normal, 0.8f, 0.8f, 0.8f);
	mDragButton->SetTint(EButtonState::Hovered, 1.f, 1.f, 1.f);
	mDragButton->SetTint(EButtonState::Click, 0.5f, 0.5f, 0.5f);
	mDragButton->SetTint(EButtonState::Disable, 0.1f, 0.1f, 0.1f);


	//std::string name = "DragButton2" + mName;
	//mDragButton2 = CreateChildWidget<CButton>(name);

	////mDragButton2 = mScene->GetUIManager()->CreateWidget<CButton>("TestButton");
	//mDragButton2->SetSize(32 * 2.5f, 32 * 2.5f);
	//mDragButton2->SetPos(0.f, 0.f);
	//mDragButton2->SetPivot(0.5f, .5f);
	//mDragButton2->SetZOrder(200);

	//mDragButton2->SetTexture(EButtonState::Normal, "StartButton", TEXT("Texture/Start.png"));
	//mDragButton2->SetTexture(EButtonState::Hovered, "StartButton");
	//mDragButton2->SetTexture(EButtonState::Click, "StartButton", TEXT("Texture/Start.png"));
	//mDragButton2->SetTexture(EButtonState::Disable, "StartButton", TEXT("Texture/Start.png"));
	//mDragButton2->SetTint(EButtonState::Normal, 0.8f, 0.8f, 0.8f);
	//mDragButton2->SetTint(EButtonState::Hovered, 1.f, 1.f, 1.f);
	//mDragButton2->SetTint(EButtonState::Click, 0.5f, 0.5f, 0.5f);
	//mDragButton2->SetTint(EButtonState::Disable, 0.1f, 0.1f, 0.1f);



	return true;
}

void CItemSlotUI::UpdateSlotUI(int index,int slotEnhance,int SlotIndex)
{
	mSlotIndex = SlotIndex;
	const FItemDef* DefPtr = CAssetManager::GetInst()->GetItemManager()->GetDef(index);

	TCHAR buf[16];
	_stprintf_s(buf, TEXT("%d"), slotEnhance);
	mEnhance->SetText(buf);

	if (!DefPtr)
	{
		mItemImage->SetEnable(false);
		return;
	}

	mItemImage->SetEnable(true);
	FItemDef Def = *DefPtr;
	mItemImage->SetTexture(Def.IconTexKey);

}
