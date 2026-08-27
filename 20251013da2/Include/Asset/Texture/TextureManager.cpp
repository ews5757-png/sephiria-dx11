#include "TextureManager.h"


#include "Texture.h"

#include "../../Share/Log.h"

CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
}

bool CTextureManager::Init()
{
	LoadTexture("Shield_Tier1", TEXT("Texture/Sephiria/Weapon/Shield_Tier1.png"));
	LoadTexture("Sword_Tier1", L"Texture/Sephiria/Weapon/Sword_Tier1.png");
	LoadTexture("InventoryBase", TEXT("Texture/Sephiria/UI/InventoryBase_New1.png"));
	LoadTexture("shieldRange", TEXT("Texture\\Sephiria\\Effect\\AOE/shieldRange.png"));
	LoadTexture("WarningRangeLine", TEXT("Texture/Sephiria/Effect/AOE/AOEWarningRangeLine.png"));
	LoadTexture("InventorySlot", TEXT("Texture/Sephiria/UI/InventorySlot.png"));

	LoadTexture("N_Title", TEXT("Texture\\Sephiria\\Title\\N_Title.png"));
	LoadTexture("N_Title_Back", TEXT("Texture\\Sephiria\\Title\\N_Title_Back.png"));
	LoadTexture("Star", TEXT("Texture\\Sephiria\\Title\\Star.png"));
	LoadTexture("white", TEXT("Texture\\white.png"));


	
	LoadTexture("AskardStatue", TEXT("Texture\\Sephiria\\Object\\AskardStatue.png"));

	LoadTexture("StoneTablet_Metastasis", TEXT("Texture/Sephiria/UI/StoneTablet/StoneTablet_Metastasis.png"));


	LoadTexture("Blur", TEXT("Texture\\Blur.png"));
	LoadTexture("TileMap", TEXT("Texture/Floors.png"));

	LoadTexture("TileMap2", TEXT("Texture/Floors2.png"));

	return true;
}

//파일 이름 중복이 없어야 한다.
bool CTextureManager::LoadTexture(const std::string& Name, const TCHAR* FileName)
{
	CTexture* Texture = FindTexture(Name);

	if (Texture)
	{
		return true;
	}

	Texture = new CTexture;

	if (!Texture->LoadTexture(FileName))
	{
		std::wstring ws(FileName ? FileName : L"(null)");
		std::string  path(ws.begin(), ws.end());

		CLog::PrintLog(std::string("LoadTexture FAIL : ") + Name + " file=" + path);

		SAFE_DELETE(Texture);
		return false;
	}

	mTextureMap.insert(std::make_pair(Name, Texture));

	return true;
}

bool CTextureManager::LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath)
{
	CTexture* Texture = FindTexture(Name);

	if (Texture)
	{
		return true;
	}

	Texture = new CTexture;

	if (!Texture->LoadTextureFullPath(FullPath))
	{
		SAFE_DELETE(Texture);
		return false;
	}

	mTextureMap.insert(std::make_pair(Name, Texture));

	return true;
}

bool CTextureManager::LoadTexture(const std::string& Name, const std::vector<const TCHAR*>& FileName)
{
	CTexture* Texture = FindTexture(Name);

	if (Texture)
	{
		return true;
	}

	Texture = new CTexture;

	if (!Texture->LoadTexture(FileName))
	{
		SAFE_DELETE(Texture);
		return false;
	}

	mTextureMap.insert(std::make_pair(Name, Texture));

	return true;
}

bool CTextureManager::LoadTextureFullPath(const std::string& Name, const std::vector<const TCHAR*>& FullPath)
{
	CTexture* Texture = FindTexture(Name);

	if (Texture)
	{
		return true;
	}

	Texture = new CTexture;

	if (!Texture->LoadTextureFullPath(FullPath))
	{
		SAFE_DELETE(Texture);
		return false;
	}

	mTextureMap.insert(std::make_pair(Name, Texture));

	return true;
}

bool CTextureManager::LoadTexture(const std::string& Name, const TCHAR* FileName, const TCHAR* Ext, int Count)
{
	CTexture* Texture = FindTexture(Name);

	if (Texture)
	{
		return true;
	}

	Texture = new CTexture;

	if (!Texture->LoadTexture(FileName, Ext, Count))
	{
		SAFE_DELETE(Texture);
		return false;
	}

	mTextureMap.insert(std::make_pair(Name, Texture));

	return true;
}

class CTexture* CTextureManager::FindTexture(const std::string& Name)
{
	auto iter = mTextureMap.find(Name);

	if (iter == mTextureMap.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CTextureManager::ReleaseTexture(class CAsset* Texture)
{
	auto iter = mTextureMap.find(Texture->GetName());

	if (iter != mTextureMap.end())
	{
		// 다른데서 사용하고 있을때 해당 에셋을 지우면 안되니까!
		if (iter->second->GetRefCount() == 1)
		{
			mTextureMap.erase(iter);
		}
	}
}
