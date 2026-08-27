#pragma once
#include "SceneObject.h"

enum class EMonsterState
{
	Idle,
	Wander,
	Chase,
	Confront,
	AttackReady,
	Attack,
	Recovery,
	Hit,
	Death,
	Delay
};

enum class EMonsterEffectState
{
	None,
	Hit,
	Death
};

class CMonsterBase abstract : public CSceneObject
{
	friend class CScene;

protected:
	CMonsterBase();
	CMonsterBase(const CMonsterBase& Obj);
	CMonsterBase(CMonsterBase&& Obj);
	virtual ~CMonsterBase();

public:
	virtual bool Init() override;

	virtual void PreUpdate(float DeltaTime)override;
	virtual void Update(float DeltaTime) override;

	virtual void SetTarget(const FVector3D& pos, class CColliderBase* obj);

protected:
	virtual void ComponentInit();
	virtual void AnimationInit();

	virtual void MovementUpdate(float DeltaTime);
	virtual void MovementAnimation();
	void MoveTo(CSceneObject* obj);
	void MoveTo(FVector2D pos);

	virtual void AI(float DeltaTime) {}
	virtual bool CanThink();
	virtual void RandomAction(float DeltaTime);

	virtual void OnDamage(const FDamageInfo& Info);

	virtual void Death() override;

protected:
	virtual void ChangeState(EMonsterState NewState);
	virtual void ChangeEffectState(EMonsterEffectState NewState);

	virtual void StartIdle() {}
	virtual void UpdateIdle(float DeltaTime) {}
	virtual void EndIdle() {}

	virtual void StartWander() {}
	virtual void UpdateWander(float DeltaTime) {}
	virtual void EndWander() {}

	virtual void StartChase() {}
	virtual void UpdateChase(float DeltaTime) {}
	virtual void EndChase() {}

	virtual void StartConfront() {}
	virtual void UpdateConfront(float DeltaTime) {}
	virtual void EndConfront() {}

	virtual void StartAttackReady() {}
	virtual void UpdateAttackReady(float DeltaTime) {}
	virtual void EndAttackReady() {}

	virtual void StartAttack() {}
	virtual void UpdateAttack(float DeltaTime) {}
	virtual void EndAttack() {}

	virtual void StartRecovery() {}
	virtual void UpdateRecovery(float DeltaTime) {}
	virtual void EndRecovery() {}

	virtual void StartDelay() {}
	virtual void UpdateDelay(float DeltaTime) {}
	virtual void EndDelay() {}

	virtual void StartHit() {}
	virtual void UpdateHit(float DeltaTime) {}
	virtual void EndHit() {}

	virtual void StartDeath();
	virtual void UpdateDeath(float DeltaTime);
	virtual void EndDeath();

	virtual void StartHitEffect() {}
	virtual void UpdateHitEffect(float DeltaTime) {}
	virtual void EndHitEffect() {}

	virtual void StartDeathEffect() {}
	virtual void UpdateDeathEffect(float DeltaTime) {}
	virtual void EndDeathEffect() {}

	virtual void Skill1(float DeltaTime) {}

	virtual void SetTintColor(FVector4D color);
	virtual void SetTintColor(float a, float b, float c);

protected:
	CSharedPtr<class CSpriteComponent>		mRoot;
	CSharedPtr<class CColliderAABB2D>		mBody;
	CSharedPtr<class CColliderSphere2D>		mDetectionRange;
	CSharedPtr<class CMovementComponent>	mMovement;
	CSharedPtr<class CRotationComponent>	mRotation;
	class CAnimation2D* mAnimation = nullptr;

	EMonsterState	mState;
	EMonsterEffectState mEffectState;
	float mEffectStateTime;
	EMonsterState	mPrevState = EMonsterState::Idle;
	float			mStateTime = 0.f;
	float			mDelyTargetTime = 0.f;

	CSharedPtr<CSceneObject> mTarget;
	FVector2D		mMoveTarget = FVector2D::Zero;
	FVector2D		mTargetdir = FVector2D::Zero;
	FVector2D		mLastTargetDir = FVector2D::Zero;
	float			mTargetdis = 0.f;

	bool			mAutoBasePose = true;
	bool			mIsMove = false;
	bool			mLookRight = true;
	bool			mLockRotation = false;

	bool			mIsRight = false;

	float mRunDustAccTime = 0.f;
	FVector3D basePos = FVector3D::Zero;
};