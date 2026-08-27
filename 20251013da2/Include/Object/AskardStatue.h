#pragma once
#include "SceneObject.h"
class CAskardStatue :
    public CSceneObject
{
public:
	CAskardStatue();
	CAskardStatue(const CAskardStatue& Obj);
	CAskardStatue(CAskardStatue&& Obj);
	virtual ~CAskardStatue();

protected:
	CSharedPtr<class CSpriteComponent> mRoot;

	CSharedPtr<class CColliderAABB2D> mBody;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};

