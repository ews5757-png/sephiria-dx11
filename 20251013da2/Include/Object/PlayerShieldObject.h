#pragma once
#include "SceneObject.h"

class CPlayerShieldObject :
	public CSceneObject
{
	friend class CScene;
	friend class CPlayerObject;

protected:
	CSharedPtr<class CSpriteComponent> mRoot;
	//CSharedPtr<class CStaticMeshComponent> mRoot;

	CSharedPtr<class CColliderOBB2D> mBody;

	CSharedPtr<CSceneObject> mOwner;

protected:
	CPlayerShieldObject();
	CPlayerShieldObject(const CPlayerShieldObject& Obj);
	CPlayerShieldObject(CPlayerShieldObject&& Obj);
	virtual ~CPlayerShieldObject();
public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};