#pragma once
#include "SceneObject.h"

class CWallshot :
	public CSceneObject
{
	friend class CScene;

protected:
	CSharedPtr<class CStaticMeshComponent> mRoot;
	CSharedPtr<class CColliderSphere2D> mBody;
	CSharedPtr<class CMovementComponent> mMovement;

	CSharedPtr<class CRotationComponent> mRotation;

	//std::list<CSharedPtr<CSceneObject>> objects;

	int count = 1;


protected:
	CWallshot();
	CWallshot(const CWallshot& Obj);
	CWallshot(CWallshot&& Obj);
	virtual ~CWallshot();
public:
	CMovementComponent* GetMovenent()
	{
		return mMovement;
	};
	CRotationComponent* GetRotation()
	{
		return mRotation;
	};

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

private:
	void CollisionBullet(const FVector3D& HitPoint, class CColliderBase* Dest);

};

