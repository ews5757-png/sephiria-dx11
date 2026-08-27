#pragma once
#include "SceneObject.h"

class CWarningLine :
    public CSceneObject
{
    friend class CScene;

    CSharedPtr<class CSceneComponent> mRoot;
    CSharedPtr<class CSpriteComponent> mOutLeft;
    CSharedPtr<class CSpriteComponent> mOutRight;
    CSharedPtr<class CSpriteComponent> minnerLeft;
    CSharedPtr<class CSpriteComponent> minnerRight;

    float mElapsedTime = 0;

    float mMaxLinewidth = 6.f;
    float mMaxLineLength = 1.5;
    float mAnimationDuration = 0.5;
    float margin = 0.25;


protected:
    CWarningLine();
    CWarningLine(const CWarningLine& Obj);
    CWarningLine(CWarningLine&& Obj);
    virtual ~CWarningLine();


public:
    virtual bool Init();
    virtual void Update(float DeltaTime);

    void startanime();

    void SetAnimationDruation(float a) { mAnimationDuration = a; }
    void SetLinewidth(float a) { mMaxLinewidth = a; }
    void SetLength(float a) { mMaxLineLength = a; }
};

