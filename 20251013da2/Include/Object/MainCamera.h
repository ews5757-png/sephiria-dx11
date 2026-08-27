#pragma once
#include "SceneObject.h"
class CMainCamera :
    public CSceneObject
{
	friend class CScene;

protected:
	CMainCamera();
	CMainCamera(const CMainCamera& Obj);
	CMainCamera(CMainCamera&& Obj);
	virtual ~CMainCamera();

protected:
	class CPlayerObject* mOwner;

	class CCameraComponent* mCamera;

	class CMovementComponent* mMovement;
public:
	void SetOwner(CPlayerObject* Owner)
	{
		mOwner = Owner;
	}
public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};

