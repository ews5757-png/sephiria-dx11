#include "Animation2DManager.h"
#include "Animation2DData.h"
#include "../../Animation/Animation2D.h"
#include "../../Asset/Texture/Texture.h"

CAnimation2DManager::CAnimation2DManager()
{
}

CAnimation2DManager::~CAnimation2DManager()
{
	//
	CAnimation2D::DestroyCBuffer();
}

bool CAnimation2DManager::Init()
{
	//애니메이션용 상수 버퍼 생성
	CAnimation2D::CreateCBuffer();

	//플레이어
	LoadAnimationAssetsforFrame("PlayerIdle_Idle_Lower", "PlayerIdle_Idle_Lower", L"Texture\\Sephiria\\Player\\Player_Adventurer\\idle\\Player_Adventurer_Idle_Lower");
	LoadAnimationAssetsforFrame("PlayerIdle_Idle_Upper", "PlayerIdle_Idle_Upper", L"Texture\\Sephiria\\Player\\Player_Adventurer\\idle\\Player_Adventurer_Idle_Upper");
	LoadAnimationAssetsforFrame("Move_Lower", "Move_Lower", L"Texture\\Sephiria\\Player\\Player_Adventurer\\move\\Player_Adventurer_Move_Lower");
	LoadAnimationAssetsforFrame("Move_Upper", "Move_Upper", L"Texture\\Sephiria\\Player\\Player_Adventurer\\move\\Player_Adventurer_Move_Upper");\
	//나무
	LoadAnimationAssetsforFrame("BigTree", "BigTree", L"Texture\\Sephiria\\Tree\\BigTree");
	//몬스터
	LoadAnimationAssetsforFrame("Stone_Mole_Idle", "Stone_Mole_Idle", L"Texture/Sephiria/Monster/Stone_Mole/melee/Idle/Stone_Mole_Idle");
	LoadAnimationAssetsforFrame("Stone_Mole_Move", "Stone_Mole_Move", L"Texture/Sephiria/Monster/Stone_Mole/melee/Move/Stone_Mole_Move");

	//MoleGuard
	LoadAnimationAssetsforFrame("MoleGuard_Airborne", "MoleGuard_Airborne", TEXT("Texture/Sephiria/Monster/MoleGuard/Airborne/MoleGuard_Airborne"));
	LoadAnimationAssetsforFrame("MoleGuard_Attack", "MoleGuard_Attack", TEXT("Texture/Sephiria/Monster/MoleGuard/Attack/MoleGuard_Attack"));
	LoadAnimationAssetsforFrame("MoleGuard_Down", "MoleGuard_Down", TEXT("Texture/Sephiria/Monster/MoleGuard/Down/MoleGuard_Down"));
	LoadAnimationAssetsforFrame("MoleGuard_Idle", "MoleGuard_Idle", TEXT("Texture/Sephiria/Monster/MoleGuard/Idle/MoleGuard_Idle"));
	LoadAnimationAssetsforFrame("MoleGuard_Move", "MoleGuard_Move", TEXT("Texture/Sephiria/Monster/MoleGuard/Move/MoleGuard_Move"));

	//이펙트

	LoadAnimationAssetsforFrame("Slash_Player", "Slash_Player", TEXT("Texture\\Sephiria\\Effect\\Slash\\Slash_Player"));

	//검기
	LoadAnimationAssetsforFrame("Sword_Swing1_", "Sword_Swing1_", TEXT("Texture\\Sephiria\\Effect\\SwordSwing\\Sword_Swing1_"));
	LoadAnimationAssetsforFrame("Sword_Swing2_", "Sword_Swing2_", TEXT("Texture\\Sephiria\\Effect\\SwordSwing\\Sword_Swing2_"));
	LoadAnimationAssetsforFrame("Sword_Swing3_", "Sword_Swing3_", TEXT("Texture\\Sephiria\\Effect\\SwordSwing\\Sword_Swing3_"));

	//대쉬
	LoadAnimationAssetsforFrame("DashDust", "DashDust", TEXT("Texture\\Sephiria\\Effect\\Dash\\DashDust"));
	LoadAnimationAssetsforFrame("RunDust", "RunDust", TEXT("Texture\\Sephiria\\Effect\\RunDust\\RunDust_Ver2_"));

	//타이틀
	LoadAnimationAssetsforFrame("Title_Tree", "Title_Tree", TEXT("Texture\\Sephiria\\Title\\TitleTree\\N_Title_Tree"));
	LoadAnimationAssetsforFrame("TitleTreeBegine", "TitleTreeBegine", TEXT("Texture\\Sephiria\\Title\\TitleTreeBegine\\N_Title_TreeBegin"));


	LoadAnimationAssetsforFrame("TorchStand", "TorchStand", TEXT("Texture\\Sephiria\\Object\\TorchStand\\TorchStand"));



	return true;
}

bool CAnimation2DManager::LoadAnimationAssetsforFrame(std::string AnimationName, std::string TextureName, const wchar_t* FilePath )
{
	int i = 0;
	std::vector<const TCHAR*> FileNames;

	CreateAnimation(AnimationName);
	SetAnimationTextureType(AnimationName, EAnimationTextureType::Frame);
	std::vector<TCHAR*> stringVector;
	while (true)
	{
		wchar_t buffer[256];
		wsprintf(buffer, L"%s%d%s", FilePath, i, TEXT(".png"));


		TCHAR fullPath[MAX_PATH];
		lstrcpy(fullPath, gRootPath);         
		lstrcat(fullPath, TEXT("Asset\\"));   
		lstrcat(fullPath, buffer);            

		DWORD attr = GetFileAttributes(fullPath);

		if (attr == INVALID_FILE_ATTRIBUTES)
			break;      // 파일 없으면 종료

		stringVector.emplace_back(_wcsdup(buffer));
		FileNames.emplace_back(stringVector.back());
		i++;
	}
	if (FileNames.size() == 0)
	{
		return false;
	}

	SetTexture(AnimationName, TextureName, FileNames);

	CAnimation2DData* Anim = FindAnimation(AnimationName);
	CTexture* Tex = Anim->GetTexture();
	unsigned int w = Tex->GetTexture()->Width;
	unsigned int h = Tex->GetTexture()->Height;

	AddFrameCount(AnimationName, i, 0.f, 0.f, w, h);

	for (size_t i = 0; i< stringVector.size(); ++i)
	{
		free(stringVector[i]);
	}
	stringVector.clear();


	return true;
}

bool CAnimation2DManager::CreateAnimation(const std::string& Name)
{
	// 생성해야하는데 같은 이름이 있으면 false
	if (FindAnimation(Name))
	{
		return false;
	}

	CAnimation2DData* Animation = new CAnimation2DData;

	Animation->SetName(Name);

	mAnimationMap.insert(std::make_pair(Name, Animation));

	return true;
}

class CAnimation2DData* CAnimation2DManager::FindAnimation(const std::string& Name)
{
	auto iter = mAnimationMap.find(Name);

	if (iter == mAnimationMap.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CAnimation2DManager::ReleaseAnimation(class CAsset* Animation)
{
	auto iter = mAnimationMap.find(Animation->GetName());

	if (iter != mAnimationMap.end())
	{
		mAnimationMap.erase(iter);
	}
}

bool CAnimation2DManager::SetAnimationTextureType(const std::string& Name, EAnimationTextureType Type)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetAnimationTextureType(Type);

	return true;
}

bool CAnimation2DManager::SetTexture(const std::string& Name, class CTexture* Texture)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetTexture(Texture);

	return true;
}

bool CAnimation2DManager::SetTexture(const std::string& Name, const std::string& TextureName)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetTexture(TextureName);

	return true;
}

bool CAnimation2DManager::SetTexture(const std::string& Name, const std::string& TextureName, const TCHAR* FileName)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetTexture(TextureName, FileName);

	return true;
}

bool CAnimation2DManager::SetTexture(const std::string& Name, const std::string& TextureName,
	std::vector<const TCHAR*> FileName)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetTexture(TextureName, FileName);

	return true;
}

bool CAnimation2DManager::SetTexture(const std::string& Name, const std::string& TextureName, const TCHAR* FileName,
	const TCHAR* Ext, int Count)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetTexture(TextureName, FileName, Ext, Count);

	return true;
}

bool CAnimation2DManager::SetTextureFullPath(const std::string& Name, const std::string& TextureName,
	const TCHAR* FullPath)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetTextureFullPath(TextureName, FullPath);

	return true;
}

bool CAnimation2DManager::SetTextureFullPath(const std::string& Name, const std::string& TextureName,
	const std::vector<const TCHAR*>& FullPath)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->SetTextureFullPath(TextureName, FullPath);

	return true;
}


//프레임 설정 
bool CAnimation2DManager::AddFrame(const std::string& Name, const FVector2D& Start, const FVector2D& Size)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->AddFrame(Start, Size);

	return true;
}

bool CAnimation2DManager::AddFrame(const std::string& Name, float StartX, float StartY, float SizeX, float SizeY)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->AddFrame(StartX, StartY, SizeX, SizeY);

	return true;
}

bool CAnimation2DManager::AddFrameCount(const std::string& Name, int Count, const FVector2D& Start,
	const FVector2D& Size)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->AddFrameCount(Count, Start, Size);

	return true;
}

bool CAnimation2DManager::AddFrameCount(const std::string& Name, int Count, float StartX, float StartY,
	float SizeX, float SizeY)
{
	CAnimation2DData* Animation = FindAnimation(Name);

	if (!Animation)
	{
		return false;
	}

	Animation->AddFrameCount(Count, StartX, StartY, SizeX, SizeY);

	return true;
}

CTexture* CAnimation2DManager::GetAnimationTexture(const std::string& AnimName)
{
	CAnimation2DData* anim = FindAnimation(AnimName);
	if (!anim) return nullptr;
	return anim->GetTexture();   // CAnimation2DData에 GetTexture() 있어야 함
}

int CAnimation2DManager::GetAnimationTextureCount(const std::string& AnimName)
{
	CTexture* tex = GetAnimationTexture(AnimName);
	return tex ? tex->GetTextureCount() : 0;
}
