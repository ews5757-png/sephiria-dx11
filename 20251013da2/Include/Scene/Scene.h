#pragma once

#include "../Gameinfo.h"

//월드를 구상하는 클래스이다. 
class CScene
{
	friend class CSceneManager;

protected:
	CScene();
	virtual ~CScene();

protected:
	//입력
	class CInput* mInput = nullptr;
	//카메라 매니저 
	class CCameraManager* mCameraManager = nullptr;
	//충돌
	class CSceneCollision* mCollision = nullptr;
	//에셋
	class CSceneAssetManager* mAssetManager = nullptr;
	//오브젝트
	std::list<CSharedPtr<class CSceneObject>> mObjList;
	// UI
	class CSceneUIManager* mUIManager = nullptr;



	bool mDebugQuadTree = true;

public:
	/*std::list<CSharedPtr<class CSceneObject>>& GetObjList()
	{
		return mObjList;
	}*/
	class CInput* GetInput() const
	{
		return mInput;
	}

	class CCameraManager* GetCameraManager() const
	{
		return mCameraManager;
	}

	class CSceneCollision* GetCollision() const
	{
		return mCollision;
	}
	class CSceneAssetManager* GetAssetManager() const
	{
		return mAssetManager;
	}

	class CSceneUIManager* GetUIManager() const
	{
		return mUIManager;
	}




public:
	virtual bool Init();
	virtual bool Init(const char* FileName);
	virtual void Input(float DeltaTime);
	virtual void PreUpdate(float DeltaTime);
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Collision(float DeltaTime);
	//virtual void PreRender();
	virtual void Render();
	//virtual void PostRender();

	virtual void RenderUI();
	virtual void EndFrame();
public:
	template<typename T>
	T* CreateObj(const std::string& Name)
	{
		T* Obj = new T;

		Obj->mScene = this;
		Obj->SetName(Name);

		if (!Obj->Init())
		{
			SAFE_DELETE(Obj);
			return nullptr;
		}

		mObjList.push_back(Obj);
		return Obj;
	}

	template<typename T>
	void FindObjectsFromType(std::list<CSharedPtr<T>>& result)
	{
		auto iter = mObjList.begin();
		auto iterEnd = mObjList.end();

		for (; iter != iterEnd; ++iter)
		{
			T* Obj = dynamic_cast<T*>((*iter).Get());

			if (Obj)
			{
				result.push_back(Obj);
			}
		}

	}
	template<typename T>
	std::list<CSharedPtr<class CSceneObject>> FindObjectsforClass()
	{
		std::list<CSharedPtr<class CSceneObject>> templist;
		for (auto& object : mObjList)
		{
			if (dynamic_cast<T*>(object.Get()))
			{
				templist.push_back(object);
			}
		}
		return templist;
	}

};

