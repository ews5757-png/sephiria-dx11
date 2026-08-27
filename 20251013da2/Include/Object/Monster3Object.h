#pragma once
#include "MonsterObject.h"
class CMonster3Object :
    public CMonsterObject
{
	friend class CScene;

	virtual bool Init();
	virtual void Update(float DeltaTime);
};

