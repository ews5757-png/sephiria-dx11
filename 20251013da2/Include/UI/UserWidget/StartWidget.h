#pragma once
#include "UserWidget.h"
class CStartWidget :
	public CUserWidget
{
	friend class CSceneUIManager;

protected:
	CStartWidget();
	virtual ~CStartWidget();

protected:
	//CSharedPtr<class CSpriteComponent> mRoot;

	//class CAnimation2D* mAnimation = nullptr;

	FResolution RS;

	CSharedPtr<class CButton> mButton;
	CSharedPtr<class CButton> mEditorButton;

public:
	virtual bool Init();
	void Update(float DeltaTime) override;

private:
	void StartButtonClick();
	void StartEidtButtonClick();

	void AddTree();
	bool mRequestAddTree = false;
	void RequestAddTree();

};

