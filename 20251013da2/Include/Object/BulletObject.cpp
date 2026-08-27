#include "BulletObject.h"

#include "../Component/StaticMeshComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Component/SpriteComponent.h"
#include "../Share/Log.h"

CBulletObject::CBulletObject()
	:CSceneObject()
{
}

CBulletObject::CBulletObject(const CBulletObject& Obj)
	:CSceneObject(Obj)
{
}

CBulletObject::CBulletObject(CBulletObject&& Obj)
	:CSceneObject(Obj)
{
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetBulletCollisionProfile(const std::string& Name)
{
	mBody->SetCollisionProfile(Name);
}


bool CBulletObject::Init()
{
	CSceneObject::Init();

	//mRoot = CreateComponent<CStaticMeshComponent>();
	mRoot = CreateComponent<CSpriteComponent >();
	mBody = CreateComponent<CColliderAABB2D>();
	mMovement = CreateComponent<CMovementComponent>();

	//mRoot->SetMesh("CenterRect");
	//mRoot->SetShader("ColorMeshShader");

	mRoot->SetTexture("CenterRect");
	mRoot->SetShader("SpriteShader");

	mRoot->SetWorldScale(50.f, 50.f, 1.f);


	mBody->SetCollisionBeginFunc<CBulletObject>(this, &CBulletObject::CollisionBullet);


	SetRootComponent(mRoot);

	mRoot->AddChild(mBody);
	mBody->SetBoxSize(50.f, 50.f);



	//Movement 셋팅 
	mMovement->SetUpdateComponent(mRoot);
	mMovement->SetMoveAxis(EAxis::Y);
	mMovement->SetMoveSpeed(500.f);


	return true;
}

void CBulletObject::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	//FVector3D Pos = mRoot->GetWorldPosition();
	////이동할 위치의 새로운 위치값 = 내 위치 + 내Y축 * 속도 * DeltaTime 
	//mRoot->SetWorldPos(Pos + mRootComponent->GetAxis(EAxis::Y) * mSpeed * DeltaTime);
}

void CBulletObject::CollisionBullet(const FVector3D& HitPoint, CColliderBase* Dest)
{
	CLog::PrintLog("Bullet Hit Collision", ELogPrintType::All);

	//Dest->GetOwner()->Damage(1.f, this);

	Destroy();
}