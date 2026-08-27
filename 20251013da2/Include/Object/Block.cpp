#include "Block.h"

#include "../Component/SpriteComponent.h"
#include "../Component/ColliderAABB2D.h"

CBlock::CBlock()
{
}

CBlock::CBlock(const CBlock& Obj)
{
}

CBlock::CBlock(CBlock&& Obj)
{
}

CBlock::~CBlock()
{
}

 bool CBlock::Init()
 {

	 mBodyType = EBodyType::Static;
	 mRoot = CreateComponent<CSpriteComponent>();

	 mBody = CreateComponent<CColliderAABB2D>();

	 mRoot->SetTexture("BonoBono", TEXT("Texture/BonoBono.png"), 0);
	 mRoot->SetTint(1.f, 1.f, 1.f);
	 mRoot->SetPivot(0.5f, 0.5f);
	 mRoot->SetOpacity(1.f);

	 mAnimation = mRoot->CreateAnimation2D<CAnimation2D>();

	 mAnimation->AddSequence("PlayerIdle_Idle_Lower", 1.f, 1.f, true, false);
	 mAnimation->AddSequence("PlayerIdle_Idle_Upper", 1.f, 1.f, true, false);
	 mAnimation->AddSequence("Move_Lower", 1.f, 1.f, true, false);
	 mAnimation->AddSequence("Move_Upper", 1.f, 1.f, true, false);

	 mRoot->SetWorldPos(0.f, 0.f, 0.f);
	 mRoot->SetWorldScale(100.f, 100.f, 1.f);
	 mRoot->AddChild(mBody);
	 SetRootComponent(mRoot);
	 return true;
 }
 void CBlock::Update(float DeltaTime)
 {
	 CSceneObject::Update(DeltaTime);
 }
