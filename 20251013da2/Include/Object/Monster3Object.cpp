#include "Monster3Object.h"
#include "../Component/StaticMeshComponent.h"
#include "BulletObject.h"
#include "../Scene/Scene.h"

bool CMonster3Object::Init()
{
	CMonsterObject::Init();
	return true;
};
void CMonster3Object::Update(float DeltaTime)
{
	CMonsterObject::Update(DeltaTime);
	if (mTarget->GetWorldPosition().Distance(GetWorldPosition()) < 500.f)
	{
	}
	else
	{
		mFireTime += DeltaTime;
		if (mFireTime > 1 )
		{
			mFireTime = 1;
		}
		SetWorldRotation(FVector3D(0.f, 0.f, 0.f));
	}
};