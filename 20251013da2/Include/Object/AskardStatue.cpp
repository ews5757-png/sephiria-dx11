#include "AskardStatue.h"
#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"

CAskardStatue::CAskardStatue()
{
}

CAskardStatue::CAskardStatue(const CAskardStatue& Obj)
{
}

CAskardStatue::CAskardStatue(CAskardStatue&& Obj)
{
}

CAskardStatue::~CAskardStatue()
{
}

bool CAskardStatue::Init()
{
	mBodyType = EBodyType::Static;

	mRoot = CreateComponent<CSpriteComponent>();

	mBody = CreateComponent<CColliderAABB2D>();

	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.f);
	mRoot->SetOpacity(1.f);

	mRoot->SetWorldPos(0.f, 0.f, 0.f);
	mRoot->SetUseFrameNativeSize(true);
	//mRoot->SetWorldScale( 1.f);

	SetRootComponent(mRoot);
	mRoot->SetTexture("AskardStatue");

	mRoot->AddChild(mBody);
	mBody->SetBoxSize(50.f, 50.f);
	mBody->SetRelativePos(0.f, 25.f);
	mBody->SetCollisionProfile("Wall");

	return true;
}
void CAskardStatue::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

}