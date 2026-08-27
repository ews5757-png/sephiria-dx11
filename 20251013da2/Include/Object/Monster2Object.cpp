#include "Monster2Object.h"
#include "../Component/StaticMeshComponent.h"
#include "BulletObject.h"
#include "../Scene/Scene.h"

bool CMonster2Object::Init()
{
	CMonsterObject::Init();
	return true;
};
void CMonster2Object::Update(float DeltaTime)
{
	CMonsterObject::Update(DeltaTime);
	if (mFireTime == 1)
	{
		mCount++;
		if (mCount ==3)
		{
			mCount = 0;

			CBulletObject* Bullet = mScene->CreateObj<CBulletObject>("Bullet");
			Bullet->SetBulletCollisionProfile("MonsterAttack");
			Bullet->SetWorldScale(50.f, 50.f);
			Bullet->SetWorldRotation(GetWorldRotation() -FVector3D(0.f,0.f,45.f));
			Bullet->SetWorldPos(GetWorldPosition());

			Bullet->SetLifeTime(2.f);

			Bullet = mScene->CreateObj<CBulletObject>("Bullet2");
			Bullet->SetBulletCollisionProfile("MonsterAttack");
			Bullet->SetWorldScale(50.f, 50.f);
			Bullet->SetWorldRotation(GetWorldRotation() + FVector3D(0.f, 0.f, 45.f));
			Bullet->SetWorldPos(GetWorldPosition());

			Bullet->SetLifeTime(2.f);
		}
	}
};