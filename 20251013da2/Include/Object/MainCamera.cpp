#include "MainCamera.h"
#include "../Component/CameraComponent.h"
#include "../Component/MovementComponent.h"
#include "PlayerObject.h"

CMainCamera::CMainCamera()
{
}

CMainCamera::CMainCamera(const CMainCamera& Obj)
{
}

CMainCamera::CMainCamera(CMainCamera&& Obj)
{
}

CMainCamera::~CMainCamera()
{
}

bool CMainCamera::Init()
{
	mCamera = CreateComponent<CCameraComponent>();

	mMovement = CreateComponent<CMovementComponent>();

	mCamera->SetProjectionType(ECameraProjectionType::Ortho);

	SetRootComponent(mCamera);

	mMovement->SetUpdateComponent(mCamera);

	return true;
}

void CMainCamera::Update(float Deltatime)
{
	if (mOwner)
	{
		FVector3D temp = (mOwner->GetWorldPosition() - GetWorldPosition());
		//temp.z = 0.f; // 2D라면 Z축 무시
		temp.Normalize();

		float length = (mOwner->GetWorldPosition() - GetWorldPosition()).Length();
		mMovement->SetMoveSpeed(length*10);

		mMovement->AddMove(temp);
	}

	CSceneObject::Update(Deltatime);
}