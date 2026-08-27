#pragma once

#include "../../Gameinfo.h"

class CAnimation2DManager
{
    friend class CAssetManager;

private:
    CAnimation2DManager();
    ~CAnimation2DManager();

private:
    std::unordered_map<std::string, CSharedPtr<class CAnimation2DData>> mAnimationMap;

public:
    bool Init();

    bool CreateAnimation(const std::string& Name);
    class CAnimation2DData* FindAnimation(const std::string& Name);
    void ReleaseAnimation(class CAsset* Animation);

    //애니메이션에 각각 설정 해주기
    bool SetAnimationTextureType(const std::string& Name, EAnimationTextureType Type);

    //텍스쳐 불러오기 
    //텍스쳐로 바로 넣어주기
    bool SetTexture(const std::string& Name, class CTexture* Texture);

    //텍스쳐 이름으로 찾아와서 넣어주기
    bool SetTexture(const std::string& Name, const std::string& TextureName);

    //이름이랑 파일경로 넣어주기
    bool SetTexture(const std::string& Name,
        const std::string& TextureName, const TCHAR* FileName);

    //이름, 배열
    bool SetTexture(const std::string& Name,
        const std::string& TextureName,
        std::vector<const TCHAR*> FileName);

    //이름 파일경로 확장자 갯수
    bool SetTexture(const std::string& Name,
        const std::string& TextureName,
        const TCHAR* FileName, const TCHAR* Ext, int Count);

    // 풀경로
    bool SetTextureFullPath(const std::string& Name,
        const std::string& TextureName, const TCHAR* FullPath);
    // 풀경로 배열 
    bool SetTextureFullPath(const std::string& Name,
        const std::string& TextureName, const std::vector<const TCHAR*>& FullPath);

    bool LoadAnimationAssetsforFrame(std::string AnimationName, std::string TextureName, const wchar_t* FilePath);

    ////
    //FAnimationFrame 를 만들어주는 함수를 만들어주자!
    bool AddFrame(const std::string& Name, const FVector2D& Start, const FVector2D& Size);
    bool AddFrame(const std::string& Name, float StartX, float StartY, float SizeX, float SizeY);
    bool AddFrameCount(const std::string& Name, int Count, const FVector2D& Start, const FVector2D& Size);
    bool AddFrameCount(const std::string& Name, int Count, float StartX, float StartY, float SizeX, float SizeY);

    class CTexture* GetAnimationTexture(const std::string& AnimName);
    int       GetAnimationTextureCount(const std::string& AnimName);

};

