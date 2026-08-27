#include "PlayerWeaponObject.h"
#include "PlayerObject.h"
#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Component/ColliderOBB2D.h"
#include "../Component/ColliderBase.h"
#include "SpriteEffect.h"
#include "../Scene/Scene.h"

CPlayerWeaponObject::CPlayerWeaponObject()
{
}

CPlayerWeaponObject::CPlayerWeaponObject(const CPlayerWeaponObject& Obj)
{
}

CPlayerWeaponObject::CPlayerWeaponObject(CPlayerWeaponObject&& Obj)
{
}

CPlayerWeaponObject::~CPlayerWeaponObject()
{
}
bool CPlayerWeaponObject::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CSpriteComponent>();
	mBody = CreateComponent<CColliderOBB2D>();
	mPivot = CreateComponent<CSceneComponent>();

	mRoot->AddChild(mPivot);
	mPivot->SetWorldScale(0.3f, 0.3f);
	mPivot->SetPivot(0.5f, 0.5f);
	mPivot->SetRelativePos(0.f, 0.f);


	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.f);
	mRoot->SetOpacity(1.f);

	//mRoot->SetWorldPos(0.f, 0.f, 0.f);
	float RatioX = 9.f / 18.f;  // 0.5
	float RatioY = 16.f / 19.f; // 0.84...

	mRoot->SetWorldScale(RatioX, RatioY, 1.f);

	mRoot->SetTexture("Sword_Tier1", 0);

	SetRootComponent(mRoot);

	mBody->SetBoxSize(9.f / 18.f * 180.f / 2  /5, 16.f / 19.f * 190.f / 5);
	mBody->SetPivot(1.f, 1.f);
	mBody->SetRelativePos(0.f, 16.f / 19.f * 190.f / 2 / 5);
	mRoot->AddChild(mBody);

	mRoot->SetFlip(true);
	mBody->SetCollisionProfile("PlayerAttack");
	mBody->SetName("weapon");

	mBody->SetCollisionBeginFunc<CPlayerWeaponObject>(this, &CPlayerWeaponObject::CollisionObj);


	return true;
}

void CPlayerWeaponObject::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

}

void CPlayerWeaponObject::SetCollisionEnable(bool  b)
{
	mBody->SetEnable(b);
}

void CPlayerWeaponObject::CollisionObj(const FVector3D& pos, CColliderBase* obj)
{
	CSceneObject* Victim = dynamic_cast<CSceneObject*>(obj->GetOwner());

	FCollisionProfile* prof = obj->GetProfile();

	if (Victim && Victim != mOwner.Get() && prof->Channel == ECollisionChannel::Monster && Victim->CanDamage())
	{
		FVector3D KnockbackDir = Victim->GetWorldPosition() - mOwner->GetWorldPosition();

		KnockbackDir.Normalize();

		FDamageInfo info(30.f, mOwner.Get(), pos, KnockbackDir);


		CSpriteEffect* SlashEffect = mScene->CreateObj<CSpriteEffect>("Slash_Player");
		CSceneComponent* SlashEffectBody = SlashEffect->GetRootComponent();
		SlashEffectBody->SetRenderOrder(100);

		FVector2D dirc = pos - mOwner->GetWorldPosition();

		float radian = atan2f(dirc.y, dirc.x);

		float degree = radian * (180.f / 3.14159265f);

		float zRotation = degree + 0.f;

		SlashEffect->SetRelativeRotationZ(zRotation);
		SlashEffect->SetAnimation("Slash_Player", 1.f, 2.f);
		SlashEffect->SetWorldPos(pos);
		SlashEffect->SetWorldScale(25.f / 5, 570.f / 10.f);
		SlashEffect->GetRootComponent()->SetRenderLayerName("ObjectFront");

		CPlayerObject* owner = dynamic_cast<CPlayerObject*>(mOwner.Get());

		Victim->Damage(info);
	}

}

void CPlayerWeaponObject::SwingEffect(const FVector2D& Dir, int combo, bool flip)
{
	CSpriteEffect* mSwingEffect = mScene->CreateObj<CSpriteEffect>("Sword_Swing1");
	if (!mSwingEffect)
		return;

	FVector2D mousedir = Dir;

	float mouseRadian = atan2f(mousedir.y, mousedir.x);
	float mouseDegree = mouseRadian * (180.f / 3.14159265f);
	float baseZ = mouseDegree - 90.f;

	mSwingEffect->SetWorldPos(mPivot->GetWorldPosition());
	mSwingEffect->SetEffectPivot(FVector2D(0.5f, 0.5f));
	mSwingEffect->SetTarget(mPivot);
	mSwingEffect->SetFollowOffset(mousedir * 20.f);
	mSwingEffect->GetRootComponent()->SetRenderOrder(110);

	float imageOffset = 180.f;
	float z = baseZ + imageOffset;

	switch (combo)
	{
	case 1:
		mSwingEffect->SetAnimation("Sword_Swing1_", 1.f, 5.f);
		mSwingEffect->SetWorldScale(22.f , 37.f );

		if (flip)
		{
			mSwingEffect->SetRelativeRotationZ(-baseZ);   
			mSwingEffect->SetRelativeRotationY(180);      
		}
		else
		{
			mSwingEffect->SetRelativeRotationZ(baseZ);    
		}
		break;

	case 2:
		mSwingEffect->SetAnimation("Sword_Swing2_", 1.f, 5.f);
		mSwingEffect->SetWorldScale(30.f , 30.f );

		if (flip)
		{
			mSwingEffect->SetRelativeRotationZ(-baseZ);   
			mSwingEffect->SetRelativeRotationY(180);      
		}
		else
		{
			mSwingEffect->SetRelativeRotationZ(baseZ);    
		}
		break;

	case 3:
		mSwingEffect->SetAnimation("Sword_Swing3_", 1.f, 5.f);
		mSwingEffect->SetWorldScale(43, 32);

		if (flip)
		{
			mSwingEffect->SetRelativeRotationZ(baseZ);            
		}
		else
		{
			mSwingEffect->SetRelativeRotationZ((-mouseDegree - 90));
			mSwingEffect->SetRelativeRotationX(180);                
		}
		break;

	default:
		break;
	}


	
}
