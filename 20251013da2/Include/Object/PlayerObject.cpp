#include "PlayerObject.h"

#include "../Component/StaticMeshComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Component/ColliderSphere2D.h"
#include "../Component/ColliderOBB2D.h"
#include "../Component/RotationComponent.h"
#include "../Component/SpriteComponent.h"
#include "../Component/InventoryComponent.h"

#include "../Scene/SceneUIManager.h"
#include "../Scene/SceneMain.h"


#include"PlayerWeaponObject.h"
#include "PlayerShieldObject.h"
#include "MoleGuard.h"
#include "SpriteEffect.h"
#include "DashTrailEffect.h"

#include "../Scene/Scene.h"
#include "../Scene/Input.h"

#include "BulletObject.h"

#include "../Share/Log.h"
#include "../Share/Timer.h"
#include "MonsterBase.h"


CPlayerObject::CPlayerObject()
	: CSceneObject()
{
}

CPlayerObject::CPlayerObject(const CPlayerObject& Obj)
	: CSceneObject(Obj)
{
}

CPlayerObject::CPlayerObject(CPlayerObject&& Obj)
	: CSceneObject(Obj)
{
}

CPlayerObject::~CPlayerObject()
{
}



bool CPlayerObject::Init()
{
	//ui init

	ComponentInit();
	AnimationInit();
	InputInit();
	SocktInit();

	return true;
}


void CPlayerObject::Update(float DeltaTime)
{
	CalculateMouseDir();

	//mLastMoveDir = mMovement->GetVelocity();
	//mLastMoveDir.Normalize();
	
	switch (mCombatState)
	{
	case EPlayerCombatState::None:
		break;
	case EPlayerCombatState::Attack:
		UpdateAttack(DeltaTime);
		break;
	case EPlayerCombatState::Defend:
		UpdateDefend(DeltaTime);
		break;
	default:
		break;
	}

	switch (mBaseState)
	{
	case EPlayerBaseState::Idle:
		//MovementUpdate에서 떄주기
		break;
	case EPlayerBaseState::Move:
		//MovementUpdate에서 떄주기
		break;
	case EPlayerBaseState::Dash:
		UpdateDash(DeltaTime);
		break;
	case EPlayerBaseState::Dead:
		break;
	case EPlayerBaseState::Stun:
		break;
	default:
		break;
	}

	CSceneObject::Update(DeltaTime);

	ComponentUpdate(DeltaTime);
	MovementUpdate(DeltaTime);


	MovementAnimation();
	SocketUpdate(DeltaTime);

}

#pragma region Init
void CPlayerObject::ComponentInit()
{
	mBodyType = EBodyType::Dynamic;

	mRoot = CreateComponent<CSpriteComponent>();

	mBody = CreateComponent<CColliderAABB2D>();

	mMovement = CreateComponent<CMovementComponent>();

	mRotation = CreateComponent<CRotationComponent>();

	float RatioX = 32.f / 18.f;  // 0.5
	float RatioY = 16.f / 19.f; // 0.84...

	mShieldEffectPivot = CreateComponent<CSceneComponent>();
	mShieldEffect = CreateComponent<CSpriteComponent>();
	mShieldEffectPivot->AddChild(mShieldEffect);
	mShieldEffect->SetRelativePos(0, 10.f, 0.f);
	mShieldEffect->SetTexture("shieldRange");
	mShieldEffect->SetTint(1.f, 1.f, 1.f);
	mShieldEffect->SetPivot(0.5f, 0.5f);
	mShieldEffect->SetOpacity(1.f);
	mShieldEffect->SetWorldScale(RatioX, RatioY, 1.f);
	mRoot->AddChild(mShieldEffectPivot);
	//mRoot->SetTexture("BonoBono", TEXT("Texture/BonoBono.png"), 0);
	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.5f);
	mRoot->SetOpacity(1.f);

	mRoot->SetWorldPos(0.f, 0.f, 0.f);
	mRoot->SetWorldScale(18.f, 19.f, 1.f);
	mRoot->AddChild(mBody);
	mBody->SetBoxSize(10.f, 10.f);
	mBody->SetRelativePos(0.f, -2.f);
	mBody->SetCollisionProfile("Player");
	mBody->SetName("player");
	mMovement->SetUpdateComponent(mRoot);
	mMovement->SetMoveSpeed(100.f);

	mRotation->SetUpdateComponent(mRoot);
	mRotation->SetVelocityInit(1.f);


	mInventory = CreateComponent<CInventoryComponent>("playerinven");

	CSceneMain* temp = dynamic_cast<CSceneMain*>(mScene);
	if (temp)
	{
		temp->SetInventoty(mInventory);
	}

	SetRootComponent(mRoot);
}

void CPlayerObject::AnimationInit()
{
	mAnimation = mRoot->CreateAnimation2D<CAnimation2D>();

	mAnimation->AddSequence("PlayerIdle_Idle_Lower", 1.f, 1.f, true, false);
	mAnimation->AddSequence("PlayerIdle_Idle_Upper", 1.f, 1.f, true, false);
	mAnimation->AddSequence("Move_Lower", 1.f, 1.f, true, false);
	mAnimation->AddSequence("Move_Upper", 1.f, 1.f, true, false);
}

void CPlayerObject::InputInit()
{
	mScene->GetInput()->AddBindKey("Attack", VK_LBUTTON);
	mScene->GetInput()->AddBindFunction("Attack", EInputType::Down, this, &CPlayerObject::AttackStart);

	mScene->GetInput()->AddBindKey("Defending", VK_RBUTTON);
	mScene->GetInput()->AddBindFunction("Defending", EInputType::Hold, this, &CPlayerObject::DefendStart);
	mScene->GetInput()->AddBindFunction("Defending", EInputType::Up, this, &CPlayerObject::DefendingEnd);

	//입력
	mScene->GetInput()->AddBindKey("MoveUp", 'W');
	mScene->GetInput()->AddBindFunction("MoveUp", EInputType::Hold, this, &CPlayerObject::MoveUp);

	mScene->GetInput()->AddBindKey("MoveDown", 'S');
	mScene->GetInput()->AddBindFunction("MoveDown", EInputType::Hold, this, &CPlayerObject::MoveDown);

	mScene->GetInput()->AddBindKey("MoveRight", 'D');
	mScene->GetInput()->AddBindFunction("MoveRight", EInputType::Hold, this, &CPlayerObject::MoveRight);

	mScene->GetInput()->AddBindKey("MoveLeft", 'A');
	mScene->GetInput()->AddBindFunction("MoveLeft", EInputType::Hold, this, &CPlayerObject::MoveLeft);
	// 총알 발사
	mScene->GetInput()->AddBindKey("Fire", VK_SPACE);
	mScene->GetInput()->AddBindFunction("Fire", EInputType::Down, this, &CPlayerObject::DashStart);

	// 스킬 1 
	mScene->GetInput()->AddBindKey("Skill1", '1');
	mScene->GetInput()->AddBindFunction("Skill1", EInputType::Down, this, &CPlayerObject::Skill1);
	mScene->GetInput()->AddBindKey("Skill2", '2');
	mScene->GetInput()->AddBindFunction("Skill2", EInputType::Down, this, &CPlayerObject::Skill2);

	mScene->GetInput()->AddBindKey("ToggleInventory", 'V');
	mScene->GetInput()->AddBindFunction("ToggleInventory", EInputType::Down, this, &CPlayerObject::ToggleInventory);

}

bool CPlayerObject::SocktInit()
{
	mWeaponSocket = CreateComponent<CSceneComponent>();
	mWeaponSocktPivot = CreateComponent<CSceneComponent>();

	mShieldSocket = CreateComponent<CSceneComponent>();
	mShieldSocktPivot = CreateComponent<CSceneComponent>();

	mWeaponSocktPivot->SetRelativePos(FVector3D::Zero);

	mWeaponSocktPivot->AddChild(mWeaponSocket);
	mShieldSocktPivot->AddChild(mShieldSocket);
	mRoot->AddChild(mWeaponSocktPivot);
	mRoot->AddChild(mShieldSocktPivot);

	CPlayerShieldObject* PlayerShield = mScene->CreateObj<CPlayerShieldObject>("playershield");
	mShieldSocket->AddChild(PlayerShield->mRoot);
	PlayerShield->mOwner = this;

	WeaponObj = mScene->CreateObj<CPlayerWeaponObject>("PlayerWeapon");
	mWeaponSocket->AddChild(WeaponObj->mRoot);
	WeaponObj->SetCollisionEnable(false);
	WeaponObj->mOwner = this;


	return true;
}

#pragma endregion

void CPlayerObject::DefendStart(float Deltatime)
{

	if (!mIsAttacking)
	{

		SetCombatState(EPlayerCombatState::Defend);
	}
}
void CPlayerObject::UpdateDefend(float DeltaTime)
{
	mIsDefending = true;
	mCurrentDefendAngle += DeltaTime * 900.f;

	float rawSineWave = cos(TODIG(mCurrentDefendAngle));

	if (mCurrentDefendAngle < 180.f)
	{
		mShieldSocketOffset.x = (rawSineWave - 1.f) * -2.f;
	}
}
void CPlayerObject::DefendingEnd(float DeltaTime)
{
	if (mCombatState == EPlayerCombatState::Defend)
	{
		mIsDefending = false;
		mCurrentDefendAngle = 0;
		SetCombatState(EPlayerCombatState::None);
	}
}

void CPlayerObject::DashStart(float DelataTime)
{
	if (mBaseState != EPlayerBaseState::Dash)
	{
		mDashDurationElapsed = 0.f;
		mDashTrailSpawned = 0;
		mLastMoveDir = mMoveDir;
		SetBaseState(EPlayerBaseState::Dash);

		CSpriteEffect* dashEffect = mScene->CreateObj<CSpriteEffect>("dashEffect");

		switch (dir8Index)
		{
		case EDir8::Right:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust", 12, 17);
			break;
		case EDir8::RightUp:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust", 6, 11);
			break;
		case EDir8::Up:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust", 0, 5);
			break;
		case EDir8::LeftUp:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust", 42, 47);
			break;
		case EDir8::Left:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust", 36, 41);
			break;
		case EDir8::LeftDown:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust", 30, 35);
			break;
		case EDir8::Down:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust", 24, 29);
			break;
		case EDir8::RightDown:
			dashEffect->SetAnimation("DashDust", 1.f, 5.f);
			dashEffect->SetFrameRange("DashDust",11, 18);
			break;
		default:
			break;
		}
		dashEffect->SetWorldScale(37 , 23 );
		dashEffect->SetWorldPos(GetWorldPosition());
	}
}
void CPlayerObject::UpdateDash(float DeltaTime)
{
	
	mDashDurationElapsed += DeltaTime;

	
	const float DASH_DURATION = 0.3f;
	//
	const int TRAIL_COUNT = 5;

	// 대시 진행률 0~1
	float t = mDashDurationElapsed / DASH_DURATION;
	if (t > 1.f) t = 1.f;

	// 지금까지 "생성되어야 하는" 트레일 개수(0~5)
	int shouldSpawn = (int)(t * TRAIL_COUNT);

	while (mDashTrailSpawned < shouldSpawn)
	{
		CDashTrailEffect* dashtrail = mScene->CreateObj<CDashTrailEffect>("dashtrail");
		dashtrail->Animationinit(this);
		++mDashTrailSpawned;
	}

	//

	if (mLastMoveDir == FVector3D::Zero)
	{
		mLastMoveDir = (FVector3D(1.f, 0.f, 0.f));
	}

	if (mDashDurationElapsed < DASH_DURATION)
	{
		mMovement->AddMove(mLastMoveDir);

		// 경과 시간을 DASH_ACCEL_TIME(0.3f)에 대한 비율로 변환하여 코사인 커브 계산
		float dashSpeedFactor = cos(TODIG(mDashDurationElapsed / DASH_DURATION * 90.0f));

		mMovement->SetMoveSpeed(dashSpeedFactor * 300.0f);
	}
	else
	{
		DashEnd(DeltaTime);
	}
}
void CPlayerObject::DashEnd(float DeltaTime)
{
	SetBaseState(EPlayerBaseState::Idle);
	mMovement->SetMoveSpeed(100.f);
}
void CPlayerObject::AttackStart(float DeltaTime)
{
	if (mIsAttacking)
	{
		mHasPreInput = true;
	}
	if (!mIsDefending)
	{
		if (mCanAttack == true)
		{
			MovementAnimation();
			SocketUpdate(DeltaTime);
			mHasPreInput = false;
			mCurrentAttackAngle = 0;
			mAttackCombo++;
			if (mAttackCombo == 4)
			{
				mAttackCombo = 1;
			}
			mIsAttacking = true;
			mCanAttack = false;
			mIsSwingEffectProduced = false;
			SetCombatState(EPlayerCombatState::Attack);
		}
	}
	else
	{

	}
}
#pragma region Attack
void CPlayerObject::UpdateAttack(float DeltaTime)
{
	if (mIsAttacking)
	{
		switch (mAttackCombo)
		{
		case 1:
			AttackCombo1(DeltaTime);
			break;
		case 2:
			AttackCombo2(DeltaTime);
			break;
		case 3:
			AttackCombo3(DeltaTime);
			break;
		default:
			break;
		}
	}
}

void CPlayerObject::AttackCombo1(float DeltaTime)
{

	mCurrentAttackAngle += DeltaTime * 300.f;

	float rawSineWave = sin(TODIG(mCurrentAttackAngle));

	float shieldTargetAngle = 360.f;
	float progressRatio = mCurrentAttackAngle / mTargetAttackAngle;
	float shieldRawWave = cos(TODIG(progressRatio * shieldTargetAngle));

	if (mCurrentAttackAngle < mTargetAttackAngle)
	{
		if (mCurrentAttackAngle >= 0.f && !mIsSwingEffectProduced)
		{
			if (WeaponObj)
			{
				WeaponObj->SwingEffect(mMouseDir, mAttackCombo, mLookRight);
				WeaponObj->SetCollisionEnable(true);
			}
			mIsSwingEffectProduced = true;
		}

		float weaponCurve = pow(rawSineWave, 0.3f);
		float shieldCurve = pow(shieldRawWave, 0.15f);
		mWeaponSocket->SetRelativeRotationZ(weaponCurve * 200.f * -1.f);

		mWeaponSocketOffset.x = weaponCurve * 4.f * -1.f;

		mShieldSocketOffset.x = (shieldRawWave - 1.f) * -1.f;
	}
	else
	{
		mIsAttacking = false;

		mAttackCombo = 0;
		mCurrentAttackAngle = 0.f;
		mWeaponSocket->SetRelativeRotationZ(0.f);
		MovementUpdate(DeltaTime);
	}

	if (mCurrentAttackAngle >= 70.f)
	{
		WeaponObj->SetCollisionEnable(false);
		mCanAttack = true;
		if (mHasPreInput == true)
		{
			AttackStart(DeltaTime);
		}
	}
}
void CPlayerObject::AttackCombo2(float DeltaTime)
{

	mCurrentAttackAngle += DeltaTime * 300.f;

	float rawSineWave = sin(TODIG(mCurrentAttackAngle));

	float shieldTargetAngle = 360.f;
	float progressRatio = mCurrentAttackAngle / mTargetAttackAngle;
	float shieldRawWave = cos(TODIG(progressRatio * shieldTargetAngle));

	if (mCurrentAttackAngle < mTargetAttackAngle)
	{
		if (mCurrentAttackAngle >= 0.f && !mIsSwingEffectProduced)
		{
			if (WeaponObj)
			{
				WeaponObj->SwingEffect(mMouseDir, mAttackCombo, mLookRight);
				WeaponObj->SetCollisionEnable(true);
			}
			mIsSwingEffectProduced = true;
		}
		float weaponCurve = pow(rawSineWave, 0.15f);
		float shieldCurve = pow(shieldRawWave+1, 0.15f);

		mWeaponSocket->SetRelativeRotationZ(weaponCurve * 180.f + 180.f);
		mWeaponSocketOffset.y = weaponCurve * 6.f;

		mShieldSocketOffset.x = (shieldRawWave - 1.f) * 1.f;
	}
	else
	{
		mIsAttacking = false;
		mAttackCombo = 0;
		mCurrentAttackAngle = 0.f;
		mWeaponSocket->SetRelativeRotationZ(0.f);
		MovementUpdate(DeltaTime);
	}

	if (mCurrentAttackAngle >= 60.f)
	{
		WeaponObj->SetCollisionEnable(false);
		mCanAttack = true;
		if (mHasPreInput == true)
		{
			AttackStart(DeltaTime);
		}
	}
}
void CPlayerObject::AttackCombo3(float DeltaTime)
{
	mWeaponSocketOffset = FVector3D::Zero;
	mShieldSocketOffset = FVector3D::Zero;

	mCurrentAttackAngle += DeltaTime * 900.f;

	float motionEndAngle = mTargetAttackAngle + 180.f;
	float delayAngle = 120.f;                          
	float totalEndAngle = motionEndAngle + delayAngle;

	float calcAngle = mCurrentAttackAngle;
	if (calcAngle > motionEndAngle)
	{
		calcAngle = motionEndAngle;
	}

	float X = sin(TODIG(mCurrentAttackAngle));
	

	float weaponPhaseAngle = calcAngle + 180.f;
	float rawSineWave = sin(TODIG(weaponPhaseAngle));

	float shieldTargetAngle = 360.f;
	float progressRatio = calcAngle / (mTargetAttackAngle + 180.f);
	float shieldRawWave = cos(TODIG(progressRatio * shieldTargetAngle));

	if (mCurrentAttackAngle < totalEndAngle + 180.f)
	{

		if (calcAngle < motionEndAngle + 180.f)
		{
			if (mCurrentAttackAngle >= 180.f && !mIsSwingEffectProduced)
			{
				if (WeaponObj)
				{
					WeaponObj->SwingEffect(mMouseDir, mAttackCombo, mLookRight);
					WeaponObj->SetCollisionEnable(true);
				}
				mIsSwingEffectProduced = true;
			}
			float absWave = fabsf(rawSineWave);
			float finalCurve = pow(absWave, 1.0f);
			if (rawSineWave < 0) finalCurve *= -1.f;

			float rotationAmp;
			(weaponPhaseAngle <= 360) ? rotationAmp = 45 : rotationAmp = 220;

			mWeaponSocket->SetRelativeRotationZ(finalCurve * rotationAmp * -1.f);

			mWeaponSocketOffset.y = (finalCurve - 1.f) * -6.f;
			mWeaponSocketOffset.x = absWave * -4.f;

			mShieldSocketOffset.x = (shieldRawWave - 1.f) * -1.f;
		}
	}
	else
	{
		WeaponObj->SetCollisionEnable(false);
		mIsAttacking = false;
		mAttackCombo = 0;
		mCurrentAttackAngle = 0.f;
		mWeaponSocket->SetRelativeRotationZ(0.f);
		mCanAttack = true;
		MovementUpdate(DeltaTime);
		if (mHasPreInput == true)
		{
			AttackStart(DeltaTime);
		}
	}

	if (mCurrentAttackAngle >= 260.f)
	{
	}
}
#pragma endregion

void CPlayerObject::SocketUpdate(float DeltaTime)
{
	FVector3D FinalWeaponPos = mWeaponSocketPos;
	FVector3D FinalShieldPos = mShieldSocketPos;




	float radian = atan2(mMouseDir.y, mMouseDir.x);
	float degree = 0;

	if (mCanAttack)
	{
		if (!mLookRight)
		{
			mShieldSocktPivot->SetRelativeRotationY(180);
			mWeaponSocktPivot->SetRelativeRotationY(180);
			degree = radian * (180.f / 3.141592f);
			mWeaponSocktPivot->SetRelativeRotationZ((degree - 180) * -1);
		}
		else
		{
			mShieldSocktPivot->SetRelativeRotationY(0);
			mWeaponSocktPivot->SetRelativeRotationY(0);
			degree = radian * (180.f / 3.141592f);
			mWeaponSocktPivot->SetRelativeRotationZ(degree);
		}
	}

	if (mIsDefending)
	{
		mShieldEffect->SetEnable(true);
		mShieldEffectPivot->SetRelativeRotationZ(degree-90);
	}
	else
	{
		mShieldEffect->SetEnable(false);
	}

	if (!mIsDefending && !mIsAttacking)
	{
		mWeaponSocketOffset = 0.f;
		mShieldSocketOffset = 0.f;
		float temp = cos(CTimer::mRuntime * 2);
		FinalWeaponPos.y += temp * .5;
	}


	FinalWeaponPos += mWeaponSocketOffset;
	FinalShieldPos += mShieldSocketOffset;




	mWeaponSocket->SetRelativePos(FinalWeaponPos);
	mShieldSocket->SetRelativePos(FinalShieldPos);

}


void CPlayerObject::ComponentUpdate(float DeltaTime)
{
	//mShieldEffect;
}

void CPlayerObject::MovementUpdate(float DeltaTime)
{
	if (mMovement->GetVelocityLength() == 0)
	{
		mIsMove = false;
	}
	else
	{
		mIsMove = true;
	}

	if (mIsMove)
	{
		if (mBaseState == EPlayerBaseState::Idle)
		{
			mBaseState = EPlayerBaseState::Move;
		}
		if (mMouseDir.y > 0)
		{
			mAnimation->ChangeAnimation("Move_Upper");
			mWeaponSocketPos = FVector3D(6.f, -5.f, 0.f);
			mShieldSocketPos = FVector3D(-2.f, -4.f, 0.f);
		}
		else
		{
			mAnimation->ChangeAnimation("Move_Lower");
			mWeaponSocketPos = FVector3D(6.f, -6.f, 0.f);
			mShieldSocketPos = FVector3D(-4.f, -4.f, 0.f);
		}
		if (mBaseState == EPlayerBaseState::Move)
		{
			mRunDustAccTime += DeltaTime;

			if (mRunDustAccTime >= 0.5f)
			{
				mRunDustAccTime -= 0.5f;

				CSpriteEffect* RunDust = mScene->CreateObj<CSpriteEffect>("RunDust");
				RunDust->SetAnimation("RunDust", 1.f, 3.f);
				RunDust->SetWorldPos(GetWorldPosition());
				RunDust->SetWorldScale(7  , 7 );
				RunDust->GetRootComponent()->SetRenderOrder(-10);
			}
		}

	}
	else
	{
		if (mMouseDir.y > 0)
		{
			mAnimation->ChangeAnimation("PlayerIdle_Idle_Upper");
			mWeaponSocketPos = FVector3D(6.f, -5.f, 0.f);
			mShieldSocketPos = FVector3D(-2.f, -4.f, 0.f);
		}
		else
		{
			mAnimation->ChangeAnimation("PlayerIdle_Idle_Lower");
			mWeaponSocketPos = FVector3D(6.f, -6.f, 0.f);
			mShieldSocketPos = FVector3D(-4.f, -4.f, 0.f);
		}
	}

	float angle = atan2f(mMoveDir.y, mMoveDir.x);   // -PI ~ PI
	float degree = angle * (180.f / 3.14159265f);   // -180 ~ 180
	if (degree < 0.f)
		degree += 360.f;   // 0 ~ 360
	
	int temp = int((degree + 22.5f) / 45.f) % 8;

	dir8Index = static_cast<EDir8>(temp);

}



void CPlayerObject::CalculateMouseDir()
{
	FVector2D temp = mScene->GetInput()->GetMouseWorldPos() - FVector2D(GetWorldPosition().x, GetWorldPosition().y);
	temp.Normalize();
	mMouseDir = temp;
}

void CPlayerObject::MovementAnimation()
{
	if (mCanAttack)
	{
		if (mMouseDir.x > 0)
		{
			mRoot->SetFlip(false);
			mLookRight = true;
		}
		else
		{
			mRoot->SetFlip(true);
			mLookRight = false;
		}
	}


}
#pragma 
void CPlayerObject::MoveUp(float DeltaTime)
{
	if (mBaseState == EPlayerBaseState::Idle || mBaseState == EPlayerBaseState::Move)
	{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::Y) * 1);
	}

}

void CPlayerObject::MoveDown(float DeltaTime)
{
	if (mBaseState == EPlayerBaseState::Idle || mBaseState == EPlayerBaseState::Move)
	{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::Y) * -1);
	}

}

void CPlayerObject::MoveRight(float DeltaTime)
{
	if (mBaseState == EPlayerBaseState::Idle || mBaseState == EPlayerBaseState::Move)
	{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::X));
	}

}

void CPlayerObject::MoveLeft(float DeltaTime)
{

	if (mBaseState == EPlayerBaseState::Idle || mBaseState == EPlayerBaseState::Move)
	{
	mMovement->AddMove(mRootComponent->GetAxis(EAxis::X) * -1);
	}
}

void CPlayerObject::Skill1(float DeltaTime)
{
	mInventory->AddItem(1);
	CSceneMain* temp = dynamic_cast<CSceneMain*>(mScene);
	if (temp)
	{
		temp->UpdateInventoty();
	}


}

void CPlayerObject::Skill2(float DeltaTime)
{
	mInventory->RotateItem(0);
}


void CPlayerObject::ToggleInventory(float DeltaTime)
{
	CSceneMain* temp = dynamic_cast<CSceneMain*>(mScene);
	if (temp)
	{
		temp->ToggleinventoryUI();
	}
}
