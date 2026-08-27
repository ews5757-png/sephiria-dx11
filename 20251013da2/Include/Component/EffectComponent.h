#pragma once
#include "SceneComponent.h"
#include "SceneComponent.h"

class CEffectComponent :
    public CSceneComponent
{
	friend class CScene;
	friend class CSceneObject;
protected:
	CEffectComponent();
	CEffectComponent(const CEffectComponent& Com);
	CEffectComponent(CEffectComponent&& Com);
	virtual ~CEffectComponent();

};

