#include "MoleGuard.h"
#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Component/ColliderBase.h"
#include "../Component/MovementComponent.h"
#include "../Component/RotationComponent.h"
#include "../Component/ColliderSphere2D.h"
#include "../Share/Log.h"
#include "../Scene/Scene.h"
#include "WarningLine.h"
#include "PlayerObject.h"
#include "../Share/Timer.h"

CMoleGuard::CMoleGuard()
{
}

CMoleGuard::CMoleGuard(const CMoleGuard& Obj)
{
}

CMoleGuard::CMoleGuard(CMoleGuard&& Obj)
{
}

CMoleGuard::~CMoleGuard()
{
}

bool CMoleGuard::Init()
{
	CMonsterBase::Init();

	// 컴포넌트 생성 및 설정
	mRoot = CreateComponent<CSpriteComponent>();
	mBody = CreateComponent<CColliderAABB2D>();
	mMovement = CreateComponent<CMovementComponent>();
	mRotation = CreateComponent<CRotationComponent>();
	mDetectionRange = CreateComponent<CColliderSphere2D>();

	SetRootComponent(mRoot);
	mRoot->AddChild(mDetectionRange);
	mRoot->AddChild(mBody);

	// 기본 설정
	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.5f);
	mRoot->SetOpacity(1.f);
	mRoot->SetWorldScale(320.f / 2 / 5, 290.f / 2 / 5, 1.f);

	// 충돌체 설정
	mBody->SetBoxSize(10.f, 10.f);
	mBody->SetCollisionProfile("Monster");
	mBody->SetName("MonsterBody");

	mDetectionRange->SetRadius(60.f);
	mDetectionRange->SetCollisionProfile("MonsterDetection");
	mDetectionRange->SetCollisionBeginFunc<CMoleGuard>(this, &CMoleGuard::SetTarget);

	// 이동 및 회전 설정
	mMovement->SetUpdateComponent(mRoot);
	mMovement->SetMoveSpeed(10.f);
	mRotation->SetUpdateComponent(mRoot);

	// 애니메이션 설정
	mAnimation = mRoot->CreateAnimation2D<CAnimation2D>();
	mAnimation->AddSequence("MoleGuard_Idle", 1.f, 1.f, true, false);
	mAnimation->AddSequence("MoleGuard_Move", 1.f, 1.f, true, false);
	mAnimation->AddSequence("MoleGuard_Attack", 1.f, 1.f, true, false);
	mAnimation->AddSequence("MoleGuard_Down", 1.f, 1.f, false, false);


	mAnimation->SetLoop("MoleGuard_Attack", false);
	mAnimation->SetLoop("MoleGuard_Down", false);

	mAnimation->SetEndFunction("MoleGuard_Attack", this, &CMoleGuard::EndAttack);
	mAnimation->AddNotify("MoleGuard_Attack", 4, this, &CMoleGuard::TriggerAttack);

	return true;
}

void CMoleGuard::SetTarget(const FVector3D& pos, CColliderBase* obj)
{
	if (dynamic_cast<CPlayerObject*>((obj)->GetOwner()))
	{
		mTarget = (obj)->GetOwner();
	}
}

void CMoleGuard::Update(float Deltatime)
{
	CMonsterBase::Update(Deltatime);
}

//idle
void CMoleGuard::StartIdle()
{
	mAnimation->ChangeAnimation("MoleGuard_Idle");
}

void CMoleGuard::UpdateIdle(float DeltaTime)
{
}

void CMoleGuard::EndIdle()
{
}
//wander
void CMoleGuard::StartWander()
{
}
void CMoleGuard::UpdateWander(float DeltaTime)
{
}
void CMoleGuard::EndWander()
{
}
//chase
void CMoleGuard::StartChase()
{
	if (mTarget)
	{
		mAnimation->ChangeAnimation("MoleGuard_Move");
	}
}

void CMoleGuard::UpdateChase(float DeltaTime)
{
	if (mTarget)
	{
		MoveTo(mTarget);
	}
}

void CMoleGuard::EndChase()
{
}
void CMoleGuard::StartConfront()
{
	mAnimation->ChangeAnimation("MoleGuard_Move");

	float randX = (rand() % 40) - 20.f;
	float randY = (rand() % 40) - 20.f;

	FVector3D offset = FVector3D(randX, randY, 0.f);

	if (offset.Length() < 10.f)
	{
		offset.Normalize();
		offset *= 10.f;
	}

	mMoveTarget = GetWorldPosition() + offset;
}

void CMoleGuard::UpdateConfront(float DeltaTime)
{
	if (GetWorldPosition().Distance(mMoveTarget) >= 2.f)
	{
		MoveTo(mMoveTarget);
	}
	else
	{
		EndConfront();
	}
}

void CMoleGuard::EndConfront()
{
	ChangeState(EMonsterState::Idle);
}

void CMoleGuard::StartAttackReady()
{
	// 공격 방향 확정 및 경고선 표시
	mLastTargetDir = mTargetdir;
	mAnimation->ChangeAnimation("MoleGuard_Attack");
	InitLine();
}

void CMoleGuard::UpdateAttackReady(float DeltaTime)
{
}

void CMoleGuard::EndAttackReady()
{
}

void CMoleGuard::TriggerAttack()
{
	ChangeState(EMonsterState::Attack);
}

void CMoleGuard::StartAttack()
{
	mLockRotation = true;
}

void CMoleGuard::UpdateAttack(float DeltaTime)
{

	const float DASH_DURATION = 0.3f;

	if (mStateTime < DASH_DURATION)
	{
		mMovement->AddMove(mLastTargetDir);
		float dashSpeedFactor = cos(TODIG(mStateTime / DASH_DURATION * 90.0f));
		mMovement->SetMoveSpeed(dashSpeedFactor * 300.0f);
	}
	else
	{
		EndAttack();
	}
}

void CMoleGuard::EndAttack()
{
	mMovement->SetMoveSpeed(20.f);
	ChangeState(EMonsterState::Recovery);
}

void CMoleGuard::StartRecovery()
{
}

void CMoleGuard::UpdateRecovery(float DeltaTime)
{
	if (mStateTime > 0.5f)
	{
		EndRecovery();
	}
}

void CMoleGuard::EndRecovery()
{
	mLockRotation = false;
	ChangeState(EMonsterState::Idle);
}

void CMoleGuard::StartDelay()
{
	mAnimation->ChangeAnimation("MoleGuard_Idle");

	float randNum = rand() % 6;
	mDelyTargetTime = 0.5f + randNum / 10.f;
}

void CMoleGuard::UpdateDelay(float DeltaTime)
{

	if (mStateTime > mDelyTargetTime)
	{
		EndDelay();
	}
}

void CMoleGuard::EndDelay()
{
	ChangeState(EMonsterState::Idle);
}

void CMoleGuard::StartHit()
{
	mAnimation->ChangeAnimation("MoleGuard_Down");
}

void CMoleGuard::UpdateHit(float DeltaTime)
{
	if (mStateTime < 0.3f)
	{

	}
	else
	{
		EndHit();
	}
}

void CMoleGuard::EndHit()
{
	ChangeState(EMonsterState::Idle);
}

void CMoleGuard::StartDeath()
{
	CMonsterBase::StartDeath();
}

void CMoleGuard::UpdateDeath(float DeltaTime)
{
	CMonsterBase::UpdateDeath(DeltaTime);
}

void CMoleGuard::EndDeath()
{
	CMonsterBase::EndDeath();
}

void CMoleGuard::StartHitEffect()
{
	SetTintColor(1, 0, 0);
}

void CMoleGuard::UpdateHitEffect(float DeltaTime)
{
	if (mEffectStateTime < 0.2f)
	{
		float tmepsin = sin(TODIG(180.f * mEffectStateTime / 0.2f));
		{
			SetRelativeScale(320.f / 2 *(1 - tmepsin*0.2) / 5, (290.f / 2) * (1 + tmepsin*0.3) / 5, 1.f);
		}
	}
	else
	{
		EndHitEffect();
	}
}

void CMoleGuard::EndHitEffect()
{
	SetRelativeScale(320.f / 2 /5, 290.f / 2 /5, 1.f);
	if (mIsDeath)
	{
		ChangeEffectState(EMonsterEffectState::Death);
	}
	else
	{
		SetTintColor(1, 1, 1);
		ChangeEffectState(EMonsterEffectState::None);
	}
}

void CMoleGuard::StartDeathEffect()
{
}

void CMoleGuard::UpdateDeathEffect(float DeltaTime)
{
}

void CMoleGuard::EndDeathEffect()
{
}

void CMoleGuard::AI(float DeltaTime)
{
	if (mTarget)
	{
		if (mTargetdis <= 60.f)
		{
			RandomAction(DeltaTime);
		}
		else
		{
			ChangeState(EMonsterState::Chase);
		}
	}
	else
	{
		ChangeState(EMonsterState::Idle);
	}
}

void CMoleGuard::RandomAction(float DeltaTime)
{
	float randNum = rand() % 100;

	if (randNum < 20)
	{
		ChangeState(EMonsterState::AttackReady);
	}
	else if (randNum < 70)
	{
		if (mPrevState == EMonsterState::Delay)
		{
			ChangeState(EMonsterState::Confront);
		}
		else
		{
			ChangeState(EMonsterState::Delay);
		}
	}
	else // 30%
	{
		ChangeState(EMonsterState::Confront);
	}

}

void CMoleGuard::InitLine()
{
	if (mTarget)
	{
		mMoveTarget = mTarget->GetWorldPosition();
	}
	CWarningLine* warline = mScene->CreateObj<CWarningLine>("waringLine");

	warline->SetWorldPos(GetWorldPosition());

	float radian = atan2(mTargetdir.y, mTargetdir.x);
	float degree = radian * (180.f / 3.141592f);
	warline->SetRelativeRotationZ(degree + -90);

	warline->SetLength(10.f);
}
void CMoleGuard::OnDamage(const FDamageInfo& Info)
{
	CMonsterBase::OnDamage(Info);

	ChangeState(EMonsterState::Hit);
	ChangeEffectState(EMonsterEffectState::Hit);
}

void CMoleGuard::Death()
{

	CMonsterBase::Death();
}