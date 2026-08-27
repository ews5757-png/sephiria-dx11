#pragma once

#include "../Gameinfo.h"
#include "Shader.h"

// 머티리얼용 픽셀쉐이더 구조체 
struct FMaterialPixelShader
{
	//
	ID3DBlob* Blob = nullptr;
	ID3D11PixelShader* PS = nullptr;

	~FMaterialPixelShader()
	{
		SAFE_RELEASE(Blob);
		SAFE_RELEASE(PS);
	}
};

class CShaderManager
{
	DECLARE_SINGLETON(CShaderManager);

private:
	//쉐이더들 저장하는 자료구조 
	std::unordered_map<std::string, CSharedPtr<CShader>> mShaderMap;

	//상수버퍼 저장하는 자료구조 
	std::unordered_map<std::string, CSharedPtr<class CConstantBuffer>> mCBufferMap;

	//머티리얼이 사용할 픽셀쉐이더를 저장할 자료구조 
	std::unordered_map<std::string, FMaterialPixelShader*> mPixelShaderMap;

public:
	//쉐이더 클래스 생성 해주기
	template <typename T>
	bool CreateShader(const std::string& Name)
	{
		CShader* Shader = FindShader(Name);

		if (Shader)
		{
			return true;
		}

		Shader = new T;

		if (!Shader->Init())
		{
			SAFE_DELETE(Shader);
			return false;
		}

		mShaderMap.insert(std::make_pair(Name, Shader));
		return true;
	}

	class CShader* FindShader(const std::string& Name);
	void ReleaseShader(const std::string& Name);
public:
	bool CreateContantBuffer(const std::string& Name, int Size, int Register, int ShaderBufferType);
	class CConstantBuffer* FindCBuffer(const std::string& Name);
	void ReleaseCBuffer(const std::string& Name);

public:
	bool Init();

	//픽셀쉐이더 생성 및 찾기 함수 
	bool LoadPixelShader(const std::string& Name, const char* EntryName, const TCHAR* FileName);
	const FMaterialPixelShader* FindPixelShader(const std::string& Name);

};

