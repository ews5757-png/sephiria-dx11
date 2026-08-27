#include "TorchStand.h"
#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"

bool CTorchStand::Init()
{
	mBodyType = EBodyType::Static;

	mRoot = CreateComponent<CSpriteComponent>();

	//Right = CreateComponent<CSpriteComponent>();
	//Right->SetWorldScale(1,1);
	//Right->SetTexture("Blur");
	mBody = CreateComponent<CColliderAABB2D>();

	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.f);
	//Right->SetPivot(0.5, 0);
	mRoot->SetOpacity(1.f);

	SetRootComponent(mRoot);


	mAnimation = mRoot->CreateAnimation2D<CAnimation2D>();

	mRoot->SetUseFrameNativeSize(true);

	mAnimation->AddSequence("TorchStand", 1.f, 1.f, true, false);

	mRoot->AddChild(mBody);
	//mRoot->AddChild(Right);
	mBody->SetBoxSize(50.f, 50.f);
	mBody->SetRelativePos(0.f, 25.f);
	mBody->SetCollisionProfile("Wall");

	return true;
}
void CTorchStand::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

}

CTorchStand::CTorchStand()
{
}

CTorchStand::CTorchStand(const CTorchStand& Obj)
{
}

CTorchStand::CTorchStand(CTorchStand&& Obj)
{
}

CTorchStand::~CTorchStand()
{
}
