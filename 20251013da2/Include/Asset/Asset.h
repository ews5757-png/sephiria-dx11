#pragma once
#include "../Share/Object.h"
class CAsset abstract :
    public CObject
{
public:
    CAsset();
    virtual ~CAsset();
protected:
    //에셋도 구분이 필요하다.
    //메쉬 또는 텍스챠 등등
    //스테이지 별로 사용할 에셋을 관리해보자
    class CScene* mScene = nullptr;
    //관리할 에셋 이름
    std::string mName;
    
    EAssetType mAssetType;


public:
	const std::string& GetName() const
	{
		return mName;
	}

	EAssetType GetAssetType() const
	{
		return mAssetType;
	}

	void SetName(const std::string& Name)
	{
		mName = Name;
	}

    
};

