#pragma once

#include "../Gameinfo.h"
#include "../Shader/ConstantBuffer.h" // CConstantBuffer
//레이어
struct FRenderLayer
{
	std::list <CSharedPtr<class CSceneComponent>> RenderList;
	//std::vector<CSharedPtr<class CSceneComponent>> RenderList;  // ← 이렇게 바꿔!
	int ZOrder = 0;
};


class CRenderManager
{
	DECLARE_SINGLETON(CRenderManager);

private:
	class CRenderStateManager* mStateManager = nullptr;

	//Render 할 SceneComponent를 모아둘것이다.
	//std::list <CSharedPtr<class CSceneComponent>> mRenderList;
	// map , unorderd_map
	// Z오더값, 그려질 목록 
	std::map<int, FRenderLayer*>			mLayerList;
	//어떤 레이어가 있고, 그 해당 레이어의 Z오더값 
	std::unordered_map<std::string, int>	mLayerNameList;


	//정렬 타입
	ERenderSortType mRenderSortType = ERenderSortType::Y;

	//기본 샘플러
	ID3D11SamplerState* mSampler = nullptr;

	//알파블랜드
	class CRenderState* mAlphaBlend = nullptr;
	//레스터라이즈스테이트
	class CRenderState* mRasterizer = nullptr;
	//뎁스 검사 X 뎁스스텐실스테이트
	class CRenderState* mDepthDisable = nullptr;
	float mBloomIntensity = 2.5f;

public:
	FRenderTarget mSceneRT;
	FRenderTarget mBrightRT;
	FRenderTarget mBlurA;
	FRenderTarget mBlurB;

	int mBloomDebug = 0;

	ID3D11SamplerState* mPostSampler = nullptr;

	ID3D11BlendState* mBlendWriteAll = nullptr;

	ID3D11Texture2D* mSceneDepthTex = nullptr;

	ID3D11DepthStencilView* mSceneDepthDSV = nullptr;


	FPostCBufferInfo mPostInfo = {};
	CConstantBuffer* mPostCB = nullptr;

	class CShader* mCopyShader = nullptr;
	class CShader* mBrightShader = nullptr;
	class CShader* mBlurShader = nullptr;
	class CShader* mCompositeShader = nullptr;
	class CMesh* mFullScreenMesh = nullptr;

	void DebugBlitSRV(ID3D11ShaderResourceView* srv);
	void DumpCurRTV(const char* tag) const;

public:
	void BeginSceneRT();
	void EndToBackBuffer();
	ID3D11ShaderResourceView* GetSceneSRV() const
	{
		return mSceneRT.SRV;
	}
	void RenderCopySceneToBackBuffer();
	void UpdatePostCB_Threshold(float threshold);
	void UpdatePostCB_Blur(int w, int h, float dx, float dy);
	void UpdatePostCB_Intensity(float intensity);

	bool CreateSceneDepth(int w, int h);



public:
	//RenderList 등록함수
	void AddRenderList(class CSceneComponent* Component);
	void RemoveRenderList(class CSceneComponent* Component); // [추가]
	void ClearRenderList();

	//레이어 생성
	bool CreateRenderLayer(const std::string& Name, int ZOrder);
	//레이어 탐색
	FRenderLayer* FindLayer(const std::string& Name);

	void RenderBloom();
	void SetViewport(int w, int h);

public:
	class CRenderStateManager* GetStateManager()
	{
		return mStateManager;
	}

	void SetBloomIntensity(float f) { mBloomIntensity = f; }

	void UpdatePostCB_Tone(float haze, float desat, float contrast,FVector3D v3 = FVector3D::Zero);

public:
	bool Init();
	void Render();


private:
	static bool SortY(const CSharedPtr<class CSceneComponent>& Src, const CSharedPtr<class CSceneComponent>& Dest);

	static bool SortOrder(const CSharedPtr<CSceneComponent>& a, const CSharedPtr<CSceneComponent>& b);

	static bool SortAlpha(const CSharedPtr<class CSceneComponent>& Src, const CSharedPtr<class CSceneComponent>& Dest);

};

