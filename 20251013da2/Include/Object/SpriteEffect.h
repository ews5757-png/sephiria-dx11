#pragma once
#include "SceneObject.h"

class CSpriteEffect :
    public CSceneObject
{
	friend class CScene;

protected:
	CSharedPtr<class CSpriteComponent> mRoot;

	class CAnimation2D* mAnimation = nullptr;
	CSharedPtr<class CSceneComponent> mTarget = nullptr;

	FVector2D mFollowOffset = FVector2D::Zero;


	bool mLoop = false;

protected:
	CSpriteEffect();
	CSpriteEffect(const CSpriteEffect& Obj);
	CSpriteEffect(CSpriteEffect&& Obj);
	virtual ~CSpriteEffect();


public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

	void SetFrameRange(std::string name, int start,int end);

public:
	void SetEffectPivot(FVector2D Pivot);
	void SetTarget(class CSceneComponent* Target)
	{
		mTarget = Target;
	}

	void SetFollowOffset(const FVector2D& off) { mFollowOffset = off; }
private:
	void AnimationEnd();

public:
	void SetAnimation(const std::string& Name, float PlayTime = 1.f, float PlayRate = 1.f, bool Loop = false, bool Reverse = false);
};

