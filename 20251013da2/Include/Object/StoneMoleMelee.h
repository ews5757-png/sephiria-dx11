
#pragma once
#include "MonsterBase.h"

class CStoneMoleMelee :
	public CMonsterBase
{
	friend class CScene;


	bool Init() override;
	void Update(float Deltatime) override;
};