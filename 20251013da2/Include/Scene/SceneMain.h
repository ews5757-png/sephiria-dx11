#pragma once
#include "Scene.h"

class CInventoryWindow;
class CInventoryComponent;

class CSceneMain : public CScene
{
	friend class CSceneManager;

protected:
	CInventoryWindow* mInventoryWindow = nullptr;
	CSceneMain();
	virtual ~CSceneMain();


	class CPlayerObject* mPlayer;

public:
	virtual bool Init();

	void ToggleinventoryUI();

	void SetInventoty(CInventoryComponent* _inven);

	void UpdateInventoty();

	void MoveItem(int f, int s);
};

