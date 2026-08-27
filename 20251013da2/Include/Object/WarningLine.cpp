#include "WarningLine.h"
#include "../Component/SpriteComponent.h"

CWarningLine::CWarningLine()
{
}

CWarningLine::CWarningLine(const CWarningLine& Obj)
{
}

CWarningLine::CWarningLine(CWarningLine&& Obj)
{
}

CWarningLine::~CWarningLine()
{
}
bool CWarningLine::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CSceneComponent>();
	mOutLeft = CreateComponent<CSpriteComponent>();
	mOutRight = CreateComponent<CSpriteComponent>();
	minnerLeft = CreateComponent<CSpriteComponent>();
	minnerRight = CreateComponent<CSpriteComponent>();

	mOutLeft->SetPivot(0.5f, 0.f);
	mOutRight->SetPivot(0.5f, 0.f);
	minnerLeft->SetPivot(0.5f, 0.f);
	minnerRight->SetPivot(0.5f, 0.f);

	SetRootComponent(mRoot);
	mRoot->AddChild(mOutLeft);
	mRoot->AddChild(mOutRight);
	mRoot->AddChild(minnerLeft);
	mRoot->AddChild(minnerRight);
	//mRoot->SetPivot(0.5f, 0.5f);

	FVector4D tempcolor(0.8f, 0.14f, 0.35f,0.9f);


	mOutLeft->SetTint(tempcolor);
	mOutRight->SetTint(tempcolor);
	minnerLeft->SetTint(tempcolor);
	minnerRight->SetTint(tempcolor);

	mRoot->SetRelativeScale(1.f * 2.f, 22.f * 2.f);

	mOutLeft->SetTexture("WarningRangeLine");
	mOutLeft->SetRelativeScale(0.5f, mMaxLineLength, 0);
	mOutLeft->SetRelativePos(-mMaxLinewidth, margin, 0);

	mOutRight->SetTexture("WarningRangeLine");
	mOutRight->SetRelativeScale(0.5f, mMaxLineLength, 0);
	mOutRight->SetRelativePos(mMaxLinewidth, margin, 0);

	minnerLeft->SetTexture("WarningRangeLine");
	minnerLeft->SetRelativeScale(0.25f, mMaxLineLength, 0);
	minnerLeft->SetRelativePos(-mMaxLinewidth /2.f, margin, 0);

	minnerRight->SetTexture("WarningRangeLine");
	minnerRight->SetRelativeScale(0.25f, mMaxLineLength, 0);
	minnerRight->SetRelativePos(mMaxLinewidth /2.f, margin, 0);


	return true;
}

void CWarningLine::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	mElapsedTime += DeltaTime;
	

	if (mElapsedTime < mAnimationDuration)
	{
		float animationOffset = mElapsedTime / mAnimationDuration * mMaxLinewidth / 2.f;
		minnerLeft->SetRelativePos(-animationOffset - mMaxLinewidth / 2.f, 0, 0);
		minnerRight->SetRelativePos(animationOffset + mMaxLinewidth / 2.f, 0, 0);
	}
	else
	{
		Destroy();
	}
}


void CWarningLine::startanime()
{
}
