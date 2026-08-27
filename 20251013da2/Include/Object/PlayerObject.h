#pragma once
#include "SceneObject.h"

enum class EPlayerCombatState
{
	None,
	Attack,
	Defend,
};
enum class EPlayerBaseState
{
	Idle,
	Move,
	Dash,
	Dead,
	Stun
};

enum class EDir8
{
	Right = 0,
	RightUp,
	Up,
	LeftUp,
	Left,
	LeftDown,
	Down,
	RightDown
};

class CPlayerObject :
	public CSceneObject
{
	friend class CScene;

protected:
	CPlayerObject();
	CPlayerObject(const CPlayerObject& Obj);
	CPlayerObject(CPlayerObject&& Obj);
	virtual ~CPlayerObject();

protected:


	CSharedPtr<class CSpriteComponent> mRoot;

	CSharedPtr<class CColliderAABB2D> mBody;

	class CAnimation2D* mAnimation = nullptr;

	CSharedPtr<class CMovementComponent> mMovement;

	CSharedPtr<class CRotationComponent> mRotation;

	CSharedPtr<class CSceneComponent> mWeaponSocket;
	//FVector3D mWeaponSocktPos;
	CSharedPtr<class CSceneComponent> mWeaponSocktPivot;

	CSharedPtr<class CSceneComponent> mShieldSocket;
	//FVector3D mShieldSocktPos;
	CSharedPtr<class CSceneComponent> mShieldSocktPivot;

	CSharedPtr<class CSpriteComponent> mShieldEffect;

	CSharedPtr<class CSceneComponent> mShieldEffectPivot;

	CSharedPtr<class CPlayerWeaponObject> WeaponObj;

	CSharedPtr<class CInventoryComponent> mInventory;


	bool mIsSwingEffectProduced = false;
	//bool mAutoBasePose = true;


public:
	bool mIsMove = false;

	bool mLookRight = true;;

	FVector2D mMouseDir = FVector2D::Zero;
public:
	bool mIsDefending = false;
	bool mIsAttacking = false;
	bool mCanAttack = true;
	bool mHasPreInput = false;
	int  mAttackCombo = 0;

	float mCurrentAttackAngle = 0.f;
	float mTargetAttackAngle = 110.f;


	float mCurrentDefendAngle = 0.f;

	FVector3D mShieldSocketPos = FVector3D::Zero;
	FVector3D mWeaponSocketPos = FVector3D::Zero;

	FVector3D mWeaponSocketOffset = FVector3D::Zero;
	FVector3D mShieldSocketOffset = FVector3D::Zero;

	EPlayerCombatState mCombatState;
	EPlayerBaseState mBaseState;

	FVector3D mLastMoveDir = FVector3D::Zero;

	float mDashDurationElapsed = 0;
	int mDashTrailSpawned = 0;
	int MaxDashCount = 0;

	float mRunDustAccTime = 0.f;

	EDir8 dir8Index;


	CAnimation2D* GetAnimaion2d()
	{
		return mAnimation;
	}

public:
	virtual bool Init();
	void ComponentInit();
	void AnimationInit();
	void InputInit();
	bool SocktInit();

	virtual void Update(float DeltaTime);
	void ComponentUpdate(float DeltaTime);
	void SocketUpdate(float DeltaTime);
	void MovementUpdate(float DeltaTime);

	void CalculateMouseDir();
	void MovementAnimation();

	void SetCombatState(EPlayerCombatState state) { mCombatState = state; };
	void SetBaseState(EPlayerBaseState state) { mBaseState = state; };


	void AttackStart(float DeltaTime);
	void UpdateAttack(float DeltaTime);

	void AttackCombo1(float DeltaTime);
	void AttackCombo2(float DeltaTime);
	void AttackCombo3(float DeltaTime);

	void UpdateIdle(float DeltaTime);


	void DefendStart(float Deltatime);
	void UpdateDefend(float DeltaTime);
	void DefendingEnd(float DeltaTime);

	void DashStart(float DelataTime);
	void UpdateDash(float DeltaTime);
	void DashEnd(float DeltaTime);



private:
	void MoveUp(float DeltaTime);
	void MoveDown(float DeltaTime);
	void MoveRight(float DeltaTime);
	void MoveLeft(float DeltaTime);



	void Skill1(float DeltaTime);
	void Skill2(float DeltaTime);

	void ToggleInventory(float DeltaTime);
public:
	int GetAttcakCombo() { return mAttackCombo; }

	FVector2D GetMouseDir() { return mMouseDir; }

	CInventoryComponent* Getinventory() { return mInventory; }

};

