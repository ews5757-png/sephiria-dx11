#include "SceneStart.h"

#include "../UI/UserWidget/StartWidget.h"
#include "SceneUIManager.h"
#include "../Render/RenderManager.h"

CSceneStart::CSceneStart()
{

}

CSceneStart::~CSceneStart()
{

}

bool CSceneStart::Init()
{
	if (!CScene::Init())
	{
		return false;
	}



	//UI 생성
	CStartWidget* Widget = mUIManager->CreateWidget<CStartWidget>("Start");

	mUIManager->AddToViewport(Widget);



	CRenderManager::GetInst()->SetBloomIntensity(10.25f);
	CRenderManager::GetInst()->UpdatePostCB_Tone(0, 0, 1);
	return true;
}
