#pragma once
#include "SceneObject.h"

class CBulletObject :
	public CSceneObject
{
	friend class CScene;

private:
	float mSpeed = 200.f;

public:
	float GetBulletSpeed() const
	{
		return mSpeed;
	}

	void SetBulletSpeed(const float& Speed)
	{
		mSpeed = Speed;
	}

	void  SetBulletCollisionProfile(const std::string& Name);

protected:
	CSharedPtr<class CSpriteComponent> mRoot;
	//CSharedPtr <class CStaticMeshComponent> mRoot;

	CSharedPtr<class CColliderAABB2D> mBody;
	CSharedPtr<class CMovementComponent> mMovement;

protected:
	CBulletObject();
	CBulletObject(const CBulletObject& Obj);
	CBulletObject(CBulletObject&& Obj);
	virtual ~CBulletObject();
public:
	CColliderAABB2D* GetBody()
	{
		return mBody;
	};

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

private:
	void CollisionBullet(const FVector3D& HitPoint, class CColliderBase* Dest);

};

