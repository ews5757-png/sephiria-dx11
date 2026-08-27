#pragma once
#include "MonsterObject.h"
class CMonster2Object :
    public CMonsterObject
{
	friend class CScene;
	float mCount = 0;

	virtual bool Init();
	virtual void Update(float DeltaTime);
};

