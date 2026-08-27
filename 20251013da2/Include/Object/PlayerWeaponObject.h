#pragma once
#include "SceneObject.h"

class CColliderBase;

class CPlayerWeaponObject :
	public CSceneObject
{
	friend class CScene;
	friend class CPlayerObject;

protected:
	CSharedPtr<class CSpriteComponent> mRoot;

	CSharedPtr<class CColliderOBB2D> mBody;
	CSharedPtr<class CSceneComponent> mPivot;

	CSharedPtr<CSceneObject> mOwner;

	float mFinalAttackPower = 0.f;

	

protected:
	CPlayerWeaponObject();
	CPlayerWeaponObject(const CPlayerWeaponObject& Obj);
	CPlayerWeaponObject(CPlayerWeaponObject&& Obj);
	virtual ~CPlayerWeaponObject();
public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

	void SetCollisionEnable(bool );

	void CollisionObj(const FVector3D& pos, CColliderBase* obj);

	void SwingEffect(const FVector2D& Dir,int combo,bool flip);
};