#include "MonsterBase.h"
#include "../Component/SpriteComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/RotationComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Component/ColliderSphere2D.h"

#include "../Scene/Scene.h"

#include "PlayerObject.h"
#include "SpriteEffect.h"

#include "../Share/Log.h"

CMonsterBase::CMonsterBase()
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& Obj)
{
}

CMonsterBase::CMonsterBase(CMonsterBase&& Obj)
{
}

CMonsterBase::~CMonsterBase()
{
}

bool CMonsterBase::Init()
{
	mBodyType = EBodyType::Dynamic;
	return true;
}

void CMonsterBase::ComponentInit()
{
}

void CMonsterBase::AnimationInit()
{
}

void CMonsterBase::SetTarget(const FVector3D& pos, CColliderBase* obj)
{
}

void CMonsterBase::PreUpdate(float DeltaTime)
{
	CSceneObject::PreUpdate(DeltaTime);
	mStateTime += DeltaTime;
	mEffectStateTime += DeltaTime;
	MovementUpdate(DeltaTime);

	if (CanThink())
	{
		AI(DeltaTime);
	}

	switch (mEffectState)
	{
	case EMonsterEffectState::Hit:		UpdateHitEffect(DeltaTime);		break;
	case EMonsterEffectState::Death:	UpdateDeathEffect(DeltaTime);	break;
	case EMonsterEffectState::None:										break;
	default:	break;
	}

	switch (mState)
	{
	case EMonsterState::Idle:		UpdateIdle(DeltaTime);		break;
	case EMonsterState::Wander:		UpdateWander(DeltaTime);	break;
	case EMonsterState::Chase:		UpdateChase(DeltaTime);		break;
	case EMonsterState::Confront:	UpdateConfront(DeltaTime);	break;
	case EMonsterState::AttackReady:UpdateAttackReady(DeltaTime); break;
	case EMonsterState::Attack:		UpdateAttack(DeltaTime);	break;
	case EMonsterState::Recovery:	UpdateRecovery(DeltaTime);	break;
	case EMonsterState::Hit:		UpdateHit(DeltaTime);	break;
	case EMonsterState::Death:		UpdateDeath(DeltaTime);	break;
	case EMonsterState::Delay:		UpdateDelay(DeltaTime);		break;
	default: break;
	}
}

void CMonsterBase::Update(float DeltaTime)
{

	CSceneObject::Update(DeltaTime);
}

void CMonsterBase::MovementUpdate(float DeltaTime)
{
	if (mMovement != nullptr && mMovement->GetVelocityLength() == 0.f)
	{
		mIsMove = false;
	}
	else
	{
		mIsMove = true;
	}

	if (mTarget)
	{
		mTargetdir = mTarget->GetWorldPosition() - GetWorldPosition();
		mTargetdis = mTargetdir.Length();
		mTargetdir.Normalize();
	}

	if (mLockRotation)
	{
		return;
	}

	float checkX = 0.f;

	if (mTarget)
	{
		checkX = mTargetdir.x;
	}
	else
	{
		checkX = mMoveDir.x;
	}

	if (!mIsDeath)
	{
		if (checkX > 0.f)
		{
			mRoot->SetFlip(false);
			mIsRight = true;
		}
		else if (checkX < 0.f)
		{
			mRoot->SetFlip(true);
			mIsRight = false;
		}
	}

	if (mState == EMonsterState::Chase|| mState == EMonsterState::Confront|| mState == EMonsterState::Wander && mIsMove)
	{
		mRunDustAccTime += DeltaTime;

		if (mRunDustAccTime >= 0.5f)
		{
			mRunDustAccTime -= 0.5f;

			CSpriteEffect* RunDust = mScene->CreateObj<CSpriteEffect>("RunDust");
			RunDust->SetAnimation("RunDust", 1.f, 3.f);;
			RunDust->SetWorldScale(7  , 7 );
			RunDust->GetRootComponent()->SetRenderOrder(10);

			FVector2D temp = (-mMoveDir*15);
			

			FVector2D dir = (-(GetWorldPosition()) + temp).Normalize();



			// 수직 벡터
			FVector2D perp(-dir.y, dir.x);

			if (mIsRight)
			{
				RunDust->SetWorldPos(GetWorldPosition() + temp + -perp *2);
			}
			else
			{
				RunDust->SetWorldPos(GetWorldPosition() + temp + perp * 2);
			}
		}
	}

}

void CMonsterBase::MovementAnimation()
{
}

void CMonsterBase::MoveTo(CSceneObject* obj)
{
	if (!obj) return;
	FVector2D target = obj->GetWorldPosition() - GetWorldPosition();
	target.Normalize();
	mMovement->AddMove(target);
}

void CMonsterBase::MoveTo(FVector2D pos)
{
	FVector2D target = pos - GetWorldPosition();
	target.Normalize();
	mMovement->AddMove(target);
}

bool CMonsterBase::CanThink()
{
	if (mState == EMonsterState::Idle ||
		mState == EMonsterState::Chase)
	{
		return true;
	}

	return false;
}

void CMonsterBase::RandomAction(float DeltaTime)
{
}

void CMonsterBase::ChangeState(EMonsterState NewState)
{

	if (mState == NewState) return;

	mPrevState = mState;
	mState = NewState;

	mStateTime = 0.f;

	switch (mState)
	{
	case EMonsterState::Idle:		StartIdle();		break;
	case EMonsterState::Wander:		StartWander();		break;
	case EMonsterState::Chase:		StartChase();		break;
	case EMonsterState::Confront:	StartConfront();	break;
	case EMonsterState::AttackReady:StartAttackReady(); break;
	case EMonsterState::Attack:		StartAttack();		break;
	case EMonsterState::Recovery:	StartRecovery();	break;
	case EMonsterState::Hit:		StartHit();			break;
	case EMonsterState::Death:		StartDeath();		break;
	case EMonsterState::Delay:		StartDelay();		break;
	default: break;
	}
}

void CMonsterBase::ChangeEffectState(EMonsterEffectState NewState)
{
	mEffectStateTime = 0.f;

	mEffectState = NewState;

	switch (mEffectState)
	{
	case EMonsterEffectState::Hit:		StartHitEffect();		break;
	case EMonsterEffectState::Death:	StartDeathEffect();		break;
	case EMonsterEffectState::None:		break;
	default:
		break;
	}
}

void CMonsterBase::SetTintColor(FVector4D color)
{
	mRoot->SetTint(color);
}
void CMonsterBase::SetTintColor(float a,float b ,float c)
{
	mRoot->SetTint(a,b,c);
}

void CMonsterBase::OnDamage(const FDamageInfo& Info)
{
}

void CMonsterBase::Death()
{
	CSceneObject::Death();
	StartDeath();
}

void CMonsterBase::StartDeath()
{
	mState = EMonsterState::Death;

	mRoot->SetTint(0.1, 0.1, 0.1);

	basePos = GetWorldPosition();
}
void CMonsterBase::UpdateDeath(float DeltaTime)
{
	const float TotalTime = 0.35;
	if (mStateTime < TotalTime)
	{
		float temp = sin(mStateTime / TotalTime * TODIG(180));
		float temp2 = sin(mStateTime / TotalTime * TODIG(90));

		if (0 < mLastAttackDir.x)
		{
			SetWorldPos(basePos + FVector3D(temp2 * 6, temp * 6, 0));
		}
		else
		{
			SetWorldPos(basePos + FVector3D(temp2 * -6, temp * 6, 0));
		}

	}
	else if (mStateTime >= TotalTime + 1.f)
	{
		EndDeath();
	}
}
void CMonsterBase::EndDeath()
{
	Destroy();
}
