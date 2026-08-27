#pragma once
#include "SceneObject.h"


class CGravitySkill : public CSceneObject
{
	friend class CScene;
	

	CSharedPtr<class CStaticMeshComponent> mRoot;

	bool mreverse = false;

	//std::vector<CSharedPtr<class CStaticMeshComponent>> targets;

protected:
	CGravitySkill();
	CGravitySkill(const CGravitySkill& Obj);
	CGravitySkill(CGravitySkill&& Obj);
	virtual ~CGravitySkill();

public:
	virtual bool Init() override;
	//virtual void Update(float DeltaTime) override;

	virtual void PreUpdate(float DeltaTime) override;
	
};

