#include "Tree.h"
#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"

CTree::CTree()
{
}

CTree::CTree(const CTree& Obj)
{
}

CTree::CTree(CTree&& Obj)
{
}

CTree::~CTree()
{
}
bool CTree::Init()
{
	mBodyType = EBodyType::Static;

	mRoot = CreateComponent<CSpriteComponent>();

	mBody = CreateComponent<CColliderAABB2D>();

	mRoot->SetTint(1.f, 1.f, 1.f);
	mRoot->SetPivot(0.5f, 0.f);
	mRoot->SetOpacity(1.f);

	mRoot->SetWorldPos(0.f, 0.f, 0.f);
	mRoot->SetWorldScale(200.f, 200.f, 1.f);

	SetRootComponent(mRoot);

	mAnimation = mRoot->CreateAnimation2D<CAnimation2D>();

	mAnimation->AddSequence("BigTree", 1.f, 1.f, true, false);

	mRoot->AddChild(mBody);
	mBody->SetBoxSize(50.f,50.f);
	mBody->SetRelativePos(0.f, 25.f);
	mBody->SetCollisionProfile("Wall");

	return true;
}
void CTree::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

}