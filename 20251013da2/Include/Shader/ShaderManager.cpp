#include "ShaderManager.h"
#include "ColorMeshShader.h"
#include "FrameMeshShader.h"
#include "ConstantBuffer.h"
#include "StaticMeshShader.h"
#include "SpriteShader.h"
#include "TileMapShader.h"
#include "UIShader.h"
#include "TileShader.h"
#include "CopyShader.h"
#include "BrightExtractShader.h"
#include "BlurShader.h"
#include "CompositeShader.h"
#include "../Share/Log.h"

#include "../Device.h"

CShaderManager::CShaderManager()
{

}

CShaderManager::~CShaderManager()
{
	auto iter = mPixelShaderMap.begin();
	auto iterEnd = mPixelShaderMap.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}

CShader* CShaderManager::FindShader(const std::string& Name)
{
	auto iter = mShaderMap.find(Name);

	if (iter == mShaderMap.end())
	{
		return nullptr;
	}

	return (CShader*)iter->second.Get();
}

void CShaderManager::ReleaseShader(const std::string& Name)
{
	auto iter = mShaderMap.find(Name);

	if (iter != mShaderMap.end())
	{
		mShaderMap.erase(iter);
	}
}

bool CShaderManager::CreateContantBuffer(const std::string& Name, int Size, int Register, int ShaderBufferType)
{
	CConstantBuffer* CBuffer = FindCBuffer(Name);

	if (CBuffer)
	{
		return true;
	}

	CBuffer = new CConstantBuffer;

	if (!CBuffer->Init(Size, Register, ShaderBufferType))
	{
		SAFE_DELETE(CBuffer);
		return false;
	}

	mCBufferMap.insert(std::make_pair(Name, CSharedPtr<CConstantBuffer>(CBuffer)));
	return true;
}

CConstantBuffer* CShaderManager::FindCBuffer(const std::string& Name)
{
	auto iter = mCBufferMap.find(Name);
	if (iter == mCBufferMap.end())
		return nullptr;

	return iter->second;   // ✅ Get() 제거
}

void CShaderManager::ReleaseCBuffer(const std::string& Name)
{
	auto iter = mCBufferMap.find(Name);

	if (iter != mCBufferMap.end())
	{
		mCBufferMap.erase(iter);
	}
}

bool CShaderManager::Init()
{
	CreateShader<CColorMeshShader>("ColorMeshShader");

	CreateShader<CFrameMeshShader>("FrameMeshShader");

	CreateShader<CStaticMeshShader>("StaticMeshShader");

	CreateShader<CSpriteShader>("SpriteShader");

	CreateShader<CTileMapShader>("TileMapShader");

	CreateShader<CTileShader>("TileShader");


	CreateShader<CUIShader>("UIShader");

	CreateShader<CCopyShader>("CopyShader");

	CreateShader<CBrightExtractShader>("BrightExtractShader");

	CreateShader<CBlurShader>("BlurShader");

	CreateShader<CCompositeShader>("CompositeShader");


	// 전용 픽셀쉐이더만 먼저 만들어둔다. 
	if (!LoadPixelShader("DefaultMaterialShader", "DefaultMaterialPS", TEXT("Mesh.fx")))
	{
		return false;
	}


	//우리가 사용할 상수버퍼도 만들어둔다. 
	CreateContantBuffer("Transform", sizeof(FTransformCBufferInfo), 0, EShaderBufferType::Vertex);

	//머티리얼 용 상수버퍼  
	CreateContantBuffer("Material", sizeof(FMaterialCBufferInfo), 1, EShaderBufferType::Pixel);

	//애니메이션용 상수버퍼  
	CreateContantBuffer("Animation2D", sizeof(FAnimation2DBufferInfo), 2, EShaderBufferType::Vertex);

	//스프라이트 전용 상수버퍼 
	CreateContantBuffer("Sprite", sizeof(FSpriteCBufferInfo), 3, EShaderBufferType::Pixel);

	//UI용 상수버퍼
	CreateContantBuffer("UI", sizeof(FUICBufferInfo), 3, EShaderBufferType::Vertex | EShaderBufferType::Pixel);

	CreateContantBuffer("TileMap", sizeof(FTileMapCBufferInfo), 3, EShaderBufferType::Vertex);


	//충돌체에 사용할 상수버퍼 
	CreateContantBuffer("Collider", sizeof(FColliderCBufferInfo), 4, EShaderBufferType::Pixel);

	CreateContantBuffer("Post", sizeof(FPostCBufferInfo), 5,	EShaderBufferType::Pixel);


	if (!FindCBuffer("Post"))
		CLog::PrintLog("Post CBuffer NOT FOUND right after CreateContantBuffer");
	else
		CLog::PrintLog("Post CBuffer OK");

	return true;
}

bool CShaderManager::LoadPixelShader(const std::string& Name, const char* EntryName, const TCHAR* FileName)
{
	if (FindPixelShader(Name))
	{
		return true;
	}

	//최종 경로가 나온다.
	TCHAR FullPath[MAX_PATH] = {};
	lstrcpy(FullPath, TEXT("../Bin/Shader/"));
	lstrcat(FullPath, FileName);

	//컴파일 했는데 에러가나면 여기에 들어올것이다. 
	ID3DBlob* ErrorBlob = nullptr;

	unsigned int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif // _DEBUG

	FMaterialPixelShader* mps = new FMaterialPixelShader;

	if (FAILED(D3DCompileFromFile(FullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryName, "ps_5_0", Flag, 0, &mps->Blob, &ErrorBlob)))
	{
#ifdef _DEBUG
		char ErrorText[512] = {};
		strcpy_s(ErrorText, (const char*)ErrorBlob->GetBufferPointer());
		strcat_s(ErrorText, "\n");
		OutputDebugStringA(ErrorText);
#endif // _DEBUG

		return false;
	}

	//컴파일이 성공했다. 
	//성공했다면 쉐이더 객체 만들어야한다. 
	if (FAILED(CDevice::GetInst()->GetDevice()->CreatePixelShader(mps->Blob->GetBufferPointer(), mps->Blob->GetBufferSize(), nullptr, &mps->PS)))
	{
		return false;
	}

	mPixelShaderMap.insert(std::make_pair(Name, mps));

	return true;
}

const FMaterialPixelShader* CShaderManager::FindPixelShader(const std::string& Name)
{
	auto iter = mPixelShaderMap.find(Name);

	if (iter == mPixelShaderMap.end())
	{
		return nullptr;
	}

	return iter->second;
}
