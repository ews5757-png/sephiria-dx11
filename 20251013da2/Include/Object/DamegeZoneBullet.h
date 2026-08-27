#pragma once

#include "SceneObject.h"

class CDamegeZoneBullet:
	public CSceneObject
{
	friend class CScene;

protected:
	CSharedPtr<class CStaticMeshComponent> mRoot;
	CSharedPtr<class CColliderSphere2D> mBody;

	std::list<CSharedPtr<CSceneObject>> objects;

	float mElapsedTime = 0.f;
	float mDelayTime = 1.f;   // 3초 뒤 실행


protected:
	CDamegeZoneBullet();
	CDamegeZoneBullet(const CDamegeZoneBullet& Obj);
	CDamegeZoneBullet(CDamegeZoneBullet&& Obj);
	virtual ~CDamegeZoneBullet();
public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

private:
	void CollisionBullet(const FVector3D& HitPoint, class CColliderBase* Dest);

	void Attack();
};

