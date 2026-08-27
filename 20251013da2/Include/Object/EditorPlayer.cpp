#include "EditorPlayer.h"
#include "../Component/CameraComponent.h"
#include "../Component/MovementComponent.h"
#include "../Scene/Input.h"
#include "../Scene/Scene.h"
#include "../Share/Log.h"

CEditorPlayer::CEditorPlayer()
{
}

CEditorPlayer::CEditorPlayer(const CEditorPlayer& Obj)
	: CSceneObject(Obj)
{
}

CEditorPlayer::CEditorPlayer(CEditorPlayer&& Obj)
	: CSceneObject(Obj)
{
}

CEditorPlayer::~CEditorPlayer()
{
}

bool CEditorPlayer::Init()
{
	CSceneObject::Init();

	mCamera = CreateComponent<CCameraComponent>();
	mMovement = CreateComponent<CMovementComponent>();

	//카메라 타입을 직교로 변경해준다. 
	mCamera->SetProjectionType(ECameraProjectionType::Ortho);

	SetRootComponent(mCamera);

	mMovement->SetUpdateComponent(mCamera);
	mMovement->SetMoveSpeed(500.f);

	mScene->GetInput()->AddBindKey("MoveUp", 'W');
	mScene->GetInput()->AddBindKey("MoveDown", 'S');
	mScene->GetInput()->AddBindKey("MoveLeft", 'A');
	mScene->GetInput()->AddBindKey("MoveRight", 'D');

	mScene->GetInput()->AddBindKey("ZoomIn", DIK_MOUSEWHEELUP);
	mScene->GetInput()->AddBindKey("ZoomOut", DIK_MOUSEWHEELDOWN);

	mScene->GetInput()->AddBindFunction("MoveUp", EInputType::Hold, this, &CEditorPlayer::MoveUp);
	mScene->GetInput()->AddBindFunction("MoveDown", EInputType::Hold, this, &CEditorPlayer::MoveDown);
	mScene->GetInput()->AddBindFunction("MoveLeft", EInputType::Hold, this, &CEditorPlayer::MoveLeft);
	mScene->GetInput()->AddBindFunction("MoveRight", EInputType::Hold, this, &CEditorPlayer::MoveRight);


	mScene->GetInput()->AddBindFunction("ZoomIn", EInputType::Down, this, &CEditorPlayer::ZoomIn);
	mScene->GetInput()->AddBindFunction("ZoomOut", EInputType::Down, this, &CEditorPlayer::ZoomOut);

	return true;
}

void CEditorPlayer::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	std::string PosLog = "X : " + std::to_string(GetWorldPosition().x) + "    Y : " + std::to_string(GetWorldPosition().y);
}

void CEditorPlayer::MoveUp(float DeltaTime)
{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::Y));
}

void CEditorPlayer::MoveDown(float DeltaTime)
{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::Y) * -1.f);
}

void CEditorPlayer::MoveLeft(float DeltaTime)
{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::X) * -1.f);
}

void CEditorPlayer::MoveRight(float DeltaTime)
{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::X));
}

void CEditorPlayer::ZoomIn(float Deltatime)
{
	mCamera->SetZoom(0.1);
}

void CEditorPlayer::ZoomOut(float Deltatime)
{
	mCamera->SetZoom(-0.1);
}
