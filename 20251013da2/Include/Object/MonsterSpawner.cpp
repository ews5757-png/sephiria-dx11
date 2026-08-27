#include "MonsterSpawner.h"
#include "../Scene/Scene.h"
#include "../Component/SceneComponent.h"
#include "../Component/StaticMeshComponent.h"


#include <cstdlib>

CMonsterSpawner::CMonsterSpawner()	:CSceneObject()
{
}

CMonsterSpawner::CMonsterSpawner(const CMonsterSpawner& Obj) : CSceneObject(Obj)
{
}

CMonsterSpawner::CMonsterSpawner(CMonsterSpawner&& Obj) : CSceneObject(Obj)
{
}

CMonsterSpawner::~CMonsterSpawner()
{
}

bool CMonsterSpawner::Init()
{
	CSceneObject::Init();

	mRoot = CreateComponent<CStaticMeshComponent>();

	mRoot->SetMesh("CenterRect");
	mRoot->SetShader("ColorMeshShader");
	mRoot->SetWorldScale(1.f, 1.f);

	SetRootComponent(mRoot);
	return true;
}
void CMonsterSpawner::Update(float DeltaTime)
{
	//mSpawnTime += DeltaTime;
	//if (mSpawnTime > 3)
	//{
	//	mSpawnTime -= 3;
	//	if (FindMonster() < 3)
	//	{
	//		SpawnMonster();
	//	}
	//}
}
void CMonsterSpawner::SetTarget(CSceneObject* player)
{
	mplayer = player;
}

void CMonsterSpawner::SpawnMonster()
{
	//CMonsterObject* Monster = mScene->CreateObj<CMonster2Object>("Monster3");

	//Monster->SetTarget(mplayer); 
	//FVector2D random (rand() % 1000 - 500, rand() % 600 - 300);
	//Monster->SetWorldPos(random);
}

//int CMonsterSpawner::FindMonster()
//{
//	//std::list<CSharedPtr<CSceneObject>> list = mScene->FindObjectsforClass<CMonsterObject>();	
//
//	//return list.size();
//}


