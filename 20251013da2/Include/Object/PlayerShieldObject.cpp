#include "PlayerShieldObject.h"
#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"
#include "../Component/StaticMeshComponent.h"
#include "../Component/ColliderOBB2D.h"

CPlayerShieldObject::CPlayerShieldObject()
{
}

CPlayerShieldObject::CPlayerShieldObject(const CPlayerShieldObject& Obj)
{
}

CPlayerShieldObject::CPlayerShieldObject(CPlayerShieldObject&& Obj)
{
}

CPlayerShieldObject::~CPlayerShieldObject()
{
}

bool CPlayerShieldObject::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CSpriteComponent>();
	mBody = CreateComponent<CColliderOBB2D>();



	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.5f);
	mRoot->SetOpacity(1.f);

	mRoot->SetRenderOrder(2);

	mRoot->SetWorldPos(0.f, 0.f, 0.f);
	
	float RatioX = 8.f / 18.f;  // 0.5
	float RatioY = 9.f / 19.f; // 0.84...

	// 이렇게 설정하면 최종적으로 화면에는 (45, 80)으로 출력됩니다.
	mRoot->SetWorldScale(RatioX, RatioY, 1.f);

	mRoot->SetTexture("Shield_Tier1", 0);

	SetRootComponent(mRoot);

	mRoot->AddChild(mBody);
	mBody->SetBoxSize(10.f, 10.f);


	mBody->SetCollisionProfile("PlayerAttack");
	mBody->SetName("Shield");

	return true;
}

void CPlayerShieldObject::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

}