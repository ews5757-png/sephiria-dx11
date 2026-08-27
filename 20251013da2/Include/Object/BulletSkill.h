#pragma once
#include "SceneObject.h"

class CBulletSkill :
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

protected:

	//CSharedPtr<class CSceneComponent> mOwner;

	CSharedPtr<class CSceneComponent> mRoot;
	CSharedPtr<class CMovementComponent> mMovement;
	CSharedPtr<class CRotationComponent> mRotate;

	CSharedPtr<class CMovementComponent> mMoveBullet1;
	int mbulletCount = 8;
	float mDig = 180;
	float mSpeedScale = 0;

	std::vector<CSharedPtr<class CSceneComponent>> mpivots;
	std::vector<CSharedPtr<class CStaticMeshComponent>> mSkillObjects;

protected:
	CBulletSkill();
	CBulletSkill(const CBulletSkill& Obj);
	CBulletSkill(CBulletSkill&& Obj);
	virtual ~CBulletSkill();

public:
	void SetUpdateComponent(class CSceneComponent* Target);
	virtual bool Init();
	virtual void Update(float DeltaTime);


};

