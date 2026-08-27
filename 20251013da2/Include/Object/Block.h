#pragma once
#include "SceneObject.h"

class CBlock :
	public CSceneObject
{
	friend class CScene;

protected:
	CBlock();
	CBlock(const CBlock& Obj);
	CBlock(CBlock&& Obj);
	virtual ~CBlock();

protected:
	CSharedPtr<class CSpriteComponent> mRoot;

	CSharedPtr<class CColliderAABB2D> mBody;

	class CAnimation2D* mAnimation = nullptr;
public:

	virtual bool Init();

	virtual void Update(float DeltaTime);

};

