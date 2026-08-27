#include "GravitySkill.h"
#include "MonsterObject.h"

#include "../Scene/Scene.h"

#include "../Component/MovementComponent.h"
#include "../Component/StaticMeshComponent.h"

CGravitySkill::CGravitySkill() :CSceneObject()
{
}

CGravitySkill::CGravitySkill(const CGravitySkill& Obj) :CSceneObject()
{
}

CGravitySkill::CGravitySkill(CGravitySkill&& Obj) :CSceneObject()
{
}

CGravitySkill::~CGravitySkill()
{
}

bool CGravitySkill::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CStaticMeshComponent>();

	mRoot->SetMesh("CenterRect");
	mRoot->SetShader("ColorMeshShader");

	mRoot->SetWorldScale(100.f, 100.f, 0.f);

	SetRootComponent(mRoot);

	return true;
}

void CGravitySkill::PreUpdate(float DeltaTime)
{
	CSceneObject::PreUpdate(DeltaTime);

	std::list<CSharedPtr<CSceneObject>> tempObjects;
	tempObjects = mScene->FindObjectsforClass<CMonsterObject>();

	for (auto& object : tempObjects)
	{
		if (auto monster = dynamic_cast<CMonsterObject*>(object.Get()))
		{
			FVector3D dir = GetWorldPosition() - monster->GetWorldPosition();
			dir.Normalize();

			if (mreverse)
			{
				monster->mMovement->SetMoveSpeed(100.f);
			}
			else
			{
				monster->mMovement->SetMoveSpeed(-100.f);
			}
			monster->mMovement->AddMove(dir);
		}
	}
}