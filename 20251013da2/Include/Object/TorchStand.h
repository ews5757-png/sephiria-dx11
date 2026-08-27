#pragma once
#include "SceneObject.h"
class CTorchStand :
    public CSceneObject
{

	friend class CScene;

protected:
	CTorchStand();
	CTorchStand(const CTorchStand& Obj);
	CTorchStand(CTorchStand&& Obj);
	virtual ~CTorchStand();

protected:
	CSharedPtr<class CSpriteComponent> mRoot;
	//CSharedPtr<class CSpriteComponent> Right;

	CSharedPtr<class CColliderAABB2D> mBody;

	class CAnimation2D* mAnimation = nullptr;

	bool mAutoBasePose = true;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};

