#include "SlotDragButton.h"
#include "../../Share/Log.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Input.h"
#include "../../Scene/SceneUIManager.h"
#include "ItemSlotUI.h"
#include "../../Scene/SceneMain.h"


bool CSlotDragButton::Init()
{
	CButton::Init();
	//mScene->GetUIManager()->AddToViewport(this);

	SetEnableDrag(true);
	return true;
}
void CSlotDragButton::ButtonClick()
{
	CButton::ButtonClick();

}

void CSlotDragButton::Update(float DeltaTime)
{
    CButton::Update(DeltaTime);
}

void CSlotDragButton::DraggingStart()
{
	CButton::DraggingStart();
}

void CSlotDragButton::DraggingEnd()
{
	CButton::DraggingEnd();
}

void CSlotDragButton::OnDrop(CWidget* DragWidget)
{
	auto* drag = dynamic_cast<CSlotDragButton*>(DragWidget);
	if (!drag) return;


	CSceneMain* temp = dynamic_cast<CSceneMain*>(mScene);
	if (temp)
	{

		CItemSlotUI* slot1 = dynamic_cast<CItemSlotUI*>(mParent);
		CItemSlotUI* slot2 = dynamic_cast<CItemSlotUI*>(drag->mParent);
		temp->MoveItem(slot1->GetSlotIndex(), slot2->GetSlotIndex());
		temp->UpdateInventoty();
	}
}
