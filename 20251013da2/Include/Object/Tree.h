#pragma once
#include "SceneObject.h"
class CTree
	: public CSceneObject
{

	friend class CScene;

protected:
	CTree();
	CTree(const CTree& Obj);
	CTree(CTree&& Obj);
	virtual ~CTree();

protected:
	CSharedPtr<class CSpriteComponent> mRoot;

	CSharedPtr<class CColliderAABB2D> mBody;

	class CAnimation2D* mAnimation = nullptr;

	bool mAutoBasePose = true;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};

