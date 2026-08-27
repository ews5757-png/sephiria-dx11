#include "StartWidget.h"

#include "../../Device.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneMain.h"
#include "../../Scene/SceneEditor.h"
#include "../../Scene/SceneManager.h"
#include "../../Scene/SceneUIManager.h"
#include "../Common/Button.h"
#include "../Common/Image.h"
#include "../../Share/Log.h"
#include "../../Asset/Texture/Texture.h"
#include "../../Asset/Texture/TextureManager.h"
#include "../../Asset/AssetManager.h"
#include "../../Scene/SceneAssetManager.h"
#include "../../Asset/Animation/Animation2DData.h"
#include "../../Asset/Animation/Animation2DManager.h"
#include "../Common/TextBlock.h"

CStartWidget::CStartWidget()
{
}

CStartWidget::~CStartWidget()
{

}

bool CStartWidget::Init()
{
	CUserWidget::Init();

	RS = CDevice::GetInst()->GetResolution();


	mButton = mScene->GetUIManager()->CreateWidget<CButton>("TestButton");
	mEditorButton = mScene->GetUIManager()->CreateWidget<CButton>("SceneEditorButton");


	CSharedPtr<CImage> Title = mScene->GetUIManager()->CreateWidget<CImage>("Title");
	Title->SetTexture("N_Title");
	Title->SetUseFrameNativeSize(true,2);   // ✅ 텍스처 원본 크기 사용
	Title->SetPivot(0.5f, 0.f);          // 중앙 기준
	Title->SetPos(RS.Width * 0.5f, RS.Height * 0.4f-30);
	Title->SetZOrder(10);
	AddWidget(Title);

	CSharedPtr<CImage> Title2 = mScene->GetUIManager()->CreateWidget<CImage>("Titlere");
	Title2->SetTexture("N_Title");
	Title2->SetSize(237 *2, 76 *-2);
	Title2->SetPivot(0.5f, 0.f);       
	Title2->SetPos(RS.Width * 0.5f, RS.Height * 0.4f + -30);
	Title2->SetZOrder(10);
	Title2->SetTint(1, 1, 1, 0.3);
	AddWidget(Title2);

	CSharedPtr<CImage> TitleBack = mScene->GetUIManager()->CreateWidget<CImage>("TitleBack");
	TitleBack->SetTexture("N_Title_Back");
	//Title->SetUseFrameNativeSize(true, 2);   // ✅ 텍스처 원본 크기 사용
	TitleBack->SetSize((float)RS.Width, (float)RS.Height);
	TitleBack->SetPivot(0.5f, 0.5f);          // 중앙 기준
	TitleBack->SetPos(RS.Width * 0.5f, RS.Height * 0.5f);
	TitleBack->SetZOrder(-1);
	TitleBack->SetTint(0.4, 0.4, 0.5, 1);
	AddWidget(TitleBack);

	CSharedPtr<CImage> Star = mScene->GetUIManager()->CreateWidget<CImage>("Star");
	Star->SetTexture("Star");
	Star->SetUseFrameNativeSize(true, 4);   // ✅ 텍스처 원본 크기 사용
	//Star->SetSize((float)RS.Width, (float)RS.Height * 0.25f);
	Star->SetPivot(0.f, 1.f);          // 중앙 기준
	Star->SetPos(0, (float)RS.Height);
	Star->SetZOrder(0);
	AddWidget(Star);

	////////////
	CSharedPtr<CImage> TitleTreeBegine = mScene->GetUIManager()->CreateWidget<CImage>("TitleTreeBegine");

	CAnimation2DData* anim = mScene->GetAssetManager()->FindAnimation("TitleTreeBegine");
	CTexture* tex = anim ? anim->GetTexture() : nullptr;
	if (!tex)
		return false;

	// 1) 텍스처 지정 (여러 장 들어있는 CTexture)
	TitleTreeBegine->SetTexture(tex);

	// 2) 프레임 개수만큼 “더미 프레임” 채우기 (이게 없으면 CImage가 프레임 0이라 재생이 안 됨)
	const int frameCount = tex->GetTextureCount();
	const float w = (float)tex->GetTexture(0)->Width;
	const float h = (float)tex->GetTexture(0)->Height;

	for (int i = 0; i < frameCount; ++i)
		TitleTreeBegine->AddBurshFrame(0.f, 0.f, w, h);


	TitleTreeBegine->SetUseFrameNativeSize(true, 2);  // 프레임 원본 크기 자동
	// 3) 화면에 보이게 기본 세팅
	//Back->SetSize((float)RS.Width, (float)RS.Height);
	TitleTreeBegine->SetPos((float)RS.Width / 2, (float)RS.Height / 2+40);
	TitleTreeBegine->SetPivot(0.5f, 0.f);

	TitleTreeBegine->SetBrushAnimation(true);
	TitleTreeBegine->SetAnimationPlayTime(1.6f);
	TitleTreeBegine->SetAnimationPlayRate(1.f);

	// 5) 등록
	TitleTreeBegine->SetZOrder(5);
	AddWidget(TitleTreeBegine);

	TitleTreeBegine->SetAnimationLoop(false);                 // ✅ 루프 끄기(마지막 프레임에서 멈춤)
	TitleTreeBegine->SetEndFunction(this, &CStartWidget::RequestAddTree);  // ✅ 끝나면 호출


	//////


	mButton->SetZOrder(10);
	mEditorButton->SetZOrder(10);

	AddWidget(mButton);
	AddWidget(mEditorButton);


	mButton->SetPos(640.f, 200.f);
	mButton->SetSize(100.f, 100.f);
	mButton->SetPivot(FVector2D(0.5f, 0.5f));
	//mButton->SetZOrder(1);

	mButton->SetTexture(EButtonState::Normal, "StartButton", TEXT("Texture/Start.png"));
	mButton->SetTexture(EButtonState::Hovered, "StartButton");
	mButton->SetTexture(EButtonState::Click, "StartButton", TEXT("Texture/Start.png"));
	mButton->SetTexture(EButtonState::Disable, "StartButton", TEXT("Texture/Start.png"));

	mButton->SetTint(EButtonState::Normal, 0.8f, 0.8f, 0.8f);
	mButton->SetTint(EButtonState::Hovered, 1.f, 1.f, 1.f);
	mButton->SetTint(EButtonState::Click, 0.5f, 0.5f, 0.5f);
	mButton->SetTint(EButtonState::Disable, 0.1f, 0.1f, 0.1f);

	mButton->SetEventCallBack(EButtonEventState::Click, this, &CStartWidget::StartButtonClick);


	// 에디터 모드 가는 버튼 
	mEditorButton->SetPos(640.f, 100.f);
	mEditorButton->SetSize(100.f, 100.f);
	mEditorButton->SetPivot(FVector2D(0.5f, 0.5f));
	mEditorButton->SetTexture(EButtonState::Normal, "EditButton", TEXT("Texture/Edit.png"));
	mEditorButton->SetTexture(EButtonState::Hovered, "EditButton");
	mEditorButton->SetTexture(EButtonState::Click, "EditButton", TEXT("Texture/Edit.png"));
	mEditorButton->SetTexture(EButtonState::Disable, "EditButton", TEXT("Texture/Edit.png"));
	mEditorButton->SetTint(EButtonState::Normal, 0.8f, 0.8f, 0.8f);
	mEditorButton->SetTint(EButtonState::Hovered, 1.f, 1.f, 1.f);
	mEditorButton->SetTint(EButtonState::Click, 0.5f, 0.5f, 0.5f);
	mEditorButton->SetTint(EButtonState::Disable, 0.1f, 0.1f, 0.1f);

	mEditorButton->SetEventCallBack(EButtonEventState::Click, this, &CStartWidget::StartEidtButtonClick);

	return true;
}

void CStartWidget::Update(float DeltaTime)
{
	CUserWidget::Update(DeltaTime);
	if (mRequestAddTree)
	{
		AddTree();
	}
}


void CStartWidget::StartButtonClick()
{

	CSceneManager::GetInst()->CreateLoadScene<CSceneMain>();
}

void CStartWidget::StartEidtButtonClick()
{

	CSceneManager::GetInst()->CreateLoadScene<CSceneEditor>();
}

void CStartWidget::AddTree()
{

	mRequestAddTree = false;

	CSharedPtr<CImage> Title_tree = mScene->GetUIManager()->CreateWidget<CImage>("Back");

	CAnimation2DData* anim = mScene->GetAssetManager()->FindAnimation("Title_Tree");
	CTexture* tex = anim ? anim->GetTexture() : nullptr;
	if (!tex)
		return;

	// 1) 텍스처 지정 (여러 장 들어있는 CTexture)
	Title_tree->SetTexture(tex);

	// 2) 프레임 개수만큼 “더미 프레임” 채우기 (이게 없으면 CImage가 프레임 0이라 재생이 안 됨)
	const int frameCount = tex->GetTextureCount();
	const float w = (float)tex->GetTexture(0)->Width;
	const float h = (float)tex->GetTexture(0)->Height;

	for (int i = 0; i < frameCount; ++i)
		Title_tree->AddBurshFrame(0.f, 0.f, w, h);


	Title_tree->SetUseFrameNativeSize(true, 2);  // 프레임 원본 크기 자동
	// 3) 화면에 보이게 기본 세팅
	//Back->SetSize((float)RS.Width, (float)RS.Height);
	Title_tree->SetPos((float)RS.Width / 2, (float)RS.Height / 2+40);
	Title_tree->SetPivot(0.5f, 0.f);
	Title_tree->SetTint(0.9, 0.9, 0.9, 1);

	Title_tree->SetBrushAnimation(true);
	Title_tree->SetAnimationPlayTime(1.f);
	Title_tree->SetAnimationPlayRate(1.f);

	// 5) 등록
	Title_tree->SetZOrder(5);
	AddWidget(Title_tree);




	Title_tree->SetZOrder(7);
}

void CStartWidget::RequestAddTree()
{
	mRequestAddTree = true;
}

