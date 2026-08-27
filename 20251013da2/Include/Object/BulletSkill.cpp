#include "BulletSkill.h"

#include "../Component/StaticMeshComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/RotationComponent.h"

CBulletSkill::CBulletSkill()
	:CSceneObject()
{
}


CBulletSkill::CBulletSkill(const CBulletSkill& Obj)
	:CSceneObject(Obj)
{
}

CBulletSkill::CBulletSkill(CBulletSkill&& Obj)
	:CSceneObject(Obj)
{
}

CBulletSkill::~CBulletSkill()
{
}
bool CBulletSkill::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CSceneComponent>();
	mMovement = CreateComponent<CMovementComponent>();
	mRotate = CreateComponent<CRotationComponent>();

	mRoot->SetWorldScale(1.f, 1.f, 1.f);

	SetRootComponent(mRoot);

	//Movement 셋팅 

	const float dig = 360.0f / (float)mbulletCount;
	CStaticMeshComponent* tempbullet;
	CSceneComponent* temppivot;
	for (int i = 0; i < mbulletCount; i++)
	{
		temppivot = CreateComponent<CSceneComponent>();
		temppivot->SetRelativeRotationZ(dig * i);
		temppivot->SetRelativeScale(1.f, 1.f, 1.f);

		mRoot->AddChild(temppivot);
		mpivots.push_back(temppivot);

		tempbullet = CreateComponent<CStaticMeshComponent>();
		tempbullet->SetMesh("CenterRect");
		tempbullet->SetShader("ColorMeshShader");
		tempbullet->SetRelativeScale(50.f, 50.f, 1.f);


		temppivot->AddChild(tempbullet);
		mSkillObjects.push_back(tempbullet);
	}



	return true;
}


void CBulletSkill::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	if (mDig > 540.f)
	{
		for (auto& comp : mSkillObjects)
		{
			//comp->Release(); // CSceneComponent::Destroy()는 CObject::mActive=false
		}
		mSkillObjects.clear();
		mDig = 180.f;
		this->Destroy();
	}
	else
	{
		mDig += DeltaTime * 100.f;
		mSpeedScale = 1 + cos((mDig * 3.14f / 180.f) - (0.9) * sin(mDig * 3.14f / 180.f));

		
		for (int i = 0; i < mbulletCount; i++)
		{
			CSceneComponent* comp = mpivots[i];

			FVector3D axisY = comp->GetAxis(EAxis::Y);
			comp->SetRelativePos(axisY * mSpeedScale * 200.f);


			FVector3D Rot2 = mSkillObjects[i]->GetRelativeRotation();
			Rot2.z += mSpeedScale * 640.f * DeltaTime;
			mSkillObjects[i]->SetRelativeRotationZ(Rot2.z);


			mRotate->SetUpdateComponent(mSkillObjects[i]);
			mRotate->AddMoveZ(100);
		}


	}
}
