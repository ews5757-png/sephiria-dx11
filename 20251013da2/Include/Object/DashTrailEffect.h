#pragma once
#include "SceneObject.h"
class CDashTrailEffect :
    public CSceneObject
{

	friend class CScene;

protected:
	CSharedPtr<class CSpriteComponent> mRoot;

	class CAnimation2D* mAnimation = nullptr;


	float mLifeTime = 0.3f;      
	float mRemainingTime = 0.3f; 

protected:
	CDashTrailEffect();
	CDashTrailEffect(const CDashTrailEffect& Obj);
	CDashTrailEffect(CDashTrailEffect&& Obj);
	virtual ~CDashTrailEffect();


public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

	void SetFrameRange(std::string name, int start, int end);

public:

private:
	void AnimationEnd();

public:
	void Animationinit(CSceneObject* target);
	//void SetAnimation(const std::string& Name, int start, int end);
};

