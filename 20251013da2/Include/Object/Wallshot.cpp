#include "Wallshot.h"


#include"../Component/StaticMeshComponent.h"
#include"../Component/ColliderSphere2D.h"
#include"../Component/MovementComponent.h"
#include"../Component/RotationComponent.h"

#include "MonsterObject.h"

CWallshot::CWallshot()
{
}
CWallshot::CWallshot(const CWallshot& Obj)
{
}
CWallshot::CWallshot(CWallshot&& Obj)
{
}
CWallshot::~CWallshot()
{
}

bool CWallshot::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CStaticMeshComponent>();
	mBody = CreateComponent<CColliderSphere2D>();
	mMovement = CreateComponent<CMovementComponent>();
	mRotation = CreateComponent<CRotationComponent>();


	mRoot->SetMesh("CenterRect");
	mRoot->SetShader("ColorMeshShader");
	mRoot->SetWorldPos(0.f, 0.f, 0.f);
	mRoot->SetWorldScale(50.f, 50.f, 0.f);

	mBody->SetCollisionBeginFunc<CWallshot>(this, &CWallshot::CollisionBullet);

	SetRootComponent(mRoot);

	mRoot->AddChild(mBody);
	mBody->SetRadius(50.f);

	mBody->SetCollisionProfile("PlayerAttack");

	mMovement->SetUpdateComponent(mRoot);
	mMovement->SetMoveAxis(EAxis::Y);
	mMovement->SetMoveSpeed(500.f);

	mRotation->SetUpdateComponent(mRoot);
	mRotation->SetVelocityInit(1.f);


	return true;
}
void CWallshot::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	mMovement->AddMove(mRootComponent->GetAxis(EAxis::Y));
}

void CWallshot::CollisionBullet(const FVector3D& HitPoint, CColliderBase* Dest)
{
	Dest->GetOwner()->Damage(1.f, Dest->GetOwner());
	if (count <= 0)
	{
		Destroy();
	}
	else
	{
		count--;
	}
}

