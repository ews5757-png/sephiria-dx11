#pragma once
#include "SceneObject.h"

class CMonsterSpawner :
    public CSceneObject
{
	friend class CScene;

protected:
	CMonsterSpawner();
	CMonsterSpawner(const CMonsterSpawner& Obj);
	CMonsterSpawner(CMonsterSpawner&& Obj);
	virtual ~CMonsterSpawner();
private:

	CSharedPtr<CSceneObject> mplayer;

	CSharedPtr<class CStaticMeshComponent> mRoot;


	float mMinDist = 0.f;   // was: min
	float mMaxDist = 0.f;   // was: max
	float mSpawnTime = 0.f;
	int   mCount = 0;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

	void SetTarget(CSceneObject*);
	void SpawnMonster();

	//int FindMonster();
};

