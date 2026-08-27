#include "StoneMoleMelee.h"
#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Component/MovementComponent.h"
#include "../Component/RotationComponent.h"
#include "../Share/Log.h"

bool CStoneMoleMelee::Init()
{
	CMonsterBase::Init();
	mRoot = CreateComponent<CSpriteComponent>();

	mBody = CreateComponent<CColliderAABB2D>();

	mMovement = CreateComponent<CMovementComponent>();

	mRotation = CreateComponent<CRotationComponent>();
	SetRootComponent(mRoot);

	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.5f);
	mRoot->SetOpacity(1.f);

	mRoot->SetWorldPos(0.f, 0.f, 0.f);
	mRoot->SetWorldScale(210.f / 2, 140.f / 2, 1.f);
	mBody->SetBoxSize(50.f, 50.f);
	mBody->SetRelativePos(0.f,0.f);
	mBody->SetCollisionProfile("Monster");


	mRoot->AddChild(mBody);
	mMovement->SetUpdateComponent(mRoot);
	mMovement->SetMoveSpeed(100.f);

	mRotation->SetUpdateComponent(mRoot);
	mRotation->SetVelocityInit(1.f);



	mAnimation = mRoot->CreateAnimation2D<CAnimation2D>();

	mAnimation->AddSequence("Stone_Mole_Idle", 1.f, 1.f, true, false);
	mAnimation->AddSequence("Stone_Mole_Move", 1.f, 1.f, true, false);

	//mAnimation->ChangeAnimation("Stone_Mole_Idle");

	//mAnimation->SetLoop("Stone_Mole_Idle",true);

	return true;
}

void CStoneMoleMelee::Update(float Deltatime)
{
	float targetlength = 0;
	FVector3D dis;
	if (mTarget)
	{
		dis = mTarget->GetWorldPosition() - GetWorldPosition();
		targetlength = dis.Length();
	}
	if (targetlength > 100.f)
	{
		mMovement->AddMove(dis.Normalize());
	}


	if (mIsMove)
	{
		mAnimation->ChangeAnimation("Stone_Mole_Move");
	}
	else
	{
		mAnimation->ChangeAnimation("Stone_Mole_Idle");
	}

	if (mMoveDir.x > 0)
	{
		mRoot->SetFlip(false);
	}
	else
	{
		mRoot->SetFlip(true);
	}
	CMonsterBase::Update(Deltatime);
}
