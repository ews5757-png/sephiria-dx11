#include "DashTrailEffect.h"
#include "PlayerObject.h"
#include "../Component/SpriteComponent.h"
#include "../Animation/Animation2D.h"


CDashTrailEffect::CDashTrailEffect()
{
}

CDashTrailEffect::CDashTrailEffect(const CDashTrailEffect& Obj)
	: CSceneObject(Obj)
{
}

CDashTrailEffect::CDashTrailEffect(CDashTrailEffect&& Obj)
	: CSceneObject(Obj)

{
}

CDashTrailEffect::~CDashTrailEffect()
{
}

bool CDashTrailEffect::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CSpriteComponent>();
	mRoot->SetPivot(0.5f, 0.5f);
	SetRootComponent(mRoot);

	mAnimation = mRoot->CreateAnimation2D<CAnimation2D>();

	mRoot->SetRenderOrder(-5);

	return true;
}

void CDashTrailEffect::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	mRemainingTime -= DeltaTime;

	float alpha = mRemainingTime / mLifeTime;

	mRoot->SetTint(FVector3D::White, alpha);

	if (mRemainingTime <= 0.f)
	{
		Destroy();
	}
}
void CDashTrailEffect::Animationinit(CSceneObject* target)
{
	CPlayerObject* owner = dynamic_cast<CPlayerObject*>(target);
	if (!owner)
	{
		return;
	}
	mRoot->SetWorldScale(owner->GetWorldScale());

	mRoot->SetWorldPos(owner->GetWorldPosition());

	mRoot->SetOverrideColor(true);


	CSpriteComponent* ownerroot = dynamic_cast<CSpriteComponent*>(owner->GetRootComponent());
	mRoot->SetFlip(ownerroot->GetFlip());

	CAnimation2DSequence* src = owner->GetAnimaion2d()->GetCurrentSequence();

	if (!src) return;

	int frame = src->GetFrame();

	std::string seqName = src->GetName(); // ← 여기서 먼저 확보

	CAnimation2DSequence* copy = src->Clone();
	copy->SetFrameRange(frame, frame);

	mAnimation->AddSequence(copy);
	mAnimation->ChangeAnimation(seqName);
}

