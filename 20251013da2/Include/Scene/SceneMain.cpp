#include "SceneMain.h"
#include "SceneAssetManager.h"

#include "../Object/PlayerObject.h"
#include "../Object/Tree.h"
#include "../Object/MainCamera.h"
#include "../Object/PlayerShieldObject.h"
#include "../Object/StoneMoleMelee.h"
#include "../Object/MoleGuard.h"
#include "../Object/WarningLine.h"

#include "../UI/UserWidget/InventoryWindow.h"
#include "../UI/UserWidget/InventoryPanel.h"
#include "SceneUIManager.h"
#include "../Object/TileMapObj.h"
#include "../Object/AskardStatue.h"
#include "../Object/TorchStand.h"

#include "../Render/RenderManager.h"


#include "../Component/InventoryComponent.h"


CSceneMain::CSceneMain()
{
}

CSceneMain::~CSceneMain()
{
}

bool CSceneMain::Init()
{
	if (!CScene::Init()) { return false; }

	//mAssetManager->LoadSound("Hit", "Effect", false, "Sound/Fire1.wav");



	CTileMapObj* BackTileObject = CreateObj<CTileMapObj>("BackTileObj");
	BackTileObject->Load("test.tlm");
	BackTileObject->SetDebugLines(false);


	CAskardStatue* statue = CreateObj<CAskardStatue>("CAskardStatue");
	statue->SetWorldPos(50, -200,10);


	CTorchStand* Stand = CreateObj<CTorchStand>("TorchStand");

	Stand->SetWorldPos(-20, -60);

	mInventoryWindow = mUIManager->CreateWidget<CInventoryWindow>("UI_InventoryWindow");
	mUIManager->AddToViewport(mInventoryWindow);

	mPlayer = CreateObj<CPlayerObject>("Player");

	if (mPlayer == nullptr)
		return false;


	CMainCamera* MainCamera = CreateObj<CMainCamera>("MainCamera");
	MainCamera->SetOwner(mPlayer);

	CMoleGuard* monster = CreateObj<CMoleGuard>("Monster");
	monster->SetWorldPos(80.f, 0.f);




	//CRenderManager::GetInst()->SetBloomIntensity(1.1f);

	CRenderManager::GetInst()->UpdatePostCB_Tone(0.1, .2, 1,FVector3D(0.55, 0.48, 0.35));
	return true;
}

void CSceneMain::ToggleinventoryUI()
{
	mInventoryWindow->ToggleInventoryUI();
}

void CSceneMain::SetInventoty(CInventoryComponent* _inven)
{
	CInventoryPanel* panel = mInventoryWindow->GetInventoryPanel();

	panel->SetInventoty(_inven);
}

void CSceneMain::UpdateInventoty()
{
	CInventoryPanel* panel = mInventoryWindow->GetInventoryPanel();
	panel->UpdateSlotUI();
}

void CSceneMain::MoveItem(int f, int s)
{
	mPlayer->Getinventory()->MoveItem(f, s);
}

