#include "MonsterObject.h"
#include "../Component/StaticMeshComponent.h"
#include "BulletObject.h"
#include "../Scene/Scene.h"
#include "../Component/MovementComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Share/Log.h"

//문자열 
#include <sstream>

CMonsterObject::CMonsterObject()
{
}

CMonsterObject::CMonsterObject(const CMonsterObject& Obj)
	: CSceneObject(Obj)
{
}

CMonsterObject::CMonsterObject(CMonsterObject&& Obj)
	: CSceneObject(Obj)

{
}

CMonsterObject::~CMonsterObject()
{
}
void CMonsterObject::CollisionMonster(const FVector3D& HitPont, CColliderBase* Dest)
{
	CLog::PrintLog("CollisionBegin");

	//Dest->GetProfile()->Channel == ECollisionChannel::PlayerAttack();
}

void CMonsterObject::CollisionMonsterEnd(class CColliderBase* Dest)
{
	CLog::PrintLog("CollisionEnd");
}


float CMonsterObject::Damage(float Attack, CSceneObject* Obj)
{
	float Dmg = CSceneObject::Damage(Attack, Obj);

	mHP -= (int)Dmg;

	if (mHP <= 0)
	{
		Destroy();
	}

	return Dmg;
}



bool CMonsterObject::Init()
{
	if (!CSceneObject::Init())
	{
		return false;
	}

	mRoot = CreateComponent<CStaticMeshComponent>();
	mMovement = CreateComponent<CMovementComponent>();
	mRoot->SetMesh("CenterRect");
	mRoot->SetShader("ColorMeshShader");
	mRoot->SetWorldScale(10.f, 10.f);

	mBody = CreateComponent<CColliderAABB2D>();
	mRoot->AddChild(mBody);
	mBody->SetBoxSize(100.f, 100.f);
	mBody->SetCollisionProfile("Monster");
	mBody->SetCollisionBeginFunc<CMonsterObject>(this, &CMonsterObject::CollisionMonster);

	SetRootComponent(mRoot);
	mMovement->SetUpdateComponent(mRoot);

	return true;
}
void CMonsterObject::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	if (!mTarget->IsActive())
	{
		mTarget = nullptr;
	}
	else if (mTarget->IsEnable())
	{
		//타겟을 바라보는 벡터
		//내가 타겟을 바라보는 방향 벡터는 ?
		// 타겟 위치 - 자기자신 위치 == 방향벡터 
		//FVector3D ViewDir = mTarget->GetWorldPosition() - GetWorldPosition();
		//// 단위벡터로 만들어준다. (단위벡터 : 길이가 1인 벡터 )
		//ViewDir.Normalize();

		//float Angle = FVector3D::Axis[EAxis::Y].GetAngle(ViewDir);

		float Angle = GetWorldPosition().GetViewTargetAngle(mTarget->GetWorldPosition());

		// 출력창 로그찍기 
		//std::wostringstream oss;
		//oss << L"Angle :" << Angle << L"\n";
		//OutputDebugStringW(oss.str().c_str());

		SetWorldRotationZ(Angle);
	}

	//발사하기 샘플 
	mFireTime -= DeltaTime;
	if (mFireTime <= 0)
	{
		mFireTime = 1.f;

		CBulletObject* Bullet = mScene->CreateObj<CBulletObject>("Bullet");
		Bullet->SetBulletCollisionProfile("MonsterAttack");
		Bullet->SetWorldScale(50.f, 50.f);
		Bullet->SetWorldRotation(GetWorldRotation());
		Bullet->SetWorldPos(GetWorldPosition());

		Bullet->SetLifeTime(2.f);
	}
}