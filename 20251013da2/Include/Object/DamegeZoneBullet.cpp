#include "DamegeZoneBullet.h"

#include"../Component/StaticMeshComponent.h"
#include"../Component/ColliderSphere2D.h"

#include "MonsterObject.h"

CDamegeZoneBullet::CDamegeZoneBullet()
{
}
CDamegeZoneBullet::CDamegeZoneBullet(const CDamegeZoneBullet& Obj)
{
}
CDamegeZoneBullet::CDamegeZoneBullet(CDamegeZoneBullet&& Obj)
{
}
CDamegeZoneBullet::~CDamegeZoneBullet()
{
}

bool CDamegeZoneBullet::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CStaticMeshComponent>();
	mBody = CreateComponent<CColliderSphere2D>();


	mRoot->SetMesh("CenterRect");
	mRoot->SetShader("ColorMeshShader");
	mRoot->SetWorldPos(0.f, 0.f, 0.f);
	mRoot->SetWorldScale(50.f, 50.f, 0.f);

	mBody->SetCollisionBeginFunc<CDamegeZoneBullet>(this, &CDamegeZoneBullet::CollisionBullet);

	SetRootComponent(mRoot);

	mRoot->AddChild(mBody);
	mBody->SetRadius(300.f);

	mBody->SetCollisionProfile("PlayerAttack");

	return true;
}
void CDamegeZoneBullet::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	mElapsedTime += DeltaTime;

	if (mElapsedTime > mDelayTime)
	{
		mElapsedTime -= mDelayTime;
		Attack();
	}
}

void CDamegeZoneBullet::CollisionBullet(const FVector3D& HitPoint, CColliderBase* Dest)
{
	if (dynamic_cast<CMonsterObject*>(Dest->GetOwner()))
	{
		objects.push_back(Dest->GetOwner());
	}
}
void CDamegeZoneBullet::Attack()
{
	for (auto iter : objects)
	{
		iter->Damage(1.f, iter);
	}
}


